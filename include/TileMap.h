#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <LDtkLoader/Project.hpp>
#include "npc.h"
#include "player.h"
#include "raylib.h"
#include "TileObjects.h"

class TileMap {
public:
    TileMap();
    ~TileMap();

    void loadLDtkMap(const std::string& filePath);
    void initialize(const std::string& Lvl);
    void Unload();
    void draw(const std::string render);
    void update();
    void InFrontObjs();
    const ldtk::Layer& GetCOL();
    ldtk::Project ldtk_project;
    void loadPlayer(Player& player_obj);
    void loadNPCs(std::vector<NPC>& NPC_objs);
    void loadTileObjs(std::vector<tileObj>& Tile_objs);


private:
    std::map<std::string, std::vector<int>> tileAnimationsMap;
    std::vector<std::pair<std::string, int>> firstFrameIds; // Vector to store animation name and first frame ID
    std::vector<int> frontTiles;
    Texture2D texture;
    RenderTexture2D renderer;
    RenderTexture2D inFront;

    int currentFrameIndex;
    bool tileAnimReverse;
    int animTimer;
    bool animFrame;
    int TILE_ANIM_TIME;
    const int TILE_ANIM_NUM = 3;
};
