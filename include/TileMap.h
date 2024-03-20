#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <LDtkLoader/Project.hpp>
#include "npc.h"
#include "player.h"
#include "raylib.h"

class TileMap {
public:
    TileMap();
    ~TileMap();

    void loadLDtkMap(const std::string& filePath);
    void initialize(const std::string& Lvl);
    void Unload();
    void draw();
    void update();
    const ldtk::Layer& GetCOL();
    ldtk::Project ldtk_project;
    void loadPlayer(Player& player_obj);
    void loadNPCs(std::vector<NPC>& NPC_objs);


private:
    std::map<std::string, std::vector<int>> tileAnimationsMap;
    std::vector<std::pair<std::string, int>> firstFrameIds; // Vector to store animation name and first frame ID
    Texture2D texture;
    RenderTexture2D renderer;

    int currentFrameIndex;
    bool tileAnimReverse;
    int animTimer;
    bool animFrame;
    int TILE_ANIM_TIME;
    const int TILE_ANIM_NUM = 3;
};
