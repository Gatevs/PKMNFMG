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

struct unrelated {
    int currentFrameIndex = -1;
    int animTimer = 0;
    bool animFrame = true;
    bool tileAnimReverse = false;
};

class TileMap {
public:
    TileMap();
    ~TileMap();

    void loadLDtkMap(const std::string& filePath, std::function<void()> callback);
    void initialize(const std::string& Lvl);
    void Unload();
    void draw(const std::string render, Vector2 lvl_pos);
    void update(const std::string lvl, unrelated& animationState, Player& player_obj);
    void InFrontObjs();
    const ldtk::Layer& GetCOL();
    ldtk::Project ldtk_project;
    void loadPlayer(Player& player_obj);
    void loadNPCs(std::vector<NPC>& NPC_objs);
    void loadTileObjs(const std::string lvl, std::vector<tileObj>& Tile_objs);
    void IsWarpClose(Player& player_obj);
    Vector2 GetSwapLevel();
    Vector2 GetCurLevel();
    Vector2 GetlevelOffset();
    bool IsNextLevelLoaded();
    bool StandingOnWarp();
    void SwapLevels(Player& player_obj);
    bool GetDrawNextLevel();
    void SetDrawNextLevel();
    std::string GetCurLevelName();
    std::string GetSwapLevelName();
    void unloadFarAwayLevel(Player& player_obj, std::vector<tileObj>& Tile_objs);
    bool IsCameraLockNear(Player& player_obj);
    int GetLockCameraAxis() const {return LockCameraAxis;}


private:
    std::map<std::string, std::vector<int>> tileAnimationsMap;
    std::vector<std::pair<std::string, int>> firstFrameIds; // Vector to store animation name and first frame ID
    std::vector<int> frontTiles;
    Texture2D texture;
    RenderTexture2D renderer;
    RenderTexture2D inFront;
    RenderTexture2D swapRender;

    void LoadNextLevel(const ldtk::Entity& warpObj);
    void EnterNextlevel(Vector2 warpPos, Player& player_obj, int dir);

    int TILE_ANIM_TIME;
    std::string curLevel;
    std::string nextLevel;
    const int TILE_ANIM_NUM = 5;

    Vector2 loadLevel_Pos;
    Vector2 curLevel_Pos;
    Vector2 curLevel_Offset;
    bool NextLevelLoaded;
    bool DrawLoadedLevel;
    bool onWarp;
    Vector2 Warp_Pos;
    int Warp_Dir;
    int SwapLevel_Dir;
    int LockCameraAxis = 0;
};
