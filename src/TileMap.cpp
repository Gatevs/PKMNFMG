#include "TileMap.h"
#include "TileObjects.h"
#include "rapidcsv.h"
#include "raylib.h"
#include <string>

TileMap::TileMap() {
    locationCard_Texture = LoadTexture("assets/TILEMAPS/LocationCard.png");
    tallGrass_Texture = LoadTexture("assets/MISC/tall_grass.png");
    DrawLoadedLevel = false;
    NextLevelLoaded = false;
    onWarp = false;
    TILE_ANIM_TIME = 50;
    curLevel = "Player_Home";
    nextLevel = "Swolie_Town";
}

TileMap::~TileMap() {
}


void TileMap::loadLDtkMap(const std::string& filePath, std::function<void()> callback) {
    try {
        ldtk_project.loadFromFile(filePath);
        // Call the callback function once the map has been loaded successfully
        callback();
    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Configure the tilemap
void TileMap::initialize(const std::string& Lvl) {
    loadingDone = false;
    curLevel = Lvl;
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(Lvl);
    const auto& layer = level.getLayer("BG");
    const auto& tiles_vector = layer.allTiles();

    // Load texture and renderer
    texture = LoadTexture(("assets/TILEMAPS/" + layer.getTileset().path).c_str());
    renderer = LoadRenderTexture(level.size.x, level.size.y);

    curLevel_Pos = (Vector2) {static_cast<float>(level.position.x), static_cast<float>(level.position.y)};

    // Load tile animations
    const auto& tileAnim = ldtk_project.getEnum("Animations");

    for (int i = 1; i <= TILE_ANIM_NUM; ++i) {
        std::string animationName = "ANIM_" + std::to_string(i);
        const auto& animTiles = layer.getTileset().getTilesWithTagEnum(tileAnim[animationName]);
        tileAnimationsMap[animationName] = animTiles;
    }

    for (const auto& pair : tileAnimationsMap) {
        const std::string& animationName = pair.first; // Get the animation name
        const std::vector<int>& animationFrames = pair.second; // Get the vector of animation frames
        if (!animationFrames.empty()) {
            int firstFrameId = animationFrames.front(); // Get the first frame ID
            firstFrameIds.push_back(std::make_pair(animationName, firstFrameId)); // Add animation name and first frame ID to the vector
        }
    }

    // Draw all the tiles.
    BeginTextureMode(renderer);
    ClearBackground(BLACK);
    for (const auto &tile : tiles_vector) {
        const auto& position = tile.getPosition();
        const auto& texture_rect = tile.getTextureRect();
        Vector2 dest = {
            static_cast<float>(position.x),
            static_cast<float>(position.y),
        };
        Rectangle src = {
            static_cast<float>(texture_rect.x),
            static_cast<float>(texture_rect.y),
            static_cast<float>(texture_rect.width) * (tile.flipX ? -1.0f : 1.0f),
            static_cast<float>(texture_rect.height) * (tile.flipY ? -1.0f : 1.0f)
        };
        DrawTextureRec(texture, src, dest, WHITE);
    }
    EndTextureMode();
    loadingDone = true;
}

// Draw the tilemap
void TileMap::draw(const std::string render, Vector2 lvl_Pos) {
    RenderTexture2D drawer;
    if (render == "Current"){
        drawer = renderer;
    } else if (render == "Swap"){
        drawer = swapRender;
    }
    Rectangle src = {
        0,
        0,
        static_cast<float>(drawer.texture.width),
        -static_cast<float>(drawer.texture.height)
    };
    Rectangle dest = {
        lvl_Pos.x,
        lvl_Pos.y,
        static_cast<float>(drawer.texture.width) * 1,
        static_cast<float>(drawer.texture.height) * 1
    };
    DrawTexturePro(drawer.texture, src, dest, {0}, .0f, WHITE);
}

// Animate Tiles
void TileMap::update(const std::string lvl, unrelated& animationState, Vector2 pos) {
    RenderTexture2D drawer;
    if (Card.Active){
        ShowLocationCard(pos);
    }
    if (lvl == curLevel){
        drawer = renderer;
    } else if (lvl == nextLevel){
        drawer = swapRender;
    }

    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(lvl);
    const auto& layer = level.getLayer("BG");
    const auto& tiles_vector = layer.allTiles();
    // If the timer reaches the set time, draw the next tile in the sequence
    if (animationState.animTimer != TILE_ANIM_TIME){
        animationState.animTimer += 1;
        animationState.animFrame = false;
    }else{
        animationState.animTimer = 0;
        animationState.animFrame = true;
    }

    if (animationState.animFrame){
        Vector2 StartPos = (Vector2){pos.x - 112, pos.y - 96};
        Vector2 EndPos = (Vector2){StartPos.x + 256, StartPos.y + 208};
        // Update the current frame index based on the tile animation direction.
        animationState.currentFrameIndex += animationState.tileAnimReverse ? -1 : 1;
        // Check if the current frame index is at the beginning or end of the animation sequence.
        // If so, toggle the tile animation direction by reversing the value of animationState.tileAnimReverse.
        if (animationState.currentFrameIndex == -1 || animationState.currentFrameIndex + 1 == 2) {
            animationState.tileAnimReverse = !animationState.tileAnimReverse;
        }
        ClearBackground(BLACK);
        BeginTextureMode(drawer);

        for (const auto &tile : tiles_vector) {
            const auto& position = tile.getPosition();
            const auto& texture_rect = tile.getTextureRect();
            int TILE_X = texture_rect.x;
            int TILE_Y = texture_rect.y;
            if (position.x > StartPos.x && position.x < EndPos.x && position.y > StartPos.y && position.y < EndPos.y){


            auto it = std::find_if(firstFrameIds.begin(), firstFrameIds.end(),
                                   [&](const std::pair<std::string, int>& pair) {
                                       return pair.second == tile.tileId;
                                   });
            if (it != firstFrameIds.end()) {
                int animIndex = std::distance(firstFrameIds.begin(), it);
                const auto& animTiles = tileAnimationsMap[firstFrameIds[animIndex].first];
                // Check if the current tile is part of the animated sequence
                if (tile.tileId == animTiles[0]){
                    // Set the tile texture to the texture from the tile that's next on the sequence
                    TILE_X = layer.getTileset().getTileTexturePos(animTiles[animationState.currentFrameIndex + 1]).x;
                    TILE_Y = layer.getTileset().getTileTexturePos(animTiles[animationState.currentFrameIndex + 1]).y;
                }
            }

            Vector2 dest = {
                static_cast<float>(position.x),
                static_cast<float>(position.y),
            };
            Rectangle src = {
                static_cast<float>((TILE_X)),
                static_cast<float>((TILE_Y)),
                static_cast<float>(texture_rect.width) * (tile.flipX ? -1.0f : 1.0f),
                static_cast<float>(texture_rect.height) * (tile.flipY ? -1.0f : 1.0f)
            };
            DrawTextureRec(texture, src, dest, WHITE);
        }
        }
        EndTextureMode();
    }
}

std::string TileMap::IndoorWarpTo(Player& player_obj){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Objects");
    const int TILE_SIZE = 16;
    for (const ldtk::Entity& wObj : objects.getEntitiesByName("Indoor_Warp")) {
        Rectangle WarpCol = {static_cast<float>(wObj.getWorldPosition().x - (TILE_SIZE)),static_cast<float>((wObj.getWorldPosition().y- (TILE_SIZE/2.0f) - TILE_SIZE)),TILE_SIZE,TILE_SIZE};
        std::string WarpGoingTo = wObj.getField<std::string>("Going_To").value();
        if (CheckCollisionRecs(player_obj.ColOffset(false),WarpCol)){
            return WarpGoingTo;
        }else {
            return "NULL";
        }
    }
    return "NULL";
}

void TileMap::IsWarpClose(Player& player_obj){
    const int TILE_SIZE = 16;
    int dirvalue = 0;

    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Objects");

    for (const ldtk::Entity& wObj : objects.getEntitiesByName("OW_Warp")) {
        auto wObj_Pos = wObj.getWorldPosition();
        auto wObj_Width = wObj.getSize().x;
        auto wObj_Height = wObj.getSize().y;
        int wObj_Dir = wObj.getField<int>("Dir").value();
        if (player_obj.GetPlayerDir() == 0 || player_obj.GetPlayerDir() == 270){
            dirvalue = 1;
        }else{
            dirvalue = -1;
        }

        if (player_obj.GetPosition().x > wObj_Pos.x && player_obj.GetPosition().x < (wObj_Pos.x + wObj_Width)){
            if (player_obj.GetPosition().y + (dirvalue * (9 * TILE_SIZE)) >= wObj_Pos.y && wObj_Dir == 270 ){
                if (!NextLevelLoaded){
                    LoadNextLevel(wObj);
                }
            }else if (player_obj.GetPosition().y + (dirvalue * (9 * TILE_SIZE)) <= wObj_Pos.y && wObj_Dir == 90){
                if (!NextLevelLoaded){
                    LoadNextLevel(wObj);
                }
            }
        }
        if (wObj_Pos.x == player_obj.GetPosition().x || wObj_Pos.y == player_obj.GetPosition().y){
            curLevel = level.name;
            std::string wname = wObj.getField<std::string>("Going_To").value();
            if (wObj_Dir == player_obj.GetPlayerDir() && !onWarp){
                Warp_Pos = (Vector2){static_cast<float>(wObj_Pos.x), static_cast<float>(wObj_Pos.y)};
                Warp_Dir = wObj_Dir;
                if (!Card.Active) {parseCSV();}
                Card.Active = true;
                onWarp = true;
            }
        }
    }
}

void TileMap::LoadNextLevel(const ldtk::Entity& warpObj){
    const auto& world = ldtk_project.getWorld();
    std::string test = warpObj.getField<std::string>("Going_To").value();
    const auto& loadLevel = world.getLevel(test);
    const auto& layer = loadLevel.getLayer("BG");
    const auto& tiles_vector = layer.allTiles();
    loadLevel_Pos = (Vector2) {static_cast<float>(loadLevel.position.x), static_cast<float>(loadLevel.position.y)};


    swapRender = LoadRenderTexture(loadLevel.size.x, loadLevel.size.y);
    nextLevel = loadLevel.name;

    // Draw all the tiles.
    BeginTextureMode(swapRender);
    ClearBackground(BLACK);
    for (const auto &tile : tiles_vector) {
        const auto& position = tile.getPosition();
        const auto& texture_rect = tile.getTextureRect();
        Vector2 dest = {
            static_cast<float>(position.x),
            static_cast<float>(position.y),
        };
        Rectangle src = {
            static_cast<float>(texture_rect.x),
            static_cast<float>(texture_rect.y),
            static_cast<float>(texture_rect.width) * (tile.flipX ? -1.0f : 1.0f),
            static_cast<float>(texture_rect.height) * (tile.flipY ? -1.0f : 1.0f)
        };
        DrawTextureRec(texture, src, dest, WHITE);
    }
    EndTextureMode();
    NextLevelLoaded = true;
    std::cout << "Loading Next Level..." << std::endl;
}

void TileMap::EnterNextlevel(Vector2 warpPos, Player& player_obj, int dir){
    bool enterLevel = false;
            if (onWarp){
                switch (dir){
                    case 0:
                        if (player_obj.GetPosition().x == warpPos.x + 8 && player_obj.GetPlayerDir() == 0){
                            SwapLevel_Dir = 180;
                            enterLevel = true;
                        }
                        break;
                    case 180:
                        if (player_obj.GetPosition().x == warpPos.x - 20 && player_obj.GetPlayerDir() == 180){
                            SwapLevel_Dir = 0;
                            enterLevel = true;
                        }
                        break;
                    case 270:
                        if (player_obj.GetPosition().y >= warpPos.y && player_obj.GetPlayerDir() == 270){
                            SwapLevel_Dir = 90;
                            enterLevel = true;
                        }
                        break;
                    case 90:
                        if (player_obj.GetPosition().y <= warpPos.y && player_obj.GetPlayerDir() == 90){
                            SwapLevel_Dir = 270;
                            enterLevel = true;
                        }
                        break;
            }
        }
        if (enterLevel){
            RenderTexture2D pastRenderer = renderer;
            Vector2 pastRenderPos = curLevel_Pos;
            std::string pastLevel = curLevel;
            curLevel = nextLevel;
            nextLevel = pastLevel;
            renderer = swapRender;
            swapRender =pastRenderer;
            curLevel_Pos = loadLevel_Pos;
            loadLevel_Pos = pastRenderPos;
            player_obj.SetLocation(curLevel);
            onWarp = false;
        }
}

void TileMap::unloadFarAwayLevel(Player& player_obj, std::vector<tileObj>& Tile_objs){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    bool TurnOff = false;
    Vector2 levelheight = (Vector2){static_cast<float>(level.size.x), static_cast<float>(level.size.y)};
    switch (SwapLevel_Dir){
        case 180:
            if (player_obj.GetPosition().x > loadLevel_Pos.x + 512){
                TurnOff = true;
            }
            break;
        case 0:
            if (player_obj.GetPosition().x < loadLevel_Pos.x - 512){
                TurnOff = true;
            }
            break;
        case 270:
            if (player_obj.GetPosition().y < (curLevel_Pos.y + levelheight.y) - 200){
                TurnOff = true;
            }
            break;
        case 90:
            if (player_obj.GetPosition().y > curLevel_Pos.y + 200){
                TurnOff = true;
            }
            break;
    }
    if (TurnOff){
        std::cout << "Unloading level..." << std::endl;
        NextLevelLoaded = false;
        DrawLoadedLevel = false;
        UnloadRenderTexture(swapRender);
        Tile_objs.erase(std::remove_if(Tile_objs.begin(), Tile_objs.end(), [&](const tileObj& obj) {
            return obj.GetLocation() != player_obj.GetLocation();
        }), Tile_objs.end());
    }
}

bool TileMap::IsCameraLockNear(Player& player_obj){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Utility");
    for (const ldtk::Entity& CameraL : objects.getEntitiesByName("LockCamera")) {
        auto CameraL_pos = CameraL.getWorldPosition();
        auto CameraL_Width = CameraL.getSize().x;
        auto CameraL_Height = CameraL.getSize().y;
        auto CameraL_Type = CameraL.getField<int>("Type").value();
        Rectangle CameraLRec = {static_cast<float>(CameraL_pos.x), static_cast<float>(CameraL_pos.y), static_cast<float>(CameraL_Width),static_cast<float>(CameraL_Height)};

        if (CheckCollisionRecs(player_obj.ColOffset(false),CameraLRec)){
            if (CameraL_Height > CameraL_Width){
                LockCameraAxis.x = 1;
                LockCameraAxis.y = 0;
            }
            if (CameraL_Height < CameraL_Width){
                LockCameraAxis.y = 1;
                LockCameraAxis.x = 0;
            }
            return true;
        }
    }
    return false;
}

Vector2 TileMap::InitLockDirection(Player& player_obj){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    int MinLevel_X = level.position.x - (level.size.x / 2);
    int MaxLevel_X = level.position.x + (level.size.x / 2);

    if (player_obj.GetPosition().x > MinLevel_X){
        return Vector2 {static_cast<float>(MinLevel_X + (level.size.x/2.0f) + (player_obj.GetPosition().x/2.0f) + 4),player_obj.GetPosition().y};
    }
    return Vector2 {0,0};
}

void TileMap::ShowLocationCard(Vector2 pos){
    if (Card.Active){
        Card.Position.x = 0 - 30;
        Card.Position.y = (0 - 30) + Card.CardYPos;
        if (Card.CardYPos < 32 && !Card.Hide){
            Card.CardYPos += 0.5f;
        }else{
            Card.Timer += GetFrameTime();
        }
        if (Card.Timer >= 3.0f){
            Card.Hide = true;
        }
    }
    if (Card.Hide){
        if (Card.CardYPos > -1){
            Card.CardYPos -= 0.5f;
        }else{
            Card.CardYPos = 0.0f;
            Card.Timer = 0.0f;
            Card.Hide = false;
            Card.Active = false;
        }
    }
}

void TileMap::DrawLocationCard(){
    if (Card.Active){
        DrawTextureRec(locationCard_Texture,Rectangle {static_cast<float>(30*(Card.Index_X)),static_cast<float>(30*(Card.Index_Y)),130,30},Card.Position,WHITE);
    }
}

const ldtk::Layer& TileMap::GetCOL() {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& collisions = level.getLayer("COL");

    return collisions;
}

void TileMap::PlayerInTallGrass(Player& player_obj){
    // Check if the tile value indicates a collision
    if (player_obj.IntGridValueAtPosition(GetCOL(),2,GetlevelOffset())) {
        // Collision detected, handle accordingly
        grass.Visible = true;
        grass.Active = true;
    } else{
        grass.Visible = false;
        grass.Position1 = {0,0};
        grass.Position2 = {0,0};
        grass.Position3 = {0,0};
    }
}

void TileMap::DrawGrass(Player& player_obj, std::vector<tileObj>& Tile_objs){
    if (grass.Visible){
        int inGridX = int(player_obj.ColOffset(true).x) % 16;
        int inGridY = int(player_obj.ColOffset(true).y) % 16;

        // Store player position
        Vector2 playerPosition = {player_obj.ColOffset(true).x, player_obj.ColOffset(true).y};

        for (auto& tile : Tile_objs) {
            int tileID = tile.GetID();
            // If the player is in a valid tile set the positions for the grass animations
            if (inGridX == 0 && inGridY == 0){
                if (!grass.set){
                    grass.startAnim = false;
                    grass.Position3 = grass.Position2;
                    grass.Position2 = grass.Position1;
                    grass.trailTimer.push_back(std::make_pair(0.0f, grass.TrailIndex));
                    grass.TrailIndex += 1;
                    grass.trailPos.push_back(grass.Position2);
                    grass.set = true;
                }
                grass.Position1 = playerPosition;
            } else {
                grass.set = false;
            }
            // Move the grass object to the player's position'
            if (tileID == 2){
                if (player_obj.IsPlayerMoving()) {
                    tile.ResetFrames(0);
                }
                if (player_obj.GetPlayerDir() == 90 && player_obj.GetPlayerFollower() != 0) {
                    tile.ResetGrass();
                } else if (player_obj.GetPlayerDir() == 270 && player_obj.IsPlayerMoving()) {
                    tile.ResetGrass();
                } else if (grass.set) {
                    tile.SetPosition(grass.Position1);
                }
            }
            // Move the other grass object behind the player's current position'
            if (tileID == 3){
                if (player_obj.IsPlayerMoving()) {
                    tile.ResetFrames(1);
                }
                if (grass.set) {
                    tile.SetOffet(-8);
                    tile.SetPosition(grass.Position2);
                }
            }
        }
    }
    // If the player is away from grass tiles
    if (grass.Active){
        GrassTrail();
        if (!grass.Visible || !player_obj.IsPlayerMoving()){
            grass.startAnim = true;
        }
        for (auto& tile : Tile_objs) {
            int tileID = tile.GetID();

            if (!tile.GetGrassAnim() && grass.startAnim){
                if (tileID == 2 || tileID == 3 || tileID == 4){
                    tile.AnimateGrass();
                }
            }

            if ((tileID == 4 || tileID == 3) && tile.GetGrassAnim()){
                if (player_obj.GetPlayerFollower() != 0 && !grass.Visible){
                    tile.ResetGrass();
                }
                if (player_obj.GetPlayerFollower() == 0){
                    tile.ResetGrass();
                }
            }

            if (player_obj.GetPlayerDir() == 270 && !grass.Visible){
                tile.ResetGrass();
            } else if (tileID == 2 && tile.GetGrassAnim() && !grass.Visible){
                grass.Active = false;
                tile.ResetGrass();
                grass.trailTimer.clear();
                grass.trailPos.clear();
                grass.TrailIndex = 0;
            }
        }
    }
}


void TileMap::GrassTrail(){
    for (auto& trail : grass.trailTimer ){
        if (trail.first < 1.0f){
            trail.first += GetFrameTime();
            DrawTextureRec(texture,{32,112,16,16},grass.trailPos[trail.second],WHITE);
            DrawTextureRec(tallGrass_Texture,{static_cast<float>(16 * int(4 * (trail.first))),0,16,16},grass.trailPos[trail.second],WHITE);
        }
    }
}

void TileMap::loadPlayer(Player& player_obj){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Objects");
    for (const ldtk::Entity& playerEnt : objects.getEntitiesByName("Player")) {
        auto player_pos = playerEnt.getWorldPosition();

        player_obj.setPosition(Vector2{(float(player_pos.x)-8),(float(player_pos.y)-16)});
        player_obj.SetLocation(curLevel);
    }
}

void TileMap::loadNPCs(Player& player_obj, std::vector<NPC>& NPC_objs) {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Objects");
    for (const ldtk::Entity& npcObj : objects.getEntitiesByName("NPC")) {
        auto npc_ID = npcObj.getField<int>("NPC_ID").value();

        // Check if an NPC with the same ID already exists in NPC_objs
        bool npcExists = false;
        for (const NPC& npc : NPC_objs) {
            if (npc.GetID() == npc_ID) {
                npcExists = true;
                break;
            }
        }

        // If the NPC with the same ID doesn't exist, add it to NPC_objs
        if (!npcExists) {
            auto npc_pos = npcObj.getWorldPosition();
            NPC_objs.push_back(NPC(npc_ID, curLevel, Vector2{(float)(npc_pos.x - 8), (float)(npc_pos.y - 16)}));
        }
    }
    if (player_obj.GetPlayerFollower() != 0){
        NPC_objs.erase(std::remove_if(NPC_objs.begin(), NPC_objs.end(), [&](const NPC& obj) {
            return obj.GetID() == player_obj.GetPlayerFollower();
        }), NPC_objs.end());
        NPC_objs.push_back(NPC(player_obj.GetPlayerFollower(), curLevel, player_obj.GetPosition()));
    }
}

void TileMap::loadTileObjs(const std::string lvl, std::vector<tileObj>& Tile_objs) {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(lvl);
    const auto& objects = level.getLayer("Objects");
    for (const ldtk::Entity& tObj : objects.getEntitiesByTag("TileObjects")) {
        auto tileObj_ID = 0;
        auto tileObj_Pos = tObj.getWorldPosition();
        auto tileObj_Rect = tObj.getTextureRect();
        Tile_objs.push_back(tileObj(tileObj_ID, lvl, Vector2{(float)(tileObj_Pos.x), (float)(tileObj_Pos.y)}, tileObj_Rect, texture));
    }
    for (int i = 2 ; i < 5; i++){
        Tile_objs.push_back(tileObj(i, lvl, Vector2{(float)(0), (float)(0)}, {0,0,16,16}, tallGrass_Texture));
    }
}

void TileMap::Unload() {
    UnloadRenderTexture(renderer);
    UnloadRenderTexture(swapRender);
    UnloadTexture(texture);
}

// This is for the location card xd
void TileMap::parseCSV(){
    rapidcsv::Document doc("assets/CSV/OW_LEVEL_DEF.csv");
    const auto& levels = doc.GetColumn<std::string>("Level");
    const auto& LevelX = doc.GetColumn<int>("Index_X");
    const auto& LevelY = doc.GetColumn<int>("Index_Y");


    // Iterate through rows and compare IDs
    for (size_t i = 0; i < levels.size(); ++i) {
        if (levels[i] == nextLevel) {
            Card.Index_X = LevelX[i];
            Card.Index_Y = LevelY[i];
        }
    }

}

Vector2 TileMap::GetSwapLevel(){
    return loadLevel_Pos;
}

Vector2 TileMap::GetCurLevel(){
    return curLevel_Pos;
}

Vector2 TileMap::GetlevelOffset(){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    int LvlOffsetX= level.position.x / 16;
    int LvlOffsetY= level.position.y / 16;
    curLevel_Offset = (Vector2){static_cast<float>(LvlOffsetX),static_cast<float>(LvlOffsetY)};


    return curLevel_Offset;
}

bool TileMap::IsNextLevelLoaded(){
    return NextLevelLoaded;
}

bool TileMap::StandingOnWarp(){
    return onWarp;
}

bool TileMap::GetDrawNextLevel(){
    return DrawLoadedLevel;
}

void TileMap::SwapLevels(Player& player_obj){
    EnterNextlevel(Warp_Pos, player_obj, Warp_Dir);
}

void TileMap::SetDrawNextLevel(){
    DrawLoadedLevel = true;
}

std::string TileMap::GetCurLevelName(){
    return curLevel;
}

std::string TileMap::GetSwapLevelName(){
    return nextLevel;
}
