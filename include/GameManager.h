#pragma once

#include "ActionHandler.h"
#include "TileMap.h"
#include <raylib.h>
#include "TileObjects.h"
#include "player.h"
#include <LDtkLoader/Project.hpp>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

class GameManager{
public:
    GameManager();
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
    std::vector<tileObj> tileObjs;
    Texture2D ShadowCentered;
    Texture2D ShadowOffCenter;
    unrelated swapper;
    unrelated cur;
    enum UI_Element {
        PAUSE,
        DIALOGUE,
        ACTION,
        NONE
    };
};

// Define a custom comparison function for sorting GameObject pointers
inline bool CompareByYPosition(const GameObject* a, const GameObject* b) {
    return a->position.y < b->position.y;
}


