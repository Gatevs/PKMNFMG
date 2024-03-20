#pragma once

#include <map>
#include <string>
#include <raylib.h>

// Define walking texture paths for each NPC based on their current stage
namespace NPCWalk {
    // Function to dynamically construct texture path based on NPC ID and event number
    inline std::string GetTexturePath(int ID, int Stage) {
        return "assets/" + std::to_string(ID) + "_" + std::to_string(Stage) + ".png";
    }
}

// Define walking texture paths for each NPC based on their current stage
namespace NPCGrow {
    // Function to dynamically construct texture path based on NPC ID and event number
    inline std::string GetTexturePath(int ID, int Stage) {
        return "assets/" + std::to_string(ID) + "_G" + std::to_string(Stage) + ".png";
    }
}
