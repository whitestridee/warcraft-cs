// Authored offline regression: real damage to the Town Hall immediately raises its stock militia alarm.
// Import as the script of a disposable classic test map; never run it in a campaign save.
globals
    unit MilitiaHall=null
endglobals

function AlarmOnDamage takes nothing returns nothing
    call IssueImmediateOrder(MilitiaHall,"townbellon")
endfunction

function main takes nothing returns nothing
    local unit actor
    local trigger alarm
    local integer i=0
    local unit peasant
    call SetCameraBounds(-7808,-7808,7808,7808,-7808,7808,7808,-7808)
    call SetDayNightModels("Environment\\DNC\\DNCLordaeron\\DNCLordaeronTerrain\\DNCLordaeronTerrain.mdl","Environment\\DNC\\DNCLordaeron\\DNCLordaeronUnit\\DNCLordaeronUnit.mdl")
    call InitBlizzard()
    call SetFloatGameState(GAME_STATE_TIME_OF_DAY,12)
    call FogEnable(false)
    call FogMaskEnable(false)
    set actor=CreateUnit(Player(0),'hfoo',-2000,-1000,0)
    call SetUnitInvulnerable(actor,true)
    call SelectUnit(actor,true)
    set MilitiaHall=CreateUnit(Player(1),'htow',-1350,-1000,180)
    call UnitAddAbility(MilitiaHall,'Amic')
    set alarm=CreateTrigger()
    call TriggerRegisterUnitEvent(alarm,MilitiaHall,EVENT_UNIT_DAMAGED)
    call TriggerAddAction(alarm,function AlarmOnDamage)
    loop
        exitwhen i>=6
        set peasant=CreateUnit(Player(1),'hpea',-1460+I2R(i/2)*20,-1100+I2R(ModuloInteger(i,2))*200,180)
        set i=i+1
    endloop
endfunction

function config takes nothing returns nothing
    call SetMapName("Militia alarm regression")
    call SetMapDescription("Disposable offline native morph test")
    call SetPlayers(1)
    call SetTeams(1)
    call DefineStartLocation(0,-2000,-1000)
    call SetPlayerStartLocation(Player(0),0)
    call SetPlayerTeam(Player(0),0)
    call SetPlayerRacePreference(Player(0),RACE_PREF_HUMAN)
    call SetPlayerRaceSelectable(Player(0),false)
    call SetPlayerController(Player(0),MAP_CONTROL_USER)
endfunction
