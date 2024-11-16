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
    int GetMaxHP() const { return HP; }
    std::string GetPKMN_Name() const { return PKMN_DEF[NAME];}
    std::string GetPKMN_NickName();

    void SetNickname(const std::string& newName) { nickname = newName; }
    void SetLevel(int newLevel) { LVL = newLevel; }
    void SetAbility(const std::string& newAbility) { ability = newAbility; }
    void SetStatValues();
    void SetMovements();
    void SetPP(int id, int PPat);
    std::string GetMovementName(int MovementNum);

    void parseCSV(const std::string& filename, std::vector<std::string>& DEF);

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
        int HP;
        int Attack;
        int Defense;
        int SP_Attack;
        int SP_Defense;
        int Speed;
        int Total;
    };

    struct Moves {
        int Move[4] = {0,0,0,0};
        int PP[4] = {0,0,0,0};
    };

    struct OtherAttributes {
        std::string Nature;
        std::string Item;
        std::string Status;
        std::string Ability;
        bool Away;
        bool InOW; // In overworld
    };

    Moves& GetMovements() {return Moveset;}

private:
    int SLOT;
    int ID;
    int LVL;
    int GSTAGE;
    int GENDER;
    int CUR_HP;
    int HP;
    std::string location;
    std::string nickname;
    std::string ability;

    Stats BaseStats;
    IndividualValues IV;
    EffortValues EV;
    Moves Moveset;
    OtherAttributes Attributes;

    std::vector<std::string> PKMN_DEF;
    std::vector<std::string> PKMNLVLUP_DEF;

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
