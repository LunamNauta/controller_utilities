#include "controller_utilities/xbox/controller.hpp"
#include "controller_utilities/xbox/constants.hpp"
#include "controller_utilities/utilities.hpp"

#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>

//TODO: See if 'epoll' is more efficient than poll in this case - probably not
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>

namespace Input{
namespace Xbox{

const float JOYSTICK_NEG_MAX = -(float)std::numeric_limits<short>::min();
const float JOYSTICK_POS_MAX = (float)std::numeric_limits<short>::max();
const float TRIGGER_MAX = 1023.0f;

std::unordered_set<std::shared_ptr<Controller_Handle>> known_controllers;
std::unordered_map<std::shared_ptr<Controller_Handle>, pollfd> auto_updated_controllers;
bool setup_cleanup = false;

std::thread polling_thread;
bool setup_polling_thread = false;
bool kill_polling_thread = false;

void update_from_input_event(std::shared_ptr<Controller_Handle> controller, const input_event& event){
    controller->guard.lock();
    if (event.type == EV_KEY || event.type == EV_ABS){
        for (std::size_t a = 0; a < EVENT_CODES[0][(std::size_t)controller->console_type].size(); a++){
            if (event.code == EVENT_CODES[(std::size_t)controller->controller_type][(std::size_t)controller->console_type][a]){
                switch (static_cast<ControllerInput>(a)){
                    case ControllerInput::RIGHT_JOYSTICK:{controller->state.rj_d = event.value; break;}
                    case ControllerInput::LEFT_JOYSTICK:{controller->state.lj_d = event.value; break;}
                    case ControllerInput::START:{controller->state.s = event.value; break;}
                    case ControllerInput::MENU:{controller->state.m = event.value; break;}
                    case ControllerInput::HOME:{controller->state.h = event.value; break;}
                    case ControllerInput::RB:{controller->state.rb = event.value; break;}
                    case ControllerInput::LB:{controller->state.lb = event.value; break;}
                    case ControllerInput::A:{controller->state.a = event.value; break;}
                    case ControllerInput::B:{controller->state.b = event.value; break;}
                    case ControllerInput::X:{controller->state.x = event.value; break;}
                    case ControllerInput::Y:{controller->state.y = event.value; break;}
                    case ControllerInput::LT:{controller->state.lt = event.value; break;}
                    case ControllerInput::RT:{controller->state.rt = event.value; break;}
                    case ControllerInput::LEFT_JOYSTICK_X:{
                        if (std::abs(event.value) < controller->deadzone) controller->state.lj_x = 0;
                        else controller->state.lj_x = (event.value < 0 ? -1 : 1) * (std::abs(event.value) - controller->deadzone); 
                        break;
                    }
                    case ControllerInput::LEFT_JOYSTICK_Y:{
                        if (std::abs(event.value) < controller->deadzone) controller->state.lj_y = 0;
                        else controller->state.lj_y = (event.value < 0 ? -1 : 1) * (std::abs(event.value) - controller->deadzone);
                        break;
                    }
                    case ControllerInput::RIGHT_JOYSTICK_X:{
                        if (std::abs(event.value) < controller->deadzone) controller->state.rj_x = 0;
                        else controller->state.rj_x = (event.value < 0 ? -1 : 1) * (std::abs(event.value) - controller->deadzone);
                        break;
                    }
                    case ControllerInput::RIGHT_JOYSTICK_Y:{
                        if (std::abs(event.value) < controller->deadzone) controller->state.rj_y = 0;
                        else controller->state.rj_y = (event.value < 0 ? -1 : 1) * (std::abs(event.value) - controller->deadzone);
                        break;
                    }
                    case ControllerInput::DPAD_Y:{
                        if (event.value == 1) controller->state.dd = 1;
                        else if (event.value == -1) controller->state.du = 1;
                        else{controller->state.dd = 0; controller->state.du = 0;}
                        break;
                    }
                    case ControllerInput::DPAD_X:{
                        if (event.value == 1) controller->state.dr = 1;
                        else if (event.value == -1) controller->state.dl = 1;
                        else{controller->state.dr = 0; controller->state.dl = 0;}
                        break;
                    }
                }
            }
        }
    }
    controller->guard.unlock();
}
void polling_handler(){
    start:
    if (kill_polling_thread) return;
    int timeout_ms = -1;
    input_event event;
    for (const auto& controller_map : auto_updated_controllers){
        pollfd handler_file = controller_map.second;
        int ret = poll(&handler_file, 1, timeout_ms);
        if (ret > 0){
            if (handler_file.revents){
                ssize_t bytes_read = read(handler_file.fd, &event, sizeof(struct input_event));
                if (bytes_read < 0) return;
            }
        }
        update_from_input_event(controller_map.first, event);
    }
    goto start;
}

Controller_State::Controller_State() :
    rj_x(0), rj_y(0), lj_x(0), lj_y(0), lt(0), rt(0),
    rj_d(0), lj_d(0), lb(0), rb(0),
    dl(0), dr(0), dd(0), du(0),
    s(0), m(0), h(0),
    a(0), b(0), x(0), y(0)
{}

Controller_Handle::Controller_Handle() : deadzone(0), rumble_id(-1), rumble_strength(0.0f){}

Controller::Controller() : handle(nullptr){}
Controller_State Controller::get_state() const{
    handle->guard.lock();
    Controller_State st = handle->state;
    handle->guard.unlock();
    return st;
}
void Controller::set_deadzone(float pct){
    handle->guard.lock();
    handle->deadzone = JOYSTICK_POS_MAX * pct;
    handle->guard.unlock();
}
short Controller::get_deadzone() const{
    handle->guard.lock();
    short dz = handle->deadzone;
    handle->guard.unlock();
    return dz;
}
void Controller::enable_polling(){
    if (auto_updated_controllers.find(handle) != auto_updated_controllers.end()) return;
    std::string handler_location = "/dev/input/" + handle->handler;
    pollfd handler_file;
    handler_file.fd = open(handler_location.c_str(), O_RDWR | O_NONBLOCK); // | O_RDONLY
    handler_file.events = POLLIN;
    if (handler_file.fd < 0) throw std::runtime_error("Error: Unable to open Xbox controller event location");
    auto_updated_controllers[handle] = handler_file;
    init_rumble();
    if (!setup_polling_thread){
        kill_polling_thread = false;
        polling_thread = std::thread(polling_handler);
        polling_thread.detach();
        setup_polling_thread = true;
    }
}
void Controller::disable_polling() const{
    if (auto_updated_controllers.find(handle) == auto_updated_controllers.end()) return;
    close(auto_updated_controllers[handle].fd);
    auto_updated_controllers.erase(handle);
    if (setup_polling_thread && !auto_updated_controllers.size()){
        kill_polling_thread = true;
        setup_polling_thread = false;
    }
}

float Controller::rj_x() const{
    float val = 0.0f;
    handle->guard.lock();
    if (handle->state.rj_x >= 0) val = (float)handle->state.rj_x / (JOYSTICK_POS_MAX - handle->deadzone);
    else val = (float)handle->state.rj_x / (JOYSTICK_NEG_MAX - handle->deadzone);
    handle->guard.unlock();
    return val;
}
float Controller::rj_y() const{
    float val = 0.0f;
    handle->guard.lock();
    if (handle->state.rj_y >= 0) val = (float)handle->state.rj_y / (JOYSTICK_POS_MAX - handle->deadzone);
    else val = (float)handle->state.rj_y / (JOYSTICK_NEG_MAX - handle->deadzone);
    handle->guard.unlock();
    return val;
}
float Controller::lj_x() const{
    float val = 0.0f;
    handle->guard.lock();
    if (handle->state.lj_x >= 0) val = (float)handle->state.lj_x / (JOYSTICK_POS_MAX - handle->deadzone);
    else val = (float)handle->state.lj_x / (JOYSTICK_NEG_MAX - handle->deadzone);
    handle->guard.unlock();
    return val;
}
float Controller::lj_y() const{
    float val = 0.0f;
    handle->guard.lock();
    if (handle->state.lj_y >= 0) val = (float)handle->state.lj_y / (JOYSTICK_POS_MAX - handle->deadzone);
    else val = (float)handle->state.lj_y / (JOYSTICK_NEG_MAX - handle->deadzone);
    handle->guard.unlock();
    return val;
}
float Controller::rt() const{
    handle->guard.lock();
    float val = (float)handle->state.rt / TRIGGER_MAX;
    handle->guard.unlock();
    return val;
}
float Controller::lt() const{
    handle->guard.lock();
    float val = (float)handle->state.lt / TRIGGER_MAX;
    handle->guard.unlock();
    return val;
}
bool Controller::rj_d() const{
    handle->guard.lock();
    bool val = handle->state.rj_d;
    handle->guard.unlock();
    return val;
}
bool Controller::lj_d() const{
    handle->guard.lock();
    bool val = handle->state.lj_d;
    handle->guard.unlock();
    return val;
}
bool Controller::rb() const{
    handle->guard.lock();
    bool val = handle->state.rb;
    handle->guard.unlock();
    return val;
}
bool Controller::lb() const{
    handle->guard.lock();
    bool val = handle->state.lb;
    handle->guard.unlock();
    return val;
}
DPad Controller::dpad() const{
    DPad val = DPad::NONE;
    handle->guard.lock();
    if (handle->state.dr) val = DPad::RIGHT;
    else if (handle->state.dl) val = DPad::LEFT;
    else if (handle->state.dd) val = DPad::DOWN;
    else if (handle->state.du) val = DPad::UP;
    handle->guard.unlock();
    return val;
}
bool Controller::s() const{
    handle->guard.lock();
    bool val = handle->state.s;
    handle->guard.unlock();
    return val;
}
bool Controller::m() const{
    handle->guard.lock();
    bool val = handle->state.m;
    handle->guard.unlock();
    return val;
}
bool Controller::h() const{
    handle->guard.lock();
    bool val = handle->state.h;
    handle->guard.unlock();
    return val;
}
bool Controller::a() const{
    handle->guard.lock();
    bool val = handle->state.a;
    handle->guard.unlock();
    return val;
}
bool Controller::b() const{
    handle->guard.lock();
    bool val = handle->state.b;
    handle->guard.unlock();
    return val;
}
bool Controller::x() const{
    handle->guard.lock();
    bool val = handle->state.x;
    handle->guard.unlock();
    return val;
}
bool Controller::y() const{
    handle->guard.lock();
    bool val = handle->state.y;
    handle->guard.unlock();
    return val;
}

void Controller::init_rumble(){set_rumble(0.25f);}
void Controller::set_rumble(float strength){
    auto it = auto_updated_controllers.find(handle);
    if (it == auto_updated_controllers.end()) return;
    strength = std::min(1.0f, std::abs(strength));

    // Create rumble effect
    ff_effect effect;
    effect.type = FF_RUMBLE;
    effect.id = -1;
    effect.direction = 0;
    effect.trigger = {0, 0};
    effect.replay = {100, 0};
    effect.u = {};
    effect.u.rumble.strong_magnitude = std::numeric_limits<short>::max() * strength; 
    effect.u.rumble.weak_magnitude = std::numeric_limits<short>::max() * strength;

    // Set strength of effect. This might do nothing... It seems as though strong and weak magnitude are the way to do this, 
    // But the documentation for the kernel says this is way to do it. Maybe it works for bluetooth mode?
    input_event input;
    input.type = EV_FF;
    input.code = FF_GAIN;
    input.value = (float)0xFFFFUL * strength;

    handle->guard.lock();
    if (handle->rumble_id != -1) ioctl(it->second.fd, EVIOCRMFF, handle->rumble_id);
    if (ioctl(it->second.fd, EVIOCSFF, &effect) < 0){
        std::cerr << "Warning: Failed to set rumble effect\n";
        return;
    }
    handle->rumble_id = effect.id;
    if (write(it->second.fd, &input, sizeof(input)) < 0){
        std::cerr << "Warning: Failed to set strength of rumble effect\n";
        return;
    }
    handle->guard.unlock();
}

void Controller::rumble(int32_t count) const{
    auto it = auto_updated_controllers.find(handle);
    if (it == auto_updated_controllers.end()) return;
    handle->guard.lock();
    if (handle->rumble_id == -1){
        std::cerr << "Warning: Controller does not support rumble\n";
        return;
    }
    
    input_event play_rumble;
    play_rumble.time = {};
    play_rumble.type = EV_FF;
    play_rumble.code = handle->rumble_id;
    play_rumble.value = count;
    
    write(it->second.fd, &play_rumble, sizeof(play_rumble));
    handle->guard.unlock();
}

void detect_controllers(){
    std::vector<input_device> input_devices = get_input_devices();
    for (const input_device& device : input_devices){
        if (device.id.vendor != MICROSOFT_VENDOR_ID) continue;
        for (const auto& product_id : CONTROLLER_PRODUCT_IDS){
            if (product_id.first == device.id.product){
                if (std::find_if(known_controllers.begin(), known_controllers.end(), [&device](const std::shared_ptr<Controller_Handle>& c){
                    return std::find(device.handlers.begin(), device.handlers.end(), c->handler) != device.handlers.end();
                }) != known_controllers.end()) break;
                std::shared_ptr<Controller_Handle> new_controller(new Controller_Handle);
                new_controller->handler = device.handlers[0];
                new_controller->console_type = std::get<0>(product_id.second);
                new_controller->controller_type = ControllerType::WIRED; // TODO: Add detection of wireless controller
                known_controllers.insert(new_controller);
                break;
            }
        }
    }
}
std::size_t detected_controllers_count(){return known_controllers.size();}
Controller get_controller(size_t i){
    if (i >= known_controllers.size()) throw std::invalid_argument("Error: Index is higher than number of controllers");
    Controller controller;
    controller.handle.reset(std::next(known_controllers.begin(), i)->get());
    return controller;
}
void disable_polling_all(){
    kill_polling_thread = true;
    setup_polling_thread = false;
    for (const auto& controller_map : auto_updated_controllers) close(controller_map.second.fd);
    auto_updated_controllers.erase(auto_updated_controllers.begin(), auto_updated_controllers.end());
}

}}
