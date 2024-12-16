#include "BattleFunctionMap.h"
#include "ActionHandler.h"
#include <iostream> // For debugging/logging

// Implement the functions
void HitTwoToFiveTimes(PKMN& sourcePoke, PKMN& targetPoke, std::string& text, int move) {
    std::cout << sourcePoke.GetPKMN_Name() << " is hitting " << targetPoke.GetPKMN_Name() << " 2 to 5 times!" << std::endl;
    // Add actual logic here
}

void BurnTarget(PKMN& sourcePoke, PKMN& targetPoke, std::string& text, int move) {
    std::cout << targetPoke.GetPKMN_Name() << " is burned!" << std::endl;
}

void StatusMultiplier(PKMN& sourcePoke, PKMN& targetPoke, std::string& text, int move) {
    static const std::unordered_map<std::string, int> StatusMap = {
        {"Attack", ATTACK},
        {"Defense", DEFENSE},
        {"Accuracy", ACCURACY},
        {"Speed", SPEED},
        {"Evasion", EVASION},
        {"SpAtk", SP_ATK},
        {"SpDef", SP_DEF},
        {"CriticalHitRate", CRIT}
    };

    static const std::unordered_map<std::string, std::string> StatusNames = {
        {"Attack", "ATTACK"},
        {"Defense", "DEFENSE"},
        {"Accuracy", "ACCURACY"},
        {"Speed", "SPEED"},
        {"Evasion", "EVASION"},
        {"SpAtk", "SP_ATK"},
        {"SpDef", "SP_DEF"},
        {"CriticalHitRate", "CRIT"}
    };

    std::string functionCode = sourcePoke.GetMoveFunctionCode(move);
    std::string TARGET = sourcePoke.GetMovementInfo(move,8);
    int polarity = functionCode.find("Raise") != std::string::npos ? 1 : -1;

    // Find the status to modify
    int AFFECTED_STATUS = -1;
    std::string AFFECTED_STATUS_NAME;
    for (auto it = StatusMap.begin(); it != StatusMap.end(); ++it) {
        if (functionCode.find(it->first) != std::string::npos) {
            AFFECTED_STATUS = it->second;
            AFFECTED_STATUS_NAME = StatusNames.at(it->first);
            break;
        }
    }

    // Check for multipliers
    int VALUE_MULTIPLIER = 0;
    if (functionCode.find("1") != std::string::npos) {
        VALUE_MULTIPLIER = 1 * polarity;
    } else if (functionCode.find("2") != std::string::npos) {
        VALUE_MULTIPLIER = 2 * polarity;
    }

    // Apply the multiplier if a status was found
    if (AFFECTED_STATUS != -1) {
        if (TARGET == "UserAndAllies" or TARGET == "User"){
            sourcePoke.SetBattleStatusMultiplier(AFFECTED_STATUS, VALUE_MULTIPLIER, AFFECTED_STATUS_NAME);
            text = sourcePoke.GetStatusText();
        } else{
            targetPoke.SetBattleStatusMultiplier(AFFECTED_STATUS, VALUE_MULTIPLIER, AFFECTED_STATUS_NAME);
            text = targetPoke.GetStatusText();
        }

    } else {
        text = "Invalid status code in functionCode!";
    }
}

void HealUserByThreeQuartersOfDamageDone(PKMN& sourcePoke, PKMN& targetPoke, std::string& text, int move){
    int DAMAGE_DEALT = sourcePoke.GetTurnDamage();
    float RECOVER = sourcePoke.GetCurHp() + ((DAMAGE_DEALT / 4.0f) * 3);
    if (RECOVER > sourcePoke.GetMaxHP()){
        RECOVER = sourcePoke.GetMaxHP();
    }
    std::cout << "Amount: " <<RECOVER << std::endl;

    sourcePoke.SetHealthAmount(int(RECOVER));
    sourcePoke.SetNextMovePhase(RECOVER_HEALTH);
    text = sourcePoke.GetPKMN_NickName() + " regained health!";
}

// Repeat for other functions as needed...

void ExecuteMove(const std::string& functionCode, PKMN& sourcePoke, PKMN& targetPoke, std::string& text, int move) {
    auto it = FunctionMap.find(functionCode);
    if (it != FunctionMap.end()) {
        it->second(sourcePoke, targetPoke, text, move); // Pass required parameters
    } else {
        text = "Nothing happened!";
        std::cerr << "Error: Unknown function code '" << functionCode << "'" << std::endl;
    }
}

// Initialize the function map
std::unordered_map<std::string, BattleFunction> FunctionMap = {
    {"HitTwoToFiveTimes", HitTwoToFiveTimes},
    {"BurnTarget", BurnTarget},
    {"LowerTargetDefense1", StatusMultiplier},
    {"LowerTargetDefense2", StatusMultiplier},
    {"LowerTargetAccuracy1", StatusMultiplier},
    {"LowerTargetAccuracy2", StatusMultiplier},
    {"LowerTargetSpeed1", StatusMultiplier},
    {"LowerTargetSpeed2", StatusMultiplier},
    {"RaiseTargetAttack1", StatusMultiplier},
    {"LowerTargetAttack1", StatusMultiplier},
    {"LowerTargetAttack2", StatusMultiplier},
    {"RaiseUserCriticalHitRate2", StatusMultiplier},
    {"LowerTargetEvasion1", StatusMultiplier},
    {"LowerTargetEvasion2", StatusMultiplier},
    {"LowerTargetSpAtk1", StatusMultiplier},
    {"LowerTargetSpAtk2", StatusMultiplier},
    {"LowerTargetSpDef1", StatusMultiplier},
    {"LowerTargetSpDef2", StatusMultiplier},
    {"HealUserByThreeQuartersOfDamageDone", HealUserByThreeQuartersOfDamageDone}
};
