/*
 * Robot Watch
 * Matthias Kubisch
 * kubisch@informatik.hu-berlin.de
 * created 13.Okt 2014
 */

#include "./robot_watch.h"

/* framework */
#include <common/modules.h>
#include <common/setup.h>

/* drawing */
#include <draw/draw.h>
#include <draw/axes.h>
#include <draw/plot1D.h>

DEFINE_GLOBALS()

void
Application::draw(const pref& p) const
{
    robot_graphics.draw(p);
    spinalcord_watch.draw(p);
    control_graphics.draw(p);
}

void
Application::user_callback_key_pressed(SDL_Keysym &keysym)
{
    switch (keysym.sym)
    {
        case SDLK_F2 : if (not restore_state) restore_state = true; break;
        case SDLK_F5 : parameter_set.reload(0, settings.seedfile);
                       control.set_control_parameter(parameter_set.get(0));
                       break;
        default:
            break;
    }
    puppet_master.key_pressed(keysym);
}

void
Application::user_callback_key_released(SDL_Keysym &keysym)
{
    puppet_master.key_released(keysym);
}

bool
Application::loop(void)
{
    bool simloid_status = false;

    if (restore_state) {
        robot.restore_state();
        control.reset();
        simloid_status |= robot.update();
        sts_msg("Restoring state.");
        restore_state = false;
    }

    control.loop();
    simloid_status |= robot.update();
    midi.fetch();


    robot_graphics.execute_cycle();
    spinalcord_watch.execute_cycle(cycles);
    ++cycles;
    return simloid_status;
}

void
Application::finish(void)
{
    robot.finish();
    sts_msg("Finished shutting down all subsystems.");
    quit();
}

APPLICATION_MAIN()
