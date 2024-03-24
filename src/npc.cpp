#include "npc.h"
#include "npc_config.h"
#include <iostream>
#include <math.h>
#include <raylib.h>
#include <string>
#include "rapidcsv.h"

NPC::NPC(int npcID, Vector2 NPCPos) {
    ID = npcID;
    Stage = 0;
    lvl = 1;
    NPCTexture = LoadTexture(NPCWalk::GetTexturePath(ID, Stage).c_str());
    position = NPCPos;
    //Initialize variables
    currentAnimation = ANIM_DOWN;
    currentFrame = 0;
    direction = 1;
    m_dir = 270;
    frameRate = 0.125f;
    frameTimer = 0.0f;
    move = false;
    animating = false;
    step_timer = 0;
    inGrowthPhase = false;
    following_Player = false;
    // Initialize collision mask
    collisionMask.width = COLLISION_MASK_WIDTH;
    collisionMask.height = COLLISION_MASK_HEIGHT;
    collisionMask.x = position.x - (COLLISION_MASK_WIDTH / 2.0f);
    collisionMask.y = position.y - (COLLISION_MASK_HEIGHT / 2.0f);

    //Initialize animation frames
    FRAME_X = 32;
    FRAME_Y = 32;

    for (int i = 0; i < 3; i++) {
        framesUp[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(0 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        framesDown[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        framesRight[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(2 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        framesLeft[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(3 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
    }

}
NPC::~NPC() {
    //clean
}

void NPC::npcMove(Vector2 past_direction, Vector2 past_position, int speed, bool moving, bool anim, int step){
    animating = anim;
    if (moving){
        move = true;
    }
    
    if (move){
        if (step < 16 / speed){
        position.x += past_direction.x * speed;
        position.y += past_direction.y * speed;
        }
        else{
            position = past_position;
            move = false;
        }
    // Set animation based on movement direction
        if (past_direction.x < 0){
            m_dir = 180;
            currentAnimation = ANIM_LEFT;
        } else if (past_direction.x > 0){
            m_dir = 0;
            currentAnimation = ANIM_RIGHT;
        } else if (past_direction.y < 0){
            m_dir = 90;
            currentAnimation = ANIM_UP;
        } else if (past_direction.y > 0){
            m_dir = 270;
            currentAnimation = ANIM_DOWN;
        }
    }
    collisionMask.x = position.x - (COLLISION_MASK_WIDTH / 2.0f);
    collisionMask.y = position.y - (COLLISION_MASK_HEIGHT / 2.0f);
}

void NPC::Update() {
    // Animate growth if in growth phase
    if (inGrowthPhase) {
        frameRate = 0.250f;
        frameTimer += GetFrameTime();
        if (frameTimer >= frameRate) {
            currentFrame++;
            if (currentFrame >= framesGrowth.size()) {
                // End of growth animation
                updateTexture("walk");
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
                inGrowthPhase = false; // Switch back to regular walking animation
            }
            frameTimer = 0.0f;
        }
    } else {
        // Animate movement for regular walking animation
        if (move || animating) {
            if (step_timer == 0){
                animating = false;
            }
            frameRate = 0.125f;
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
        } else {
            currentFrame = 1;
        }
    }
}


void NPC::Draw() {
    //Calculate the position where the texture is drawn
    Vector2 drawPosition = {
        floor(((position.x + COLLISION_MASK_WIDTH) - float(FRAME_X / 2.0f))),
        ((position.y + (COLLISION_MASK_HEIGHT*2)) - (FRAME_Y))
    };
    if (NPC_DEF[8] == "Centered"){
        DrawTexture(ShadowCentered, (collisionMask.x + COLLISION_MASK_WIDTH), (collisionMask.y + (COLLISION_MASK_HEIGHT * 2)),WHITE);
    } else if (NPC_DEF[8] == "Off_Center"){
        DrawTexture(ShadowOffCenter, (collisionMask.x + COLLISION_MASK_WIDTH), (collisionMask.y + (COLLISION_MASK_HEIGHT * 2)) + 2,WHITE);
    }
    switch (currentAnimation) {
        case ANIM_UP:
            DrawTextureRec(NPCTexture, framesUp[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_DOWN:
            DrawTextureRec(NPCTexture, framesDown[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_RIGHT:
            DrawTextureRec(NPCTexture, framesRight[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_LEFT:
            DrawTextureRec(NPCTexture, framesLeft[currentFrame], drawPosition, WHITE);
            break;
        case ANIM_GROWTH:
            DrawTextureRec(NPCTexture, framesGrowth[currentFrame], drawPosition, WHITE);
            break;
    }
    //DrawText(TextFormat("%02i debug",int(step_timer)), position.x + 32, position.y + 32, 10, WHITE);
}

void NPC::updateAnimationFrameDimensions(const std::string& animation) {
    int textureWidth = NPCTexture.width;
    int textureHeight = NPCTexture.height;

    // Default frame dimensions
    int frameCountX = 3;
    int frameCountY = 4;

    if (animation == "walk") {
        // Customize frame dimensions for "walk" animation
        frameCountX = 3;
        frameCountY = 4;
        
        // Calculate frame size
        FRAME_X = textureWidth / frameCountX;
        FRAME_Y = textureHeight / frameCountY;

        // Update animation frames
        for (int i = 0; i < frameCountX; i++) {
            framesUp[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(0 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
            framesDown[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
            framesRight[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(2 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
            framesLeft[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(3 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        }

    } else if (animation == "growth"){
        // Customize frame dimensions for "grow" animation
        int GROW_FRAMES = stoi(NPC_DEF[Stage]);
        framesGrowth.resize(GROW_FRAMES);

        // Calculate frame size
        FRAME_X = textureWidth / 1;
        FRAME_Y = textureHeight / GROW_FRAMES;

        // Update animation frames
        for (int i = 0; i < GROW_FRAMES; i++) {
            framesGrowth[i] = { static_cast<float>(0 * FRAME_X), static_cast<float>(i * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
        }
        currentAnimation = ANIM_GROWTH;
        inGrowthPhase = true;
        DrawText(TextFormat("%02i cock",int(FRAME_X)), 64, 64, 10, WHITE);
        DrawText(TextFormat("%02i cock",int(FRAME_Y)), 64, 80, 10, WHITE);
    }
}


void NPC::updateTexture(const std::string& animation) {
    if (animation=="growth"){
        NPCTexture = LoadTexture(NPCGrow::GetTexturePath(ID, Stage).c_str());
    } else if (animation=="walk")
    {
        NPCTexture = LoadTexture(NPCWalk::GetTexturePath(ID, Stage).c_str());
    }
    // Load texture based on the current event
}

void NPC::setEvent(int newEvent) {
    // Set the event to the new value
    Stage = newEvent;
}

void NPC::lookAtPlayer(int lookingAt){

    switch (lookingAt)
    {
    case 270:
        m_dir = 90;
        currentAnimation = ANIM_UP;
        break;
    case 90:
        m_dir = 270;
        currentAnimation = ANIM_DOWN;
        break;
    case 180:
        m_dir = 0;
        currentAnimation = ANIM_RIGHT;
        break;
    case 0:
        m_dir = 180;
        currentAnimation = ANIM_LEFT;
        break;
    }

}

std::string NPC::GetCombinedValues(int Event){
    std::string Value = (std::to_string(ID) + "_" + std::to_string(Stage) + "_" + std::to_string(lvl) + "_" + "Anywhere" + "_" + std::to_string(Event));

    return Value;
};

void NPC::parseCSV(const std::string& filename) {
    rapidcsv::Document doc(filename);
    const auto& ids = doc.GetColumn<int>("ID");

    if(filename == "assets/Dataset.csv"){
        // Clear previous data for this NPC
        m_data.clear();
        // Iterate through rows and compare IDs
        for (size_t i = 0; i < ids.size(); ++i) {
            if (ids[i] == ID) {
                // Store entire row in NPC-specific vector
                m_data.push_back(doc.GetRow<std::string>(i));
            }
        }
    }else if(filename == "assets/NPC_OW_DEF.csv"){
        for (size_t i = 0; i < ids.size(); ++i) {
            if (ids[i] == ID) {
                // Store entire row in NPC-specific vector
                NPC_DEF = doc.GetRow<std::string>(i);
                break; // Stop after finding the first matching ID
            }
        }
    }
}

void NPC::GetShadow(Texture2D load1, Texture2D load2){
    ShadowCentered = load1;
    ShadowOffCenter = load2;
}
