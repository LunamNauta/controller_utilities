#include "controller_utilities/xbox/controller.hpp"

#include <stdexcept>
#include <cstring>
#include <limits>
#include <thread>

#include <linux/input.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <poll.h>

#include <ncurses.h>

int main(){
    Input::Xbox::detect_controllers();
    if (!Input::Xbox::detected_controllers_count()) throw std::runtime_error("Error: Failed to find Xbox controller");
    Input::Xbox::Controller controller = Input::Xbox::get_controller();
    controller.set_deadzone(4000);
    controller.enable_polling();

    float joystick_max = std::numeric_limits<short>::max() - controller.get_deadzone();
    float trigger_max = 1023;

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, true);

    float ball_position[2]{20.0f, 20.0f};
    float ball_speed = 10.0f;

    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Approximate 60 fps. Doesn't have to be accurate
        float delta_time = 16.0f/1000.0f;

        Input::Xbox::Controller_State state = controller.get_state();
        float right_joystick_x = (float)state.rj_x / joystick_max;
        float right_joystick_y = (float)state.rj_y / joystick_max;
        float left_joystick_x = (float)state.lj_x / joystick_max;
        float left_joystick_y = (float)state.lj_y / joystick_max;
        float right_trigger = (float)state.rt / trigger_max;
        float left_trigger = (float)state.lt / trigger_max;

        if (right_joystick_x < -1) right_joystick_x = -1;
        if (right_joystick_y < -1) right_joystick_y = -1;
        if (left_joystick_x < -1) left_joystick_x = -1;
        if (left_joystick_y < -1) left_joystick_y = -1;

        clear();
        mvprintw(0, 0, "Right Joystick:");
        mvprintw(0, 17 - (right_joystick_x < 0), "%.7f,", right_joystick_x);
        mvprintw(0, 29 - (right_joystick_y < 0), "%.7f", right_joystick_y);
        mvprintw(1, 0, "Left Joystick:");
        mvprintw(1, 17 - (left_joystick_x < 0), "%.7f,", left_joystick_x);
        mvprintw(1, 29 - (left_joystick_y < 0), "%.7f", left_joystick_y);
        mvprintw(2, 0, "Right Trigger:   %.7f", right_trigger);
        mvprintw(3, 0, "Left Trigger:    %.7f", left_trigger);
        mvprintw(4, 0, "Buttons: ");
        mvprintw(5, 4, "A=%i     B=%i     X=%i     Y=%i", state.a, state.b, state.x, state.y);
        mvprintw(6, 4, "Home=%i  Menu=%i  Start=%i", state.h, state.m, state.s);
        mvprintw(7, 4, "RB=%i    LB=%i", state.rb, state.lb);
        mvprintw(8, 4, "RJ=%i    LJ=%i", state.rj_d, state.lj_d);
        if (state.du) mvprintw(9, 0, "Dpad: Up");
        else if (state.dd) mvprintw(9, 0, "Dpad: Down");
        else if (state.dl) mvprintw(9, 0, "Dpad: Left");
        else if (state.dr) mvprintw(9, 0, "Dpad: Right");
        else mvprintw(9, 0, "Dpad: None");

        ball_position[0] += left_joystick_y * ball_speed * delta_time;
        ball_position[1] += left_joystick_x * ball_speed * delta_time;
        mvprintw(ball_position[1], ball_position[0], "#");
        refresh();
    }

    endwin();
}
