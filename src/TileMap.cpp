#include "TileMap.h"
#include "TileObjects.h"
#include "rapidcsv.h"
#include "raylib.h"
#include <string>

TileMap::TileMap() {
    locationCard_Texture = LoadTexture("assets/LocationCard.png");
    DrawLoadedLevel = false;
    NextLevelLoaded = false;
    onWarp = false;
    TILE_ANIM_TIME = 50;
    curLevel = "Swolie_Town";
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
    curLevel = Lvl;
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(Lvl);
    const auto& layer = level.getLayer("BG");
    const auto& tiles_vector = layer.allTiles();

    // Load texture and renderer
    texture = LoadTexture(("assets/" + layer.getTileset().path).c_str());
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
void TileMap::update(const std::string lvl, unrelated& animationState, Player& player_obj) {
    RenderTexture2D drawer;
    if (Card.Active){
        ShowLocationCard(player_obj);
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
        Vector2 StartPos = (Vector2){player_obj.GetPosition().x - 112, player_obj.GetPosition().y - 96};
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

void TileMap::IsWarpClose(Player& player_obj){
    const int TILE_SIZE = 16;
    int dirvalue = 0;

    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Objects");

    for (const ldtk::Entity& wObj : objects.getEntitiesByTag("Warps")) {
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
                ShowLocationCard(player_obj);
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
        Rectangle CameraLRec = {static_cast<float>(CameraL_pos.x), static_cast<float>(CameraL_pos.y), static_cast<float>(CameraL_Width),static_cast<float>(CameraL_Height)};

        if (CheckCollisionRecs(player_obj.ColOffset(false),CameraLRec)){
            if (CameraL_Width < CameraL_Height){
                LockCameraAxis = 1;
            } else if (CameraL_Width > CameraL_Height){
                LockCameraAxis = 2;
            }
            return true;
        }
    }
    return false;
}

void TileMap::ShowLocationCard(Player& player_obj){
    if (!Card.Active){
        Card.Position = {player_obj.GetPosition().x - 110, player_obj.GetPosition().y - 110};
        Card.Active = true;
    }
    if (Card.Active){
        Card.Position.x = player_obj.GetPosition().x - 110;
        Card.Position.y = (player_obj.GetPosition().y - 110) + Card.CardYPos;
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

void TileMap::loadPlayer(Player& player_obj){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Objects");
    for (const ldtk::Entity& playerEnt : objects.getEntitiesByName("Player")) {
        auto player_pos = playerEnt.getPosition();

        player_obj.setPosition(Vector2{(float(player_pos.x)-8),(float(player_pos.y)-16)});
        player_obj.SetLocation(curLevel);
    }
}

void TileMap::loadNPCs(std::vector<NPC>& NPC_objs) {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(curLevel);
    const auto& objects = level.getLayer("Objects");
    for (const ldtk::Entity& npcObj : objects.getEntitiesByName("NPC")) {
        auto npc_ID = npcObj.getField<int>("NPC_ID").value();
        auto npc_pos = npcObj.getWorldPosition();
        NPC_objs.push_back(NPC(npc_ID, curLevel, Vector2{(float)(npc_pos.x - 8), (float)(npc_pos.y - 16)}));
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
}

void TileMap::Unload() {
    UnloadRenderTexture(renderer);
    UnloadRenderTexture(swapRender);
    UnloadTexture(texture);
}

void TileMap::parseCSV(){
    rapidcsv::Document doc("assets/OW_LEVEL_DEF.csv");
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
