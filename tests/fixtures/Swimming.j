// Authored offline water regression; use only with a disposable basin/shoreline test terrain.
function main takes nothing returns nothing
    local unit actor
    local unit flying
    call SetCameraBounds(-7808,-7808,7808,7808,-7808,7808,7808,-7808)
    call SetDayNightModels("Environment\\DNC\\DNCLordaeron\\DNCLordaeronTerrain\\DNCLordaeronTerrain.mdl","Environment\\DNC\\DNCLordaeron\\DNCLordaeronUnit\\DNCLordaeronUnit.mdl")
    call InitBlizzard()
    call SetFloatGameState(GAME_STATE_TIME_OF_DAY,12)
    call FogEnable(false)
    call FogMaskEnable(false)
    set actor=CreateUnit(Player(0),'hfoo',-1600,0,0)
    call SetUnitInvulnerable(actor,true)
    call SelectUnit(actor,true)
    set flying=CreateUnit(Player(0),'hgry',700,400,180)
endfunction

function config takes nothing returns nothing
    call SetMapName("Swimming regression")
    call SetMapDescription("Disposable offline deep/shallow water and bank test")
    call SetPlayers(1)
    call SetTeams(1)
    call DefineStartLocation(0,-1600,0)
    call SetPlayerStartLocation(Player(0),0)
    call SetPlayerTeam(Player(0),0)
    call SetPlayerRacePreference(Player(0),RACE_PREF_HUMAN)
    call SetPlayerRaceSelectable(Player(0),false)
    call SetPlayerController(Player(0),MAP_CONTROL_USER)
endfunction
