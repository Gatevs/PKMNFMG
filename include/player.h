#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <LDtkLoader/Project.hpp>
#include "GameObject.h"
#include "npc.h"
#include "ActionHandler.h"
#include <vector>
#include <algorithm>

class Player :public GameObject {
public:
    Player();
    ~Player();

// Core functionality
    void Update();
    void Draw() override;

    // Input handling
    void HandleInput(std::vector<NPC>& npcs, ActionHandler& MenuObj);
    void InvokeUIElement();

    // Movement and position
    void npcMoving(std::vector<NPC>& npcs);
    void UpdatePositionAndCamera();
    void UpdateAnim ();
    void setPosition(Vector2 newPos);

    // Collision detection
    void checkCollisions(const ldtk::Layer& collisions,  const std::vector<NPC>& npcs);
    Rectangle ColOffset(bool Tile) const;

    // Accessors
    void SetFollowerID(int follow);
    float GetYPosition() const override { return position.y; }
    Vector2 GetPosition() const;
    int GetPlayerDir() const;
    int GetPlayerSpeed() const;
    int GetPlayerFollower() const;
    bool IsPlayerMoving() const;
    bool IsPlayerAnimating() const;
    int CheckForNPCInFront(const std::vector<NPC>& npcs) const;
    void GetShadow(Texture2D load);

private:
    // Graphics
    Texture2D playerTexture;
    Rectangle framesUp[3];
    Rectangle framesDown[3];
    Rectangle framesRight[3];
    Rectangle framesLeft[3];
    Texture2D shadowTexture;

    // Animation
    enum Animation {
        ANIM_UP,
        ANIM_DOWN,
        ANIM_RIGHT,
        ANIM_LEFT
    };
    int FRAME_X;
    int FRAME_Y;
    int currentAnimation;
    int currentFrame;

    // Position and movement
    Vector2 position;
    Vector2 past_dir;
    Vector2 past_pos;
    Vector2 input_direction;
    int direction;
    int m_dir;
    int step_timer;
    int FollowerID;
    int RunFrames;
    bool animating;
    bool move;
    bool canMove;
    int player_speed;
    float frameRate;
    float frameTimer;
    float keyPressTimer;

    // Collision detection
    Rectangle collisionMask;
    const int COLLISION_MASK_WIDTH = 16;
    const int COLLISION_MASK_HEIGHT = 16;
    int lastNPCId;
    int colID;
    Camera2D camera;
};

#endif // PLAYER_H
