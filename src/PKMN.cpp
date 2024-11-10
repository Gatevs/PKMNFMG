#include "PKMN.h"
#include "raylib.h"
#include "rapidcsv.h"

PKMN::PKMN(int id, int level, int gender, int gstage){
    ID = id;
    LVL = level;
    GENDER = gender;
    GSTAGE = gstage;
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
