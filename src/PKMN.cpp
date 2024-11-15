#include "PKMN.h"
#include "raylib.h"
#include "CSVCache.h"
#include <string>
#include <random>

PKMN::PKMN(int id, int level, int gender, int gstage){
    ID = id;
    LVL = level;
    GENDER = gender;
    GSTAGE = gstage;
    nickname = "NONE";
    parseCSV("assets/CSV/PKMN_DB.csv");
    SetStatValues();
    Moveset.Move1 ="CBT";
}

PKMN::~PKMN() {
    //clean
}

void PKMN::parseCSV(const std::string& filename) {
    CSVCache& cache = CSVCache::GetInstance();
    cache.LoadCSV(filename); // Load the file into memory (if not already loaded)

    try {
        // Query the row with the matching ID
        const auto& row = cache.GetRow(filename, 0, ID); // Assuming ID is in column 0
        PKMN_DEF = row; // Store the row in your Pokémon-specific vector
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving Pokémon data: " << e.what() << std::endl;
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

    IV.HP = gen(engine);
    std::cout << IV.HP << std::endl;

    HP = (((2 * std::stoi(PKMN_DEF[BASE_HP]) + IV.HP + (IV.HP / 4)) * LVL) / 100) + LVL + 10;
    CUR_HP = HP;
    std::cout << HP << std::endl;
}
