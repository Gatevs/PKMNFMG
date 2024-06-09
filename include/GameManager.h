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

    void DebugIntro();
    void GameInitialization(std::string map);
    void CameraUpdate();
    void GameLoop(float FixedDt);
    void DrawWorld();
    void DrawBars();
    void Unload();
    bool IsIntroFinished() {return IntroFinished;}
    bool IsVNMenu() {return VNMenu;}
    void DrawIntro();
    void DrawOutsideItems();
private:
    void JsonSaveNPCData();
    void JsonLoadNPCData();
    void WarpPlayer(std::string where);
    int test;
    Color green = {20,160,133,254};
    const int gameWidth = 256;
    const int gameHeight = 192;
    int scaleFactor;
    // Camera zoom and offset
    Camera2D camera = { 0 };
    Camera2D bigCamera = { 1 };
    bool lockCamera = false;
    int WarpingPlayer = 0;
    Vector2 targetPos;
    Vector2 BottomBarPos;
    TileMap Outside;
    ActionHandler Menu;
    Player player;
    std::vector<NPC> npcs;
    std::vector<tileObj> tileObjs;
    Texture2D ShadowCentered;
    Texture2D ShadowOffCenter;
    unrelated swapper;
    unrelated cur;
    bool IntroFinished = false;
    bool VNMenu = false;
    Texture2D IntroBG = LoadTexture("assets/MISC/DEBUG_CHAR.png");
    Texture2D IntroSelect = LoadTexture("assets/MISC/DEBUG_SELECT.png");
    Texture2D VNTest;
    Sound EntityGrowth;
    Font BagFont = LoadFont("assets/SPRITEFONT/SpriteFont_Bag.png");;
    int DebugID = 1;
    enum UI_Element {
        PAUSE,
        DIALOGUE,
        ACTION,
        NONE
    };
    Vector2 SelectPos = (Vector2){18,6};
    const Rectangle Select = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(92),
        static_cast<float>(136)
    };
};

// Define a custom comparison function for sorting GameObject pointers
inline bool CompareByYPosition(const GameObject* a, const GameObject* b) {
    return a->position.y < b->position.y;
}


