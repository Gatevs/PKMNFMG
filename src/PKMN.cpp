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
    parseCSV("assets/CSV/PKMN_DB.csv", PKMN_DEF);
    parseCSV("assets/CSV/PKMN_LevelUp.csv", PKMNLVLUP_DEF);
    SetStatValues();
    SetMovements();
}

PKMN::~PKMN() {
    //clean
}

void PKMN::parseCSV(const std::string& filename, std::vector<std::string>& DEF) {
    CSVCache& cache = CSVCache::GetInstance();
    cache.LoadCSV(filename); // Load the file into memory (if not already loaded)

    try {
        // Query the row with the matching ID
        const auto& row = cache.GetRow(filename, 0, ID); // Assuming ID is in column 0
        DEF = row; // Store the row in your Pokémon-specific vector
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

void PKMN::SetMovements(){
    std::vector<int> assignedMoves;

    // Start from the Pokémon's current level and work backward
    for (int lvl = LVL - 1; lvl > 0 && assignedMoves.size() < 4; --lvl) {
        if (lvl < PKMNLVLUP_DEF.size() && PKMNLVLUP_DEF[lvl] != "0") { // Check if valid move exists
            std::stringstream ss(PKMNLVLUP_DEF[lvl]);
            std::string move;

            // Split multiple moves (e.g., "033&039")
            while (std::getline(ss, move, '&')) {
                move.erase(0, move.find_first_not_of('0'));
                int MoveID = std::stoi(move);
                if (assignedMoves.size() < 4) { // Assign up to 4 moves
                    assignedMoves.push_back(MoveID);
                } else {
                    break;
                }
            }
        }
    }
    for (int i = 0; i < assignedMoves.size(); i++){
        Moveset.Move[i] = assignedMoves[i];
        SetPP(Moveset.Move[i], i);
        std::cout << "Move ID: "<< Moveset.Move[i] << std::endl;
        std::cout << "PP Value: "<< Moveset.PP[i] << std::endl;
    }
}

std::string PKMN::GetMovementName(int MovementNum){
    CSVCache& cache = CSVCache::GetInstance();
    std::string filename = "assets/CSV/Movements.csv";
    cache.LoadCSV(filename); // Load the file into memory (if not already loaded)

    try {
        // Query the row with the matching ID
        if (MovementNum != 0){
            const auto& row = cache.GetRow(filename, 0, MovementNum);
            return row[1];
        } else {
            return "NONE";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error retrieving Pokémon data: " << e.what() << std::endl;
        return "Struggle";
    }
}

void PKMN::SetPP(int id, int PPat){
    CSVCache& cache = CSVCache::GetInstance();
    std::string filename = "assets/CSV/Movements.csv";
    cache.LoadCSV(filename); // Load the file into memory (if not already loaded)

    try {
        // Query the row with the matching ID
        const auto& row = cache.GetRow(filename, 0, id);
        Moveset.PP[PPat] = std::stoi(row[4]);

    } catch (const std::exception& e) {
        std::cerr << "Error retrieving Pokémon data: " << e.what() << std::endl;
    }
}
