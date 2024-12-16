#pragma once

#include <raylib.h>
#include <string>
#include <vector>

class PKMN {
public:
    PKMN(int id, int level, int gender, int gstage);
    ~PKMN();

    // Getter and setter functions
    int GetLevel() const { return LVL; }
    int GetID() const { return ID; }
    int GetGStage() const { return GSTAGE; }
    int GetGender() const { return GENDER; }
    int GetCurHp() const { return CUR_HP; }
    int GetMaxHP() const { return BaseStats.HP; }
    std::string GetPKMN_Name() const { return PKMN_DEF[NAME];}
    std::string GetPKMN_NickName();
    std::string GetMoveType(int move) { return GetMovementInfo(move,3);}
    std::string GetMoveFunctionCode(int move) { return GetMovementInfo(move,10);}
    int GetAttackDamage(std::vector<int> enemyTypes, int D, int move);
    int GetTurnDamage() {return TurnAttackDamage;}
    int GetHealthAmount() {return TurnHealAmount;}
    int GetNextMovePhase() {return NextMovePhase;}
    int GetOddsEscape(int speedPlayer, int speedWild, int Attempts);
    void SetBattleStatusMultiplier(int affected, int multiplier, std::string affectedName);
    void SetStatusValue(int& stat, int multiplier, std::string statname);
    std::string GetStatusText(){ return StatusChangeText;}

    void SetNickname(const std::string& newName) { nickname = newName; }
    void SetLevel(int newLevel) { LVL = newLevel; }
    void SetGStage(int newStage) {GSTAGE = newStage; }
    void SetInitialStatValues();
    void SetMovements();
    void SetPP(int id, int PPat);
    void SetCurHP (int value) {CUR_HP = value;}
    void SetStatusText(std::string text) {StatusChangeText = text;}
    void SetHealthAmount(int amount) {TurnHealAmount = amount;}
    void SetNextMovePhase(int phase) {NextMovePhase = phase;}
    std::string GetMovementInfo(int MovementNum, int column);
    float GetTypeEffectiveness(int A_Type, int E_Type);
    float GetStatMultiplier(int stat);
    std::vector<int> GetPokemonTypes();
    bool IsCrit() {return critHit;}
    void ResetTempStats();
    void SetCurPP(int PPnumber, int PPValue) { Moveset.PP[PPnumber] = PPValue;}

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

    enum class StatusCondition {
        NONE,
        BURN,
        FREEZE,
        PARALYSIS,
        POISON,
        SLEEP
    };

    struct Natures {
        const char* name;
        AffectedStat increase;
        AffectedStat decrease;
    };

    struct TempStats {
        int HP = 0;
        int Attack = 0;
        int Defense = 0;
        int SP_Attack = 0;
        int SP_Defense = 0;
        int Speed = 0;
        int Accuracy = 100;   // Default accuracy
        int Evasion = 0;      // Default evasion
        int Crit = 16;
    };

    struct Stats {
        int HP;
        int Attack;
        int Defense;
        int SP_Attack;
        int SP_Defense;
        int Speed;
    };

    struct IndividualValues {
        int HP;
        int Attack;
        int Defense;
        int SP_Attack;
        int SP_Defense;
        int Speed;
    };

    struct EffortValues {
        int HP = 0;
        int Attack = 0;
        int Defense = 0;
        int SP_Attack = 0;
        int SP_Defense = 0;
        int Speed = 0;
        int Total = 0;
    };

    struct Moves {
        int Move[4] = {0,0,0,0};
        int PP[4] = {0,0,0,0};
    };

    struct OtherAttributes {
        Natures Nature;
        std::string Item;
        std::string Status;
        std::string Ability;
        bool Away;
        bool InOW; // In overworld
    };

    Moves& GetMovements() {return Moveset;}
    Stats& GetBaseStats() {return BaseStats;}
    TempStats& GetTempStats() {return TemporaryStats;}

private:
    int SLOT;
    int ID;
    int LVL;
    int GSTAGE;
    int GENDER;
    int CUR_HP;
    StatusCondition STATUS = StatusCondition::NONE;
    std::string location;
    std::string nickname;
    bool critHit = false;

    int TurnAttackDamage = 0;
    int TurnHealAmount = 0;

    Stats BaseStats;
    TempStats TemporaryStats;
    IndividualValues IV;
    EffortValues EV;
    Moves Moveset;
    OtherAttributes Attributes;
    int HPStatCalc ();
    int OtherStatCalc(int Stat, int IV, int EV, int StatID);
    void NatureMultiplyier(Natures nature);
    std::string StatusChangeText;
    int NextMovePhase = 0;

    std::vector<std::string> PKMN_DEF;
    std::vector<std::string> PKMNLVLUP_DEF;

    Natures natureTable[25] = {
        {"Hardy", NONE, NONE},
        {"Lonely", ATTACK, DEFENSE},
        {"Brave", ATTACK, SPEED},
        {"Adamant", ATTACK, SP_ATK},
        {"Naughty", ATTACK, SP_DEF},
        {"Bold", DEFENSE, ATTACK},
        {"Docile", NONE, NONE},
        {"Relaxed", DEFENSE, SPEED},
        {"Impish", DEFENSE, SP_ATK},
        {"Lax", DEFENSE, SP_DEF},
        {"Timid", SPEED, ATTACK},
        {"Hasty", SPEED, DEFENSE},
        {"Serious", NONE, NONE},
        {"Jolly", SPEED, SP_ATK},
        {"Naive", SPEED, SP_DEF},
        {"Modest", SP_ATK, ATTACK},
        {"Mild", SP_ATK, DEFENSE},
        {"Quiet", SP_ATK, SPEED},
        {"Bashful", NONE, NONE},
        {"Rash", SP_ATK, SP_DEF},
        {"Calm", SP_DEF, ATTACK},
        {"Gentle", SP_DEF, DEFENSE},
        {"Sassy", SP_DEF, SPEED},
        {"Careful", SP_DEF, SP_ATK},
        {"Quirky", NONE, NONE}
    };

    enum PKMN_DB{
        INDEX,
        NAME,
        TYPE1,
        TYPE2,
        FIRST_ABILITY,
        SECOND_ABILITY,
        GENDER_RATIO,
        CATCH_RATE,
        EGG_GROUPS,
        HATCH_STEPS,
        HEIGHT,
        WEIGHT,
        EXP_YIELD,
        LEVELING_RATE,
        EV_YIELD,
        SHAPE,
        POKEDEX_COLOR,
        HAPPINESS,
        BASE_HP,
        BASE_ATTACK,
        BASE_DEFENSE,
        BASE_SPATTACK,
        BASE_SPDEFENSE,
        BASE_SPEED,
        HIDDEN_ABILITIES,
        HABITAT,
        CATEGORY,
        POKEDEX,
        GENERATION,
        EVOLUTION,
        TYPE_ID_1,
        TYPE_ID_2,
        OW_ID
    };
};
