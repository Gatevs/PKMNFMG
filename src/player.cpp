#include "player.h"
#include "raylib.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "rapidcsv.h"

Player::Player() {
    Stage = 0;
    Name = "Player";
    playerTexture = LoadTexture("assets/MISC/dummy.png");
    Bump = LoadSound("assets/SFX/BUMP.ogg");
    // Initialize member variables
    position = { 32, 32 };
    currentAnimation = ANIM_UP;
    currentFrame = 0;
    direction = 1;
    m_dir = 0;
    step_timer = 0;
    animating = false;
    move = false;
    player_speed = 1;
    frameRate = 0.125f;
    frameTimer = 0.0f;
    RunFrames = 0;
    camera = { 0 };
    camera.target = position;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    canMove = true;
    FollowerID = 0;
    colID = 0;
    lastNPCId = 0;
    keyPressTimer = 0.0f;
    input_direction = {0,0};
    past_dir = {0,0};
    past_pos = {0,0};
    collisionMask.width = COLLISION_MASK_WIDTH;
    collisionMask.height = COLLISION_MASK_HEIGHT;
    LoadUI_Element = NONE;
    lastFrame = 0;
    wantedStage = -1;
    inGrowthPhase = false;

    // Initialize animation frames
    FRAME_X = 32;
    FRAME_Y = 32;

    for (int i = 0; i < 3; i++) {
        framesUp[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(0 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        framesDown[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        framesRight[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(2 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        framesLeft[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(3 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
    }
}

Player::~Player() {
    // Clean up any resources if needed
}

// Listen to player Input
void Player::HandleInput(std::vector<NPC>& npcs){
    float MOVEMENT_DELAY = 0.150f;
    // Increase the timer if any directional key is pressed
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) {
        keyPressTimer += GetFrameTime();
    } else {
        // Reset the timer if no directional key is pressed
        keyPressTimer = 0.0f;
        if (!inGrowthPhase){
            animating = false;
        }
    }
    if (step_timer >= 16/player_speed) {
        past_dir = input_direction;
        step_timer = 0;
        move = 0;
    }

    // Check if the timer exceeds the delay threshold
    if (keyPressTimer >= MOVEMENT_DELAY) {

        if (!move) {
            if (IsKeyDown(KEY_UP)) {
                currentAnimation = ANIM_UP;
                animating = true;
                move = true;
                m_dir = 90;
                input_direction.y = -1;
                input_direction.x = 0;
                past_pos = position;
            } else if (IsKeyDown(KEY_DOWN)) {
                currentAnimation = ANIM_DOWN;
                animating = true;
                move = true;
                m_dir = 270;
                input_direction.y = 1;
                input_direction.x = 0;
                past_pos = position;
            } else if (IsKeyDown(KEY_RIGHT)) {
                currentAnimation = ANIM_RIGHT;
                animating = true;
                move = true;
                m_dir = 0;
                input_direction.x = 1;
                input_direction.y = 0;
                past_pos = position;
            } else if (IsKeyDown(KEY_LEFT)) {
                currentAnimation = ANIM_LEFT;
                animating = true;
                move = true;
                m_dir = 180;
                input_direction.x = -1;
                input_direction.y = 0;
                past_pos = position;
            }
            player_speed = IsKeyDown(KEY_X) ? 2 : 1;
            RunFrames = (player_speed == 2) ? 4: 0 ;

        }

    } else if (step_timer == 0) {
        // Player is not allowed to move yet, only update animation direction
        if (IsKeyDown(KEY_UP)) {
            currentAnimation = ANIM_UP;
            m_dir = 90;
            animating = true;
        } else if (IsKeyDown(KEY_DOWN)) {
            currentAnimation = ANIM_DOWN;
            m_dir = 270;
            animating = true;
        } else if (IsKeyDown(KEY_RIGHT)) {
            currentAnimation = ANIM_RIGHT;
            m_dir = 0;
            animating = true;
        } else if (IsKeyDown(KEY_LEFT)) {
            currentAnimation = ANIM_LEFT;
            m_dir = 180;
            animating = true;
        }
    }

    if (IsKeyPressed(KEY_A)){
        if (!IsPlayerMoving()){
            LoadUI_Element = PAUSE;
        }
    }
    if (IsKeyPressed(KEY_S)){
        if (!IsPlayerMoving()){
            LoadUI_Element = ACTION;
        }
    }
    if (IsKeyPressed(KEY_Z)){
        if (!IsPlayerMoving()){
            LoadUI_Element = DIALOGUE;
        }
    }
}

void Player::Growth(){
        //  Compare the NPC's current stage with the wanted stage, and shrink or grow accordingly
    if (Stage < wantedStage && !inGrowthPhase && wantedStage != -1){
        Stage += 1;
        UpdateTexture("growth");
        updateAnimationFrameDimensions("growth");
        lastFrame = framesGrowth.size();
        direction = 1;
    }
    if (Stage > wantedStage && !inGrowthPhase && wantedStage != -1 ){
        UpdateTexture("growth");
        updateAnimationFrameDimensions("shrink");
        lastFrame = 0;
        direction = -1;
    }
    // Animate growth if in growth phase
    if (inGrowthPhase) {
        frameRate = 0.250f;
        frameTimer += GetFrameTime();
        if (frameTimer >= frameRate) {
            currentFrame += direction;
            if (currentFrame == lastFrame) {
                // End of growth animation
                if (Stage > wantedStage){
                    Stage -= 1;
                }
                UpdateTexture("walk");
                updateAnimationFrameDimensions("walk");
                switch (m_dir)
                {
                case 90:
                currentAnimation = ANIM_UP;
                    break;
                case 180:
                currentAnimation = ANIM_LEFT;
                    break;
                case 270:
                currentAnimation = ANIM_DOWN;
                    break;
                case 0:
                currentAnimation = ANIM_RIGHT;
                    break;
                }
                currentFrame = 1;
                frameRate = 0.125f;
                inGrowthPhase = false; // Switch back to regular walking animation
            }
            frameTimer = 0.0f;
        }
    }
}

void Player::UpdateAnim(){
    int textureWidth = playerTexture.width;
    int textureHeight = playerTexture.height;
     // Customize frame dimensions for "walk" animation
    int frameCountX = 3;
    int frameCountY = 8;

        // Calculate frame size
        FRAME_X = textureWidth / frameCountX;
        FRAME_Y = textureHeight / frameCountY;

        // Update animation frames
        for (int i = 0; i < frameCountX; i++) {
            framesUp[i] = { static_cast<float>(i * FRAME_X), static_cast<float>((0 + RunFrames) * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
            framesDown[i] = { static_cast<float>(i * FRAME_X), static_cast<float>((1 + RunFrames)* FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
            framesRight[i] = { static_cast<float>(i * FRAME_X), static_cast<float>((2 + RunFrames) * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
            framesLeft[i] = { static_cast<float>(i * FRAME_X), static_cast<float>((3 + RunFrames) * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        }
}

// Animate movement
void Player::Update() {
    if (Gender == "PlayerF"){
        Growth();
    }
    if (move || animating) {
        frameTimer += GetFrameTime();
        if (frameTimer >= frameRate) {
            currentFrame += direction;
            if (currentFrame > 2) {
                currentFrame = 1;
                direction = -1;
            }
            if (currentFrame < 0) {
                currentFrame = 1;
                direction = 1;
            }
            frameTimer = 0.0f;
        }
    } else if (!inGrowthPhase){
        currentFrame = 1;
    }
    if (move){
        frameRate = 0.125f;
    }
}

// If player has a follower make them move along player
void Player::npcMoving(std::vector<NPC>& npcs){
    for (auto& npc : npcs) {
        if (npc.GetID() == FollowerID) {
            npc.npcMove(past_dir,past_pos,player_speed,move,animating, step_timer);
            break;
        }
    }
}

// Update player position based on movement logic
void Player::UpdatePositionAndCamera() {
    Rectangle colOffset = ColOffset(false); // Calculate once

    if (move) {
        if (step_timer < 16/player_speed) {
            step_timer += 1;

            position.x = colOffset.x + (COLLISION_MASK_WIDTH / 2.0f);
            position.y = colOffset.y + (COLLISION_MASK_HEIGHT / 2.0f);
        }

    }
    // Update collision mask position to match player position
    collisionMask.x = position.x - (COLLISION_MASK_WIDTH / 2.0f);
    collisionMask.y = position.y - (COLLISION_MASK_HEIGHT / 2.0f);
}

bool Player::IntGridValueAtPosition(const ldtk::Layer& collisions, int value, Vector2 IntGridOffset){
    // Calculate the position in front of the player based on direction
    int tile_Col = 0;
    // Get the integer grid value at the specified position
    auto tileValue1 = collisions.getIntGridVal((ColOffset(true).x / 16) - IntGridOffset.x, (ColOffset(true).y / 16) - IntGridOffset.y);
    auto tileValue2 = collisions.getIntGridVal(((ColOffset(true).x + 16 )/ 16) - IntGridOffset.x, ((ColOffset(true).y) / 16) - IntGridOffset.y);
    auto tileValue3 = collisions.getIntGridVal((ColOffset(true).x/ 16) - IntGridOffset.x, ((ColOffset(true).y + 16 ) / 16) - IntGridOffset.y);
    if (m_dir == 90 or m_dir == 180){
        tile_Col = tileValue1.value;
    } else if (m_dir == 0){
        tile_Col = tileValue2.value;
    } else{
        tile_Col = tileValue3.value;
    }
    if (tile_Col == value){
        return true;
    } else{
        return false;
    }
}

void Player::checkCollisions(const ldtk::Layer& collisions,  const std::vector<NPC>& npcs, Vector2 IntGridOffset) {
    // Check if the tile value indicates a collision
    if (IntGridValueAtPosition(collisions,1,IntGridOffset)) {
        // Collision detected, handle accordingly
        if (step_timer != 0){
            past_dir = input_direction;
        }
        BumpAgainstObject();
    }
    for (const auto& npc : npcs) {
        if (CheckCollisionRecs(ColOffset(false), npc.GetCollisionMask())) {
            colID = npc.GetID();
            // Collision detected, player can't move
            if (colID != FollowerID){
                BumpAgainstObject();
            }
        }
    }
}

void Player::BumpAgainstObject(){
    if (!IsSoundPlaying(Bump)){
        PlaySound(Bump);
    }
    frameRate = 0.250f;
    step_timer = 0;
    move = false;
}

Rectangle Player::ColOffset (bool Tile) const{
    int newPlayerX = position.x;
    int newPlayerY = position.y;
    if (Tile){
        newPlayerX = position.x + 16;
        newPlayerY = position.y + 24;
    }

    // Calculate the new position based on player's direction
    switch (m_dir) {
        case 270:
            if (Tile) {
                newPlayerY += player_speed - 1; // Adjusted for checkCollisions
            } else {
                newPlayerY += player_speed;
            }
            break;
        case 90:
            if (Tile) {
                newPlayerY -= player_speed;
            } else {
                newPlayerY -= player_speed;
            }
            break;
        case 0:
            if (Tile) {
                newPlayerX += player_speed - 1; // Adjusted for checkCollisions
            } else {
                newPlayerX += player_speed;
            }
            break;
        case 180:
            if (Tile) {
                newPlayerX -= player_speed;
            } else {
                newPlayerX -= player_speed;
            }
            break;
    }

    // Create and return the collision mask
    return {
        static_cast<float>(newPlayerX - (COLLISION_MASK_WIDTH / 2.0f)),
        static_cast<float>(newPlayerY - (COLLISION_MASK_HEIGHT / 2.0f)),
        static_cast<float>(COLLISION_MASK_WIDTH),
        static_cast<float>(COLLISION_MASK_HEIGHT)
    };
}

// Returns the ID of the NPC that the player is facing
int Player::CheckForNPCInFront(const std::vector<NPC>& npcs) const {
    // Check if there is an NPC at the calculated position
    for (const NPC& npc : npcs) {
        if (CheckCollisionRecs(ColOffset(false), npc.GetCollisionMask())) {
            return npc.GetID(); // Return the ID of the NPC
        }
    }
    return -1; // Return -1 if no NPC found
}


void Player::setPosition(Vector2 newPos){
    position = newPos;
}

// Draw the player's texture into the world
void Player::Draw() {
        //Calculate the position where the texture is drawn
    Vector2 drawPosition = {
        static_cast<float>(floor(((position.x + COLLISION_MASK_WIDTH) - float(FRAME_X / 2.0f)))),
        ((position.y + (COLLISION_MASK_HEIGHT*2)) - (FRAME_Y))
    };
    DrawTexture(shadowTexture, (collisionMask.x + COLLISION_MASK_WIDTH), (collisionMask.y + (COLLISION_MASK_HEIGHT * 2)),WHITE);
    switch (currentAnimation) {
        case ANIM_UP:
            DrawTextureRec(playerTexture, framesUp[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_DOWN:
            DrawTextureRec(playerTexture, framesDown[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_RIGHT:
            DrawTextureRec(playerTexture, framesRight[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_LEFT:
            DrawTextureRec(playerTexture, framesLeft[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_GROWTH:
            DrawTextureRec(playerTexture, framesGrowth[currentFrame], drawPosition, WHITE);
            break;
    }

    //DrawText(TextFormat("%02i debug",int(past_dir.y)), position.x + 32, position.y + 16, 10, WHITE);
}

Vector2 Player::GetPosition() const {
    return position;
}

int Player::GetPlayerSpeed() const {
    return player_speed;
}

bool Player::IsPlayerMoving() const {
    return move;
}

bool Player::IsPlayerAnimating() const {
    return animating;
}

int Player::GetPlayerFollower() const {
    return FollowerID;
}

int Player::GetPlayerDir() const {
    return m_dir;
}

void Player::SetShadow(Texture2D load){
    shadowTexture = load;
}

void Player::SetFollowerID(int follow){
    FollowerID = follow;
}

int Player::InvokeUIElement(){
    return LoadUI_Element;
}

void Player::StopUI_Element(){
    LoadUI_Element = NONE;
}

void Player::SetFollowDir(Vector2 Dir){
    past_dir = Dir;
}

void Player::SetLocation(std::string loc){
    location = loc;
}

void Player::SetPlayerGender(std::string playerGender){
    Gender = playerGender;
    UnloadTexture(playerTexture);
    std::string TexturePath = "assets/OW_SPRITE/" + playerGender + "_" + std::to_string(Stage) + ".png";
    playerTexture = LoadTexture(TexturePath.c_str());
}

std::string Player::GetPlayerGender() const {
    return Gender;
}

std::string Player::GetPlayerName() const {
    return Name;
}

int Player::GetStage() const {
    return Stage;
}

void Player::setNextStage(int NextStage){
    wantedStage = NextStage;
}

void Player::UpdateTexture(const std::string& animation){
    if (animation == "growth"){
        std::string TexturePath = "assets/OW_SPRITE/" + Gender + "_G" + std::to_string(Stage) + ".png";
        UnloadTexture(playerTexture);
        playerTexture = LoadTexture(TexturePath.c_str());
    } else if(animation == "walk"){
        std::string TexturePath = "assets/OW_SPRITE/" + Gender + "_" + std::to_string(Stage) + ".png";
        UnloadTexture(playerTexture);
        playerTexture = LoadTexture(TexturePath.c_str());
    }else if(animation == "idle"){
        std::string TexturePath = "assets/OW_SPRITE/" + Gender + "_I" + std::to_string(Stage) + ".png";
        playerTexture = LoadTexture(TexturePath.c_str());
    }

}

void Player::parseCSV(const std::string& filename) {
    rapidcsv::Document doc(filename);
    const auto& ids = doc.GetColumn<int>("ID");

    if(filename == "assets/CSV/NPC_OW_DEF.csv"){
        for (size_t i = 0; i < ids.size(); ++i) {
            if (ids[i] == 1) {
                // Store entire row in NPC-specific vector
                PLAYER_DEF = doc.GetRow<std::string>(i);
                break; // Stop after finding the first matching ID
            }
        }
    }
}

void Player::updateAnimationFrameDimensions(const std::string& animation) {
    int textureWidth = (animation == "idle") ? playerTextureIdle.width : playerTexture.width;
    int textureHeight = (animation == "idle") ? playerTextureIdle.height : playerTexture.height;

    if (animation == "walk" || animation == "idle") {
        UpdateAnim();
    } else if (animation == "growth" || animation == "shrink"){
        // Customize frame dimensions for "grow" animation
        int GROW_FRAMES = stoi(PLAYER_DEF[Stage]);
        framesGrowth.resize(GROW_FRAMES);

        // Calculate frame size
        FRAME_X = textureWidth / 1;
        FRAME_Y = textureHeight / GROW_FRAMES;

        // Update animation frames
        for (int i = 0; i < GROW_FRAMES; i++) {
            framesGrowth[i] = { static_cast<float>(0 * FRAME_X), static_cast<float>(i * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        }
        currentAnimation = ANIM_GROWTH;

        if (!inGrowthPhase and animation == "shrink"){
            currentFrame = (framesGrowth.size() - 1);
        } else if (!inGrowthPhase and animation == "growth"){
            currentFrame = 1;
        }
        inGrowthPhase = true;
    }
}
