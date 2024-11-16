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
    int GetCurHp() const { return CurStats.HP; }
    int GetMaxHP() const { return BaseStats.HP; }
    std::string GetPKMN_Name() const { return PKMN_DEF[NAME];}
    std::string GetPKMN_NickName();

    void SetNickname(const std::string& newName) { nickname = newName; }
    void SetLevel(int newLevel) { LVL = newLevel; }
    void SetInitialStatValues();
    void SetMovements();
    void SetPP(int id, int PPat);
    std::string GetMovementInfo(int MovementNum, int column);

    void parseCSV(const std::string& filename, std::vector<std::string>& DEF);
    enum AffectedStat {
        ATTACK,
        DEFENSE,
        SPEED,
        SP_ATK,
        SP_DEF,
        NONE // For natures with no effect
    };

    struct Natures {
        const char* name;
        AffectedStat increase;
        AffectedStat decrease;
    };

    struct Stats {
        int HP;
        int Attack;
        int Defense;
        int SP_Attack;
        int SP_Defense;
        int Speed;
        int Accuracy = 100;   // Default accuracy
        int Evasion = 0;      // Default evasion
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
    Stats& GetCurStats(){return CurStats;}

private:
    int SLOT;
    int ID;
    int LVL;
    int GSTAGE;
    int GENDER;
    std::string location;
    std::string nickname;

    Stats CurStats;
    Stats BaseStats;
    IndividualValues IV;
    EffortValues EV;
    Moves Moveset;
    OtherAttributes Attributes;
    int HPStatCalc ();
    int OtherStatCalc(int Stat, int IV, int EV, int StatID);
    void NatureMultiplyier(Natures nature);

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
        EGG_GROUP1,
        EGG_GROUP2,
        HATCH_TIME,
        HEIGHT_I,
        HEIGHT_M,
        WEIGHT_I,
        wEIGHT_M,
        MEGASTONE,
        EXP_YIELD,
        LEVELING_RATE,
        EV_YIELD,
        SHAPE,
        FOOTPRINT,
        POKEDEX_COLOR,
        FRIENDSHIP,
        BASE_HP,
        BASE_ATTACK,
        BASE_DEFENSE,
        BASE_SPATTACK,
        BASE_SPDEFENSE,
        BASE_SPEED,
        HELD_ITEM,
        TYPE_ID_1,
        TYPE_ID_2,
        OW_ID
    };
};
