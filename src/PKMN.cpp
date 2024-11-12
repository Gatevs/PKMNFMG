#include "PKMN.h"
#include "raylib.h"
#include "rapidcsv.h"
#include <string>
#include <random>

PKMN::PKMN(int id, int level, int gender, int gstage){
    ID = id;
    LVL = level;
    GENDER = gender;
    GSTAGE = gstage;
    nickname = "NONE";
}

PKMN::~PKMN() {
    //clean
}

void PKMN::parseCSV(const std::string& filename) {
    rapidcsv::Document doc(filename);
    const auto& ids = doc.GetColumn<int>("ID");

    if(filename == "assets/CSV/PKMN_DB.csv"){
        for (size_t i = 0; i < ids.size(); ++i) {
            if (ids[i] == ID) {
                // Store entire row in PKMN-specific vector
                PKMN_DEF = doc.GetRow<std::string>(i);
                break; // Stop after finding the first matching ID
            }
        }
    }
}

std::string PKMN::GetPKMN_NickName(){
    if (nickname != "NONE"){
        return nickname;
    } else{
        std::string UpperCase = PKMN_DEF[NAME];
        std::transform(UpperCase.begin(),UpperCase.end(),UpperCase.begin (),::toupper);
        return UpperCase;
    }
}

void PKMN::SetStatValues(){
    int MIN = 1;
    int MAX = 31;

    std::random_device seed;
    std::mt19937 engine(seed());
    std::uniform_int_distribution<int> gen(MIN, MAX); // uniform, unbiased

    IV_HP = gen(engine);
    std::cout << IV_HP << std::endl;

    HP = (((2 * std::stoi(PKMN_DEF[BASE_HP]) + IV_HP + (EV_HP / 4)) * LVL) / 100) + LVL + 10;
    CUR_HP = HP;
    std::cout << HP << std::endl;
}
