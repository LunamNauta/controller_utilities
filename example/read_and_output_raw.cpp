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

/*
 * NOTE: The joystick values shown here are not normalized
 * They are the raw values from the controller, scaled to between -1.0f and 1.0f
 * You cannot assume that the magnitude of the direction vector is equal to 1.0f,
 * Only that each component's magnitude is at most 1.0f
*/

int main(){
    // Detect all currently connected controllers
    Input::Xbox::detect_controllers();
    if (!Input::Xbox::detected_controllers_count()) throw std::runtime_error("Error: Failed to find Xbox controller");
    
    Input::Xbox::Controller controller = Input::Xbox::get_controller(); // Get the first controller that was found
    controller.set_deadzone(0.25f); // Set the deadzone (as a percent). Joystick values less than this will be read as 0.
    controller.enable_polling();   // Enable polling of this device. Without this, the controller state will not update

    // On all the Xbox controllers tested, joystick max has been SHRT_MAX and the SHRT_MIN (joystick position is defined by 'short's)
    // Likewise, trigger max values have always been (2^10)-1
    float joystick_max = std::numeric_limits<short>::max() - controller.get_deadzone();
    float trigger_max = 1023;

    // Ncurses setup. None of this is required to use this library. It just looks nice
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, true);

    // Data for a little ball you can move around the console
    float ball_position[2]{20.0f, 20.0f};
    float ball_speed = 10.0f;

    while (true){
        // Simulate approximately 60 fps. Doesn't have to be accurate
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        float delta_time = 16.0f/1000.0f;

        // Get the state of the controller
        Input::Xbox::Controller_State state = controller.get_state();

        // Scale the joystick values to be between -1.0f and 1.0f
        float right_joystick_x = (float)state.rj_x / joystick_max;
        float right_joystick_y = (float)state.rj_y / joystick_max;
        float left_joystick_x = (float)state.lj_x / joystick_max;
        float left_joystick_y = (float)state.lj_y / joystick_max;
        float right_trigger = (float)state.rt / trigger_max;
        float left_trigger = (float)state.lt / trigger_max;

        // Because SHRT_MAX != -SHRT_MIN, the values need to be capped again. I'm too lazy to make this work better.
        if (right_joystick_x < -1) right_joystick_x = -1;
        if (right_joystick_y < -1) right_joystick_y = -1;
        if (left_joystick_x < -1) left_joystick_x = -1;
        if (left_joystick_y < -1) left_joystick_y = -1;
    
        // Output the controller state to the console
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
    
        // Output the little ball
        ball_position[0] += left_joystick_y * ball_speed * delta_time;
        ball_position[1] += left_joystick_x * ball_speed * delta_time;
        mvprintw(ball_position[1], ball_position[0], "#");
        refresh();
    }

    endwin();
}
