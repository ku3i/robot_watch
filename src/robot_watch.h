#ifndef MAIN_H
#define MAIN_H

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <unistd.h>

#include <ctime>
#include <cmath>
#include <cfloat>
#include <cstring>
#include <cerrno>
#include <clocale>
#include <sys/time.h>
#include <signal.h>
#include <vector>
#include <functional>

#include <common/application_interface.h>
#include <common/event_manager.h>
#include <common/log_messages.h>
#include <common/basic.h>
#include <midi/midi_in.h>

#include <robots/robot.h>
#include <robots/simloid.h>
#include <robots/simloid_graphics.h>
#include <robots/spinalcord_watch.h>

#include <control/jointcontrol.h>
#include <control/jointcontrol_graphics.h>
#include "./setting.h"

class Puppet_Master {
    robots::Simloid&  robot;
          std::size_t body_id;
    const std::size_t num_bodies;
    Vector3           force;

public:
    Puppet_Master(robots::Simloid& robot, std::size_t num_bodies) : robot(robot), body_id(0), num_bodies(num_bodies), force(.0) {}

    void key_pressed(SDL_Keysym &keysym) {
        switch (keysym.sym) {
        case SDLK_LEFT : force.x = -10.0; break;
        case SDLK_RIGHT: force.x = +10.0; break;
        case SDLK_UP   : force.y = +10.0; break;
        case SDLK_DOWN : force.y = -10.0; break;
        case SDLK_HOME : force.z = +60.0; break;
        case SDLK_END  : force.z = -60.0; break;
        case SDLK_n    : ++body_id; if (body_id  >= num_bodies) body_id = 0;       break;
        case SDLK_m    : if (body_id  > 0) --body_id; else body_id = num_bodies-1; break;
        default        : return;
        }
        robot.set_force(body_id, force);
    }

    void key_released(SDL_Keysym& keysym) {
        switch (keysym.sym) {
        case SDLK_LEFT : force.x = 0.0; break;
        case SDLK_RIGHT: force.x = 0.0; robot.set_force(0, force); break;
        case SDLK_UP   : force.y = 0.0; robot.set_force(0, force); break;
        case SDLK_DOWN : force.y = 0.0; robot.set_force(0, force); break;
        case SDLK_HOME : force.z = 0.0; robot.set_force(0, force); break;
        case SDLK_END  : force.z = 0.0; robot.set_force(0, force); break;
        default        : return;
        }
        robot.set_force(body_id, force);
    }
};

class Application : public Application_Interface, public Application_Base
{
public:
    Application(int argc, char **argv, Event_Manager &em)
    : Application_Base("Robot Watch", 1600, 800)
    , settings(argc, argv)
    , event(em)
    , robot(settings.tcp_port, settings.robot_id, settings.scene_id, true)
    , control(robot)
    , parameter_set(1)
    , joystick()
    , midi(1)
    , puppet_master(robot, robot.get_number_of_bodies())
    , robot_graphics(robot)
    , spinalcord_watch(robot, 200)
    , control_graphics(robot, control)
    , cycles(0)
    , restore_state(true)
    {
        /* register key event */
        event.register_user_callback_key_pressed (std::bind(&Application::user_callback_key_pressed , this, std::placeholders::_1));
        event.register_user_callback_key_released(std::bind(&Application::user_callback_key_released, this, std::placeholders::_1));

        parameter_set.add(settings.seedfile);
        control.set_control_parameter(parameter_set.get(0));
    }

    bool loop();
    void finish();
    void draw(const pref&) const ;
    uint64_t get_cycle_count(void) const { return cycles; }
    bool visuals_enabled(void) { return true; }
    void user_callback_key_pressed (SDL_Keysym &keysym);
    void user_callback_key_released(SDL_Keysym &keysym);

private:
    Settings                 settings;
    Event_Manager&           event;
    robots::Simloid          robot;
    control::Jointcontrol    control;
    control::Control_Vector  parameter_set;
    Joystick                 joystick;
    MidiIn                   midi;
    Puppet_Master            puppet_master;

    /* drawings */
    robots::Simloid_Graphics       robot_graphics;
    robots::Spinalcord_Watch       spinalcord_watch;
    control::Jointcontrol_Graphics control_graphics;


    uint64_t                 cycles;
    bool                     restore_state;
};

#endif /*MAIN_H*/

