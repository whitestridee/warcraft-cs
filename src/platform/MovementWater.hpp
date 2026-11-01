#pragma once
#include "WarcraftApi.hpp"
#include "../movement/MovementPhysics.hpp"
#include <algorithm>
#include <cmath>

// Both collision strategies share native liquid classification without replacing their solid sweeps.
namespace MovementWater {
inline constexpr int Floatability=6; // Owned common.j PATHING_TYPE_FLOATABILITY, not the peon-harvest mask (4).
inline bool ExitAvailable(wc3::Handle unit,const MovementPhysics& movement,const MoveInput& input) {
    if(!movement.Swimming() || !input.jump || input.immobilized || movement.WaterExitHeight()<=0)return false;
    float length=std::hypot(input.forward,input.right);if(length<.01f)return false;
    float angle=input.yaw*.01745329252f;
    float dx=(input.forward*std::cos(angle)+input.right*std::sin(angle))/length;
    float dy=(input.forward*std::sin(angle)-input.right*std::cos(angle))/length;
    float x=wc3::Real(wc3::GetUnitX(unit)),y=wc3::Real(wc3::GetUnitY(unit));
    // A bank must exist in the intended direction, within bounded reach and exit height.
    // This only authorizes takeoff: terrain, solids and ceilings still resolve every swept step.
    for(float distance=32;distance<=movement.WaterExitReach();distance+=16) {
        float px=x+dx*distance,py=y+dy*distance,deck=0;
        float floor=wc3::TerrainGround(px,py);
        auto reachable=[&](float height) {
            return std::isfinite(height) && height>movement.FeetZ()+movement.StepHeight() &&
                height<=movement.WaterLevel()+movement.WaterExitHeight();
        };
        if(wc3::WalkableSurface(px,py,deck) && reachable(deck))return true;
        // Native cliff rims can reject walking even above their lip. Existing sweeps still
        // require sufficient physical height; an overhead deck must not hide a lower bank.
        if(reachable(floor) && (!wc3::IsTerrainPathable(&px,&py,1) ||
            wc3::GetTerrainCliffLevel(&px,&py)!=wc3::GetTerrainCliffLevel(&x,&y)))return true;
    }
    return false;
}
inline void Update(wc3::Handle unit,MovementPhysics& movement) {
    // Native flight is added separately to this motor's feet/camera. Keep its existing support,
    // rather than treating a flying actor's logical terrain feet as an immersed body.
    float flight=wc3::GetUnitFlyHeight ? wc3::Real(wc3::GetUnitFlyHeight(unit)) : 0;
    movement.AllowSwimming(std::isfinite(flight) && std::abs(flight)<=.01f);
    WaterContact contact;
    float x=wc3::Real(wc3::GetUnitX(unit)),y=wc3::Real(wc3::GetUnitY(unit)),deck=0;
    if(movement.SwimmingEnabled() && wc3::WaterSurface(x,y,contact.surface)) {
        contact.bottom=wc3::TerrainGround(x,y);contact.present=std::isfinite(contact.bottom);
        // A rider above the liquid stays on its deck. Water under an overhead bridge remains swimmable.
        if(wc3::WalkableSurface(x,y,deck) && movement.FeetZ()>=deck-movement.StepHeight() && deck>=contact.surface)
            contact.present=false;
    }
    movement.SetWater(contact);
}
inline float Ground(float x,float y,const MovementPhysics& movement) {
    float water=0;
    // Water is not a solid floor: shallow wading uses its bed; disabling swimming restores legacy support.
    return movement.SwimmingEnabled() && wc3::WaterSurface(x,y,water) ? wc3::TerrainGround(x,y) : wc3::Ground(x,y);
}
inline bool Passable(float x,float y,const MovementPhysics& movement) {
    float water=0;
    // Only real liquid with native floatability may override a walking-only pathing restriction.
    return movement.SwimmingEnabled() && wc3::WaterSurface(x,y,water) && water>wc3::TerrainGround(x,y)+.01f &&
        !wc3::IsTerrainPathable(&x,&y,Floatability);
}
}
