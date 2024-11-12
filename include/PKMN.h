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

    void parseCSV(const std::string& filename);

private:
    int ID;
    int LVL;
    int GSTAGE;
    int GENDER;
    int CUR_HP;
    int HP;
    int IV_HP;
    int EV_HP = 0;
    std::string location;
    std::string nickname;
    std::string ability;

    std::vector<std::string> PKMN_DEF;

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
