#include "SwimmingScene.hpp"
#include "BackgroundGameplay.hpp"
#include "../src/platform/GameBuild.hpp"
#include <fstream>
#include <string>

void SwimmingScene::Tick(ShooterController& controller,const char* root) {
    if(!BackgroundGameplay::Enabled() || wc3::ActiveBuild()!=wc3::GameBuildKind::Classic126a)return;
    static DWORD until=0,last=0;
    auto path=std::string(root)+"\\test-swimming.request";
    if(GetFileAttributesA(path.c_str())!=INVALID_FILE_ATTRIBUTES) {
        std::ifstream file(path);std::string command;file>>command;
        if(command=="watch"){DWORD duration=0;file>>duration;until=GetTickCount()+std::min(duration,DWORD(30000));}
        else if(command=="exit") {DWORD duration=0;file>>duration;controller.TestWaterExit(duration);}
        else if(command=="sample") {
            float x=0,y=0,water=0,deck=0;file>>x>>y;
            bool present=wc3::WaterSurface(x,y,water),raised=wc3::WalkableSurface(x,y,deck);
            wc3::Log("fixture water sample x=%.1f y=%.1f liquid=%d water=%.3f bed=%.3f native=%.3f bridge=%d deck=%.3f walkBlocked=%d floatBlocked=%d",
                x,y,present,water,wc3::TerrainGround(x,y),wc3::Ground(x,y),raised,deck,wc3::IsTerrainPathable(&x,&y,1),wc3::IsTerrainPathable(&x,&y,6));
        } else if(command=="selectfly") {
            // Select an already initialized map unit; spawning a model during RenderWorld is never part of this oracle.
            auto group=wc3::CreateGroup();wc3::GroupEnumUnitsOfPlayer(group,wc3::GetLocalPlayer(),0);
            for(auto unit=wc3::FirstOfGroup(group);unit;unit=wc3::FirstOfGroup(group)) {
                wc3::GroupRemoveUnit(group,unit);
                if(wc3::GetUnitTypeId(unit)==0x68677279){wc3::ClearSelection();wc3::SelectUnit(unit,TRUE);wc3::Log("fixture water flying actor=%08X flight=%.1f",unit,wc3::Real(wc3::GetUnitFlyHeight(unit)));break;}
            }
            wc3::DestroyGroup(group);
        } else if(command=="inspect")controller.TestWaterState();
        file.close();DeleteFileA(path.c_str());
    }
    // Sampling after completed drawing never creates or transforms native objects inside RenderWorld.
    if(controller.Visible() && GetTickCount()<until && GetTickCount()-last>=100){last=GetTickCount();controller.TestWaterState();}
}
