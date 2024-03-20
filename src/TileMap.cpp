#include "TileMap.h"

TileMap::TileMap() {
    currentFrameIndex = -1;
    tileAnimReverse = false;
    animTimer = 0;
    animFrame = true;
    TILE_ANIM_TIME = 50;
}

TileMap::~TileMap() {
}


void TileMap::loadLDtkMap(const std::string& filePath) {
    try {
        ldtk_project.loadFromFile(filePath);
    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Configure the tilemap
void TileMap::initialize(const std::string& Lvl) {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel(Lvl);
    const auto& layer = level.getLayer("BG");
    const auto& tiles_vector = layer.allTiles();

    // Load texture and renderer
    texture = LoadTexture(("assets/" + layer.getTileset().path).c_str());
    renderer = LoadRenderTexture(level.size.x, level.size.y);

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
void TileMap::draw() {
    Rectangle src = {
        0,
        0,
        static_cast<float>(renderer.texture.width),
        -static_cast<float>(renderer.texture.height)
    };
    Rectangle dest = {
        0,
        0,
        static_cast<float>(renderer.texture.width) * 1,
        static_cast<float>(renderer.texture.height) * 1
    };
    DrawTexturePro(renderer.texture, src, dest, {0}, .0f, WHITE);
}

// Animate Tiles
void TileMap::update() {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel("Level_0");
    const auto& layer = level.getLayer("BG");
    const auto& tiles_vector = layer.allTiles();
    // If the timer reaches the set time, draw the next tile in the sequence
    if (animTimer != TILE_ANIM_TIME){
        animTimer += 1;
        animFrame = false;
    }else{
        animTimer = 0;
        animFrame = true;
    }

    if (animFrame){
        // Update the current frame index based on the tile animation direction.
        currentFrameIndex += tileAnimReverse ? -1 : 1;
        // Check if the current frame index is at the beginning or end of the animation sequence.
        // If so, toggle the tile animation direction by reversing the value of tileAnimReverse.
        if (currentFrameIndex == -1 || currentFrameIndex + 1 == 2) {
            tileAnimReverse = !tileAnimReverse;
        }
        ClearBackground(BLACK);
        BeginTextureMode(renderer);

        for (const auto &tile : tiles_vector) {
            const auto& position = tile.getPosition();
            const auto& texture_rect = tile.getTextureRect();
            int TILE_X = texture_rect.x;
            int TILE_Y = texture_rect.y;
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
                    TILE_X = layer.getTileset().getTileTexturePos(animTiles[currentFrameIndex + 1]).x;
                    TILE_Y = layer.getTileset().getTileTexturePos(animTiles[currentFrameIndex + 1]).y;
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
        EndTextureMode();
    }
}

const ldtk::Layer& TileMap::GetCOL() {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel("Level_0");
    const auto& collisions = level.getLayer("COL");

    return collisions;
}

void TileMap::loadPlayer(Player& player_obj){
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel("Level_0");
    const auto& objects = level.getLayer("Objects");
    for (const ldtk::Entity& playerEnt : objects.getEntitiesByName("Player")) {
        auto player_pos = playerEnt.getPosition();

        player_obj.setPosition(Vector2{(float(player_pos.x)-8),(float(player_pos.y)-16)});
    }
}

void TileMap::loadNPCs(std::vector<NPC>& NPC_objs) {
    const auto& world = ldtk_project.getWorld();
    const auto& level = world.getLevel("Level_0");
    const auto& objects = level.getLayer("Objects");
    for (const ldtk::Entity& npcObj : objects.getEntitiesByName("NPC")) {
        auto npc_ID = npcObj.getField<int>("NPC_ID").value();
        auto npc_pos = npcObj.getPosition();
        NPC_objs.push_back(NPC(npc_ID, Vector2{(float)(npc_pos.x - 8), (float)(npc_pos.y - 16)}));
    }
}

void TileMap::Unload() {
    UnloadRenderTexture(renderer);
    UnloadTexture(texture);
}
