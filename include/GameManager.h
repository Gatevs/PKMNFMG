#pragma once

#include "ActionHandler.h"
#include "TileMap.h"
#include <raylib.h>
#include "player.h"
#include <LDtkLoader/Project.hpp>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

class GameManager{
public:
    GameManager() : player("assets/player.png"), Menu("assets/UI_Atlas.png"){};
    ~GameManager();

    void GameInitialization();
    void CameraUpdate();
    void GameLoop();
    void DrawWorld();
    void DrawBars();
    void Unload();

private:
    int test;
    Color green = {20,160,133,254};
    const int gameWidth = 256;
    const int gameHeight = 192;
    int scaleFactor;
    // Camera zoom and offset
    Camera2D camera = { 0 };
    Vector2 cameraHelper;
    TileMap Outside;
    ActionHandler Menu;
    Player player;
    std::vector<NPC> npcs;
    Texture2D ShadowCentered;
    Texture2D ShadowOffCenter;
};

// Define a custom comparison function for sorting GameObject pointers
inline bool CompareByYPosition(const GameObject* a, const GameObject* b) {
    return a->position.y < b->position.y;
}

