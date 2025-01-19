#ifndef XBOX_CONTROLLER_HEADER
#define XBOX_CONTROLLER_HEADER

#include "xbox/constants.hpp"

#include <string>
#include <memory>
#include <mutex>

namespace Input{
namespace Xbox{

struct Controller_State{
    short lj_x;    // Left joystick (X-Axis)
    short lj_y;    // Left joystick (Y-Axis)
    short rj_x;    // Right joystick (X-Axis)
    short rj_y;    // Right joystick (Y-Axis)
    short lt;      // Left trigger
    short rt;      // Right trigger
    bool lj_d : 1; // Left joystick (Down)
    bool rj_d : 1; // Right joystick (Down)
    bool lb : 1;   // Left bumper
    bool rb : 1;   // Right bumper
    bool dl : 1;   // Dpad left
    bool dr : 1;   // Dpad right
    bool dd : 1;   // Dpad down
    bool du : 1;   // Dpad up
    bool s : 1;    // Start
    bool m : 1;    // Menu
    bool h : 1;    // Home
    bool a : 1;
    bool b : 1;
    bool x : 1;
    bool y : 1;

    Controller_State();
};

struct Controller_Handle{
    std::mutex guard;
    std::string handler;
    Controller_State state;
    ConsoleType console_type;
    ControllerType controller_type;
    short deadzone;

    Controller_Handle();
};

class Controller{
private:
    std::shared_ptr<Controller_Handle> handle;

public:
    Controller();

    Controller_State get_state() const;
    void set_deadzone(short di);
    short get_deadzone() const;

    void enable_polling() const;
    void disable_polling() const;

    float rj_x() const;
    float rj_y() const;
    float lj_x() const;
    float lj_y() const;
    float rt() const;
    float lt() const;
    bool rj_d() const;
    bool lj_d() const;
    bool rb() const;
    bool lb() const;
    DPad dpad() const;
    bool s() const;
    bool m() const;
    bool h() const;
    bool a() const;
    bool b() const;
    bool x() const;
    bool y() const;

    friend void detect_controllers();
    friend Controller get_controller(std::size_t i);
};

void detect_controllers();
std::size_t detected_controllers_count();
Controller get_controller(std::size_t i = 0);

}}

#endif
