# pragma once

#include "PKMN.h"
#include <map>
#include <raylib.h>
#include <string>
#include <vector>

class BattleManager {
public:
    BattleManager(std::string type, int id, std::string location);
    ~BattleManager();
    PKMN GetRandomPokemonForLocation(const std::string& location);
    void SetWildEvent(int id, const std::string& location);
    void SetTrainerEvent(int id, std::string location);
    void parseCSV(const std::string& filename, std::vector<std::string>& DEF);
    int GetWildPKMN_ID() {return WILD_ID;}
    int GetWildPKMN_LVL() {return WILD_LVL;}
    int GetWildPKMN_GENDER() {return WILD_GENDER;}
    int GetWildPKMN_GSTAGE() {return WILD_GSTAGE;}

private:
    std::vector<std::string> POKE_LOC_DEF;
    int WILD_ID;
    int WILD_LVL;
    int WILD_GENDER;
    int WILD_GSTAGE = 0;

    void LoadBattleData(); // Load Pokémon and trainer data.
};
