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
    SetInitialStatValues();
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

void PKMN::SetInitialStatValues(){
    int MIN = 1;
    int MAX = 31;
    int randomIndex = GetRandomValue(0, sizeof(natureTable) / sizeof(natureTable[0]) - 1);

    std::random_device seed;
    std::mt19937 engine(seed());
    std::uniform_int_distribution<int> gen(MIN, MAX); // uniform, unbiased

    Attributes.Nature = natureTable[randomIndex];
    std::cout << Attributes.Nature.name << std::endl;

    IV.HP = gen(engine);
    IV.Attack = gen(engine);
    IV.Defense = gen(engine);
    IV.SP_Attack = gen(engine);
    IV.SP_Defense = gen(engine);
    IV.Speed = gen(engine);

    BaseStats.HP = HPStatCalc();
    CUR_HP = BaseStats.HP;

    BaseStats.Attack = OtherStatCalc(std::stoi(PKMN_DEF[BASE_ATTACK]), IV.Attack, EV.Attack, ATTACK);
    std::cout << "Attack: " <<BaseStats.Attack << std::endl;

    BaseStats.Defense = OtherStatCalc(std::stoi(PKMN_DEF[BASE_DEFENSE]), IV.Defense, EV.Defense, DEFENSE);
    std::cout << "Defense: " <<BaseStats.Defense << std::endl;

    BaseStats.Speed = OtherStatCalc(std::stoi(PKMN_DEF[BASE_SPEED]), IV.Speed, EV.Speed, SPEED);
    std::cout << "Speed: " <<BaseStats.Speed << std::endl;

    BaseStats.SP_Attack = OtherStatCalc(std::stoi(PKMN_DEF[BASE_SPATTACK]), IV.SP_Attack, EV.SP_Attack, SP_ATK);
    std::cout << "SP_Attack: " <<BaseStats.SP_Attack << std::endl;

    BaseStats.SP_Defense = OtherStatCalc(std::stoi(PKMN_DEF[BASE_SPDEFENSE]), IV.SP_Defense, EV.SP_Defense, SP_DEF);
    std::cout << "SP_Defense: " <<BaseStats.SP_Defense << std::endl;
}

int PKMN::GetAttackDamage(std::vector<int> enemyTypes, int D, int move){
    critHit = false;
    std::random_device seed;
    std::mt19937 engine(seed());
    std::uniform_int_distribution<int> crit(1, TemporaryStats.Crit); // uniform, unbiased
    std::uniform_int_distribution<int> random(85, 100); // uniform, unbiased
    int CRIT_RTD = crit(engine);

    std::string MOVE_TYPE = GetMovementInfo(move,3);
    std::string ATTACK_TYPE = GetMovementInfo(move,2);
    int ATTACK_TYPE_ID = std::stoi(GetMovementInfo(move,8));
    std::string ABILITY = PKMN_DEF[FIRST_ABILITY];
    int MOVE_POWER = std::stoi(GetMovementInfo(move,5));
    int A = BaseStats.Attack;
    float BURN = 1.0f;
    float SCREEN = 1.0f;
    float WEATHER = 1.0f;
    float FF = 1.0f;
    float CRITICAL = 1.0f;
    float ITEM = 1.0f;
    float FIRST = 1.0f;
    float RANDOM = random(engine)/100.0f;
    float STAB = 1.0f;
    float TYPE_1 = GetTypeEffectiveness(ATTACK_TYPE_ID,enemyTypes[0]);
    float TYPE_2 = GetTypeEffectiveness(ATTACK_TYPE_ID,enemyTypes[1]);
    float SRF = 1.0f;
    float EB = 1.0f;
    float TL = 1.0f;
    float BERRY = 1.0f;

    if (STATUS == StatusCondition::BURN && ABILITY != "Guts" && MOVE_TYPE == "Physical"){
        BURN = 0.5f;
    }
    if (CRIT_RTD == 1){
        critHit = true;
        CRITICAL = 2.0f;
        if (ABILITY == "Sniper"){
            CRITICAL = 3.0f;
        }
    }
    if (Attributes.Item == "Life Orb"){
        ITEM = 1.3f;
    }
    if (move == 382){
        FIRST = 1.5f;
    }
    if (ATTACK_TYPE == PKMN_DEF[TYPE1] or ATTACK_TYPE == PKMN_DEF[TYPE2]){
        STAB = 1.5f;
        if (ABILITY == "Adaptability"){
            STAB = 2.0f;
        }
    }


    int CALC1 = ((2*LVL)/5) + 2;
    int CALC2 = (CALC1 * MOVE_POWER * (A/D)) / 50;
    int CALC3 = CALC2 * BURN * SCREEN * WEATHER * FF + 2;
    int CALC4 = CALC3 * CRITICAL * ITEM * FIRST * RANDOM * STAB * TYPE_1 * TYPE_2 * SRF * EB * TL * BERRY;

    return CALC4;
}

int PKMN::GetOddsEscape(int speedPlayer, int speedWild, int Attempts){
    int OddsEscape = ((speedPlayer * 128) / speedWild + (30 * Attempts)) % 256;
    return OddsEscape;
}

int PKMN::HPStatCalc(){
    int calc = (((2 * std::stoi(PKMN_DEF[BASE_HP]) + IV.HP + (EV.HP / 4)) * LVL) / 100) + LVL + 10;
    return calc;
}

int PKMN::OtherStatCalc(int Stat, int IV, int EV, int StatID){
    float StatMultiplier = 1.0f;

    if (StatID == Attributes.Nature.increase){
        StatMultiplier = 1.1f;
    }
    if (StatID == Attributes.Nature.decrease){
        StatMultiplier = 0.9f;
    }

    int calc = (((((2 * Stat) + IV + (float(EV)/4)) * LVL) / 100) + 5) * StatMultiplier;
    return calc;
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
    }
}

std::string PKMN::GetMovementInfo(int MovementNum, int column){
    CSVCache& cache = CSVCache::GetInstance();
    std::string filename = "assets/CSV/Movements.csv";
    cache.LoadCSV(filename); // Load the file into memory (if not already loaded)

    try {
        // Query the row with the matching ID
        if (MovementNum != 0){
            const auto& row = cache.GetRow(filename, 0, MovementNum);
            return row[column];
        } else {
            return "NONE";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error retrieving movement data: " << e.what() << std::endl;
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

std::vector<int> PKMN::GetPokemonTypes(){
    std::vector<int> Types;
    Types.push_back(std::stoi(PKMN_DEF[TYPE_ID_1]));
    Types.push_back(std::stoi(PKMN_DEF[TYPE_ID_2]));
    return Types;
}

float PKMN::GetTypeEffectiveness(int A_Type, int E_Type){
    float EffectiveValue;
    CSVCache& cache = CSVCache::GetInstance();
    std::string filename = "assets/CSV/TypeChart.csv";
    cache.LoadCSV(filename); // Load the file into memory (if not already loaded)
    try {
        // Query the row with the matching ID
        if (E_Type != 0 ){
            const auto& row = cache.GetRow(filename, 0, 1);
            EffectiveValue = std::stof(row[1]);
            return EffectiveValue;
        }else{
            return 1.0f;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error retrieving Type data: " << e.what() << std::endl;
        return 1.0f;
    }
}

float PKMN::GetStatMultiplier(int stat) {
    if (stat < 0) return 2.0f / ((stat * -1) + 2); // Decrease by ~1/6 per stat
    else return (stat + 2) / 2.0f;             // Increase by 0.5 per stage
}
