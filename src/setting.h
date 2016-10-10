/* setting.h */

#ifndef SETTING_H
#define SETTING_H

#include <string.h>
#include <common/settings.h>

class Settings : public Settings_Base
{

public:
    /* public settings */
    std::size_t robot_id;
    std::size_t scene_id;
    std::size_t tcp_port;

    std::string seedfile;

    Settings(int argc, char **argv)
    : Settings_Base(argc, argv, "./robot_watch.dat")
    , robot_id(read_uint("robot_id", 31  ))
    , scene_id(read_uint("scene_id", 0   ))
    , tcp_port(read_uint("tcp_port", 9876))
    , seedfile(read_str ("seedfile", "../data/seeds/" + std::to_string(robot_id) + "/" + std::to_string(robot_id) + "s_initial.dat"))
    {}
};

#endif // SETTING_H
