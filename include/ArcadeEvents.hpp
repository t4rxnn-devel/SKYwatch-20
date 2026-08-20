#pragma once
#include "RadarSystem.hpp"

namespace Arcade {
    // Spawns rogue UFO targets with impossible flight profiles occasionally
    void trigger_ufo_anomaly(RadarSystem& tower);

    // Forces a physical audio beep alert through your computer speaker
    void play_cockpit_alarm();

    // Checks the airspace for supersonic flights to determine if a sonic boom occurs
    bool check_sonic_boom(const RadarSystem& tower);
}
