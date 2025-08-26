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

struct AnimationState {
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
    void update(const std::string lvl, AnimationState& animationState, Vector2 pos);
    void InFrontObjs();
    const ldtk::Layer& GetCOL();
    ldtk::Project ldtk_project;
    void loadPlayer(Player& player_obj);
    void loadNPCs(const std::string lvl, Player& player_obj, std::vector<NPC>& NPC_objs, bool initialization);
    void loadTileObjs(const std::string lvl, std::vector<tileObj>& Tile_objs);
    void IsWarpClose(Player& player_obj);
    Vector2 GetSwapLevel();
    Vector2 GetCurLevel();
    Vector2 GetlevelOffset() const;
    bool IsNextLevelLoaded();
    bool StandingOnWarp();
    void SwapLevels(Player& player_obj);
    bool GetDrawNextLevel();
    void SetDrawNextLevel();
    bool GetLoadingDone() const {return loadingDone; }
    std::string GetCurLevelName();
    std::string GetSwapLevelName();
    void unloadFarAwayLevel(Player& player_obj, std::vector<tileObj>& Tile_objs);
    bool IsCameraLockNear(Player& player_obj);
    Vector2 GetLockCameraAxis() const {return LockCameraAxis;}
    void PlayerInTallGrass(Player& player_obj);
    void DrawLocationCard();
    void DrawGrass(Player& player_obj,std::vector<tileObj>& Tile_objs);
    void parseCSV();
    Vector2 InitLockDirection(Player& player_obj);
    std::string IndoorWarpTo(Player& player_obj);
    int getCollision(int x, int y) const;

private:
    void initializeCollisionGrid(const std::string& levelName);
    void preprocessEntityCollisions(const std::string& levelName);
    std::vector<int> m_collision_grid;
    int m_collision_grid_width;
    struct AnimatedTile {
        Vector2 position;
        std::string animationName;
    };
    std::vector<AnimatedTile> animated_tiles;
    std::map<std::string, std::vector<int>> tileAnimationsMap;
    std::vector<std::pair<std::string, int>> firstFrameIds; // Vector to store animation name and first frame ID
    std::vector<int> frontTiles;
    bool loadingDone = false;
    Texture2D texture;
    Texture2D locationCard_Texture;
    RenderTexture2D renderer;
    RenderTexture2D inFront;
    RenderTexture2D swapRender;

    Texture2D tallGrass_Texture;
    struct TallGrass{
        bool Visible = false;
        bool Active = false;
        bool set = false;
        bool startAnim = false;
        Vector2 Position1;
        Vector2 Position2;
        Vector2 Position3;
        int TrailIndex = 0;
        std::vector<std::pair<float, int>> trailTimer;
        std::vector<Vector2> trailPos;
    };
    TallGrass grass;
    void GrassTrail();

    void LoadNextLevel(const ldtk::Entity& warpObj);
    void EnterNextlevel(Vector2 warpPos, Player& player_obj, int dir);
    void ShowLocationCard(Vector2 pos);

    int TILE_ANIM_TIME;
    std::string curLevel;
    std::string nextLevel;
    const int TILE_ANIM_NUM = 5;

    struct LocationCard {
        std::string Name;
        Vector2 Position;
        float Timer = 0.0f;
        float CardYPos = 0.0f;
        bool Hide = false;
        bool Active = false;
        int Index_X = 0;
        int Index_Y = 0;
    };
    LocationCard Card;

    Vector2 loadLevel_Pos;
    Vector2 curLevel_Pos;
    bool NextLevelLoaded;
    bool DrawLoadedLevel;
    bool onWarp;
    Vector2 Warp_Pos;
    int Warp_Dir;
    int SwapLevel_Dir;
    Vector2 LockCameraAxis = {0,0};
};
