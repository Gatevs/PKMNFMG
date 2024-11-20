#include "BattleManager.h"
#include "CSVCache.h"
#include <random>
#include <string>

BattleManager::BattleManager(std::string type, int id, std::string location){
    CSVCache& cache = CSVCache::GetInstance();
    cache.parseCSVFromString("assets/CSV/WILD_PKMN_LOC.csv", POKE_LOC_DEF, location);
    if (type == "WILD"){
        SetWildEvent(id, location);
    } else if(type == "TRAINER"){
        SetTrainerEvent(id, location);
    }
}

BattleManager::~BattleManager() {
    //clean
}


void BattleManager::SetWildEvent(int id, const std::string& location) {
    std::vector<int> pokesAtLocation;

    std::random_device seed;
    std::mt19937 engine(seed());

    // Get the Pokémon IDs string for the location
    std::string pokemonIdsString = POKE_LOC_DEF[2]; // Adjust index as needed
    int MIN_LVL = std::stoi(POKE_LOC_DEF[3]);
    int MAX_LVL = std::stoi(POKE_LOC_DEF[4]);

    // Use a stringstream to parse the IDs
    std::stringstream ss(pokemonIdsString);
    std::string poke;

    // Split the string by '&' and add each ID to the vector
    while (std::getline(ss, poke, '&')) {
        poke.erase(0, poke.find_first_not_of('0')); // Remove leading zeros if any
        int PokeID = std::stoi(poke);              // Convert string to int
        pokesAtLocation.push_back(PokeID);         // Add to vector
    }

    std::uniform_int_distribution<int> gen(0, pokesAtLocation.size() - 1); // uniform, unbiased
    std::uniform_int_distribution<int> lvl(MIN_LVL, MAX_LVL); // uniform, unbiased
    std::uniform_int_distribution<int> gender(0, 1); // uniform, unbiased

    int randomIndex = gen(engine);
    WILD_ID = pokesAtLocation[randomIndex];
    WILD_LVL = lvl(engine);
    WILD_GENDER = gender(engine);

    // Optionally, log the IDs for debugging
    std::cout << "Pokémon at location " << location << ": ";
    for (int id : pokesAtLocation) {
        std::cout << id << " ";
    }
    std::cout << std::endl;

    // Use pokesAtLocation for further logic in your BattleManager
}


void BattleManager::SetTrainerEvent(int id, std::string location){
}
