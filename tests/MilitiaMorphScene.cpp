#include "MilitiaMorphScene.hpp"
#include "BackgroundGameplay.hpp"
#include "../src/platform/GameBuild.hpp"
#include "../src/platform/NativeFaultDiagnostics.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace {
wc3::Handle hall=0;
std::vector<wc3::Handle> workers;
DWORD sampled=0;
LONG CALLBACK Observe(EXCEPTION_POINTERS* exception) {
    // Preserve Warcraft's own exception handling; the opt-in fixture records the original first-chance site.
    if(exception && exception->ExceptionRecord && exception->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)
        NativeFaultDiagnostics::Filter("militia-fixture",exception);
#ifdef _M_IX86
    if(exception && exception->ExceptionRecord && exception->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION) {
        // Stack slots are raw evidence, not a claimed unwind; optimized native frames omit EBP chains.
        auto stack=exception->ContextRecord->Esp;uintptr_t values[96]={};SIZE_T read=0;
        if(ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(stack),values,sizeof(values),&read))
            for(unsigned i=0;i<read/sizeof(uintptr_t);++i)wc3::Log("fixture militia stack slot=%u value=%08X",i,values[i]);
    }
#endif
    return EXCEPTION_CONTINUE_SEARCH;
}
}
void MilitiaMorphScene::Tick(ShooterController& controller,const char* root) {
    if(!BackgroundGameplay::Enabled() || wc3::ActiveBuild()!=wc3::GameBuildKind::Classic126a)return;
    std::string path=std::string(root)+"\\test-militia.request";
    if(GetFileAttributesA(path.c_str())!=INVALID_FILE_ATTRIBUTES) {
        std::ifstream request(path);std::string command;request>>command;request.close();DeleteFileA(path.c_str());
        if(command=="setup") {
            static auto observer=AddVectoredExceptionHandler(1,Observe);(void)observer;
            if(hall && wc3::GetUnitTypeId(hall))wc3::RemoveUnit(hall);
            for(auto worker:workers)if(wc3::GetUnitTypeId(worker))wc3::RemoveUnit(worker);
            workers.clear();
            auto actor=controller.TestUnit();float x=wc3::Real(wc3::GetUnitX(actor))+650,y=wc3::Real(wc3::GetUnitY(actor)),angle=180;
            hall=wc3::CreateUnit(wc3::Player(1),0x68746F77,&x,&y,&angle);
            // A newly created nonstarting hall needs the stock bell ability to match a base's alarm.
            using Add=BOOL (__cdecl*)(wc3::Handle,int);
            auto base=reinterpret_cast<uintptr_t>(GetModuleHandleA("Game.dll"));
            reinterpret_cast<Add>(base+0x3C82A0)(hall,0x416D6963);
            for(int i=0;i<6;++i) {
                float px=x-280+(i%2)*60,py=y+(i/2-1)*75;
                workers.push_back(wc3::CreateUnit(wc3::Player(1),0x68706561,&px,&py,&angle));
            }
            wc3::Log("fixture militia setup hall=%08X workers=%u bell=%d on=%d off=%d",hall,unsigned(workers.size()),
                wc3::GetUnitAbilityLevel(hall,0x416D6963),wc3::OrderId("townbellon"),wc3::OrderId("townbelloff"));
        } else if(command=="attach") {
            // Use map-created units and its ordinary damage trigger, rather than issuing a synthetic mid-render bell.
            static auto observer=AddVectoredExceptionHandler(1,Observe);(void)observer;
            hall=0;workers.clear();auto group=wc3::CreateGroup();wc3::GroupEnumUnitsOfPlayer(group,wc3::Player(1),0);
            for(auto unit=wc3::FirstOfGroup(group);unit;unit=wc3::FirstOfGroup(group)) {
                wc3::GroupRemoveUnit(group,unit);int type=wc3::GetUnitTypeId(unit);
                if(type==0x68746F77)hall=unit;
                if(type==0x68706561 || type==0x686D696C)workers.push_back(unit);
            }
            wc3::DestroyGroup(group);wc3::Log("fixture militia attached hall=%08X workers=%u",hall,unsigned(workers.size()));
        } else if(command=="on" || command=="off") {
            int order=wc3::OrderId(command=="on" ? "townbellon" : "townbelloff");
            wc3::Log("fixture militia bell begin order=%d",order);
            auto accepted=wc3::IssueImmediateOrderById(hall,order);
            wc3::Log("fixture militia bell returned accepted=%d",accepted);
        } else if(command=="workers" || command=="return") {
            int order=wc3::OrderId(command=="workers" ? "militia" : "militiaoff");
            for(auto worker:workers) {
                wc3::Log("fixture militia worker order begin unit=%08X type=%08X order=%d",worker,wc3::GetUnitTypeId(worker),order);
                wc3::IssueImmediateOrderById(worker,order);
            }
        }
    }
    if(!workers.empty() && GetTickCount()-sampled>=500) {
        sampled=GetTickCount();unsigned peasant=0,militia=0;
        for(auto worker:workers){int type=wc3::GetUnitTypeId(worker);peasant+=type==0x68706561;militia+=type==0x686D696C;}
        wc3::Log("fixture militia state peasants=%u militia=%u fps=%d",peasant,militia,controller.Visible());
    }
}
