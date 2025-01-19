#include "xbox/controller.hpp"

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
 * NOTE: Joystick values retrieved with rj_x, rj_y, lj_x, and lj_y are not normalized
 * They are the raw values from the controller, scaled to between -1.0f and 1.0f
 * You cannot assume that the magnitude of the direction vector is equal to 1.0f,
 * Only that each component's magnitude is at most 1.0f
*/

int main(){
    Input::Xbox::detect_controllers();
    if (!Input::Xbox::detected_controllers_count()) throw std::runtime_error("Error: Failed to find Xbox controller");
    Input::Xbox::Controller controller = Input::Xbox::get_controller();
    controller.set_deadzone(4000);
    controller.enable_polling();
 
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, true);

    float ball_position[2]{20.0f, 20.0f};
    float ball_speed = 10.0f;

    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        float delta_time = 16.0f/1000.0f;

        clear();
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
        ball_position[0] += controller.lj_x() * ball_speed * delta_time;
        ball_position[1] += controller.lj_y() * ball_speed * delta_time;
        mvprintw(ball_position[1], ball_position[0], "#");
        refresh();
    }

    endwin();
}
