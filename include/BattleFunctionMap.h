#include <unordered_map>
#include <string>
#include <functional>
#include <PKMN.h>

// Define a function type for battle effects
using BattleFunction = std::function<void(PKMN& sourcePoke, PKMN& targetPoke, std::string& text, int move)>;


// Declare the function map
extern std::unordered_map<std::string, BattleFunction> FunctionMap;

void ExecuteMove(const std::string& functionCode, PKMN& sourcePoke, PKMN& targetPoke, std::string& text, int move);

enum AffectedStat {
    ATTACK,
    DEFENSE,
    SPEED,
    SP_ATK,
    SP_DEF,
    ACCURACY,
    EVASION,
    CRIT,
    NONE // For natures with no effect
};

enum MoveActionState {
        SHOW_MOVE_TEXT,
        EXECUTE_MOVE,
        BLINK_SPRITE,
        ANIMATE_ACTION,
        DAMAGE_UPDATE,
        APPLY_EFFECTS,
        FAINT_CHECK,
        RECOVER_HEALTH,
        END_PHASE
    };
