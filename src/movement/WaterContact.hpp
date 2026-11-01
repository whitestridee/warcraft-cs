#pragma once

// Native water presence is independent of terrain height and an overhead bridge deck.
struct WaterContact {
    bool present=false;
    float surface=0;
    float bottom=0;
};
