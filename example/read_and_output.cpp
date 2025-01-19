#include "controller_utilities/xbox/controller.hpp"

#include <stdexcept>
#include <cstring>
#include <chrono>
#include <thread>

#include <linux/input.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <poll.h>

#include <ncurses.h>

/*
 * NOTE: Joystick values retrieved with rj_x, rj_y, lj_x, and lj_y are not normalized (direction wise)
 * They are the raw values from the controller, scaled to between -1.0f and 1.0f
 * You cannot assume that the magnitude of the direction vector is equal to 1.0f
*/

int main(){
    // Detect all currently connected controllers
    Input::Xbox::detect_controllers();
    if (!Input::Xbox::detected_controllers_count()) throw std::runtime_error("Error: Failed to find Xbox controller");

    // Get the first controller that was found
    Input::Xbox::Controller controller = Input::Xbox::get_controller();
    
    controller.set_deadzone(0.25f); // Set the deadzone (as a percent to the edge). Movement less than this will return 0.0f when retrieving joystick direction
    controller.enable_polling();    // Enable automatic polling of the device state
    controller.set_rumble(0.2f);    // Set the rumble (as a percent). 1.0f will give the max rumble. Less than 0.2f may not cause any rumble at all
    
    // Keep track of if the 'a' button was pressed
    bool pressed_a = false;
    
    // The position of a 'ball' that will be drawn to the screen
    float ball_position[2]{20.0f, 20.0f};
    float ball_speed = 10.0f;
    
    // Set up for ncurses. This isn't necessary, it just makes formatting easier
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, true);

    while (true){
        // Simulate a 60 fps update loop
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        float delta_time = 16.0f/1000.0f;

        // Clear screen
        clear();
        
        // If the 'a' button was pressed, rumble the controller
        if (!pressed_a && controller.a()){
            pressed_a = true;
            controller.rumble(5);
        } else if (!controller.a()) pressed_a = false;
        
        // Output the state of the controller
        mvprintw(0, 0, "Right Joystick:");
        mvprintw(0, 17 - (controller.rj_x() < 0), "%.7f,", controller.rj_x());
        mvprintw(0, 29 - (controller.rj_y() < 0), "%.7f", controller.rj_y());
        mvprintw(1, 0, "Left Joystick:");
        mvprintw(1, 17 - (controller.lj_x() < 0), "%.7f,", controller.lj_x());
        mvprintw(1, 29 - (controller.lj_y() < 0), "%.7f", controller.lj_y());
        mvprintw(2, 0, "Right Trigger:   %.7f", controller.rt());
        mvprintw(3, 0, "Left Trigger:    %.7f", controller.lt());
        mvprintw(4, 0, "Buttons: ");
        mvprintw(5, 4, "A=%i     B=%i     X=%i     Y=%i", controller.a(), controller.b(), controller.x(), controller.y());
        mvprintw(6, 4, "Home=%i  Menu=%i  Start=%i", controller.h(), controller.m(), controller.s());
        mvprintw(7, 4, "RB=%i    LB=%i", controller.rb(), controller.lb());
        mvprintw(8, 4, "RJ=%i    LJ=%i", controller.rj_d(), controller.lj_d());
        switch (controller.dpad()){
            case Input::Xbox::DPad::RIGHT:{mvprintw(9, 0, "DPad: Right"); break;}
            case Input::Xbox::DPad::LEFT:{mvprintw(9, 0, "DPad: Left"); break;}
            case Input::Xbox::DPad::DOWN:{mvprintw(9, 0, "DPad: Down"); break;}
            case Input::Xbox::DPad::UP:{mvprintw(9, 0, "DPad: Up"); break;}
            default:{mvprintw(9, 0, "DPad: None"); break;}
        }

        // Update the ball's position, then draw it
        ball_position[0] += controller.lj_x() * ball_speed * delta_time;
        ball_position[1] += controller.lj_y() * ball_speed * delta_time;
        mvprintw(ball_position[1], ball_position[0], "#");
        
        // Update screen
        refresh();
    }
    
    // Uninitalize ncurses
    endwin();
}
