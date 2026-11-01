#pragma once

// Water tuning uses Warcraft heights and a percentage of the current, status-adjusted movement speed.
struct SwimmingSettings {
    bool enabled=true;
    float minimumDepth=96;
    float speedPercent=65;
    float acceleration=6;
    float drag=4;
    float floatSeconds=.2f;
    float eyeAboveWater=12;
    // Space near a reachable bank launches one swept exit, never repeated jumps in open water.
    float exitHeight=128;
    float exitReach=128;
};
