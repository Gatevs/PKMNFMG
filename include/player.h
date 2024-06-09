#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <LDtkLoader/Project.hpp>
#include "GameObject.h"
#include "npc.h"
#include <string>
#include <vector>
#include <algorithm>

class Player :public GameObject {
public:
    Player();
    ~Player();

// Core functionality
    void Update();
    void Draw() override;

    // Animation
    void UpdateTexture(const std::string& animation);
    void updateAnimationFrameDimensions(const std::string& animation);

    void setNextStage(int nextStage);
    bool IsNPCGrowing() const { return inGrowthPhase; }

    // CSV parsing
    void parseCSV(const std::string& filename);

    // Input handling
    void HandleInput(std::vector<NPC>& npcs);
    int InvokeUIElement();

    // Movement and position
    void npcMoving(std::vector<NPC>& npcs);
    void UpdatePositionAndCamera(float FixedDt);
    void UpdateAnim ();
    void setPosition(Vector2 newPos);

    // Collision detection
    void checkCollisions(const ldtk::Layer& collisions,  const std::vector<NPC>& npcs, Vector2 IntGridOffset);
    Rectangle ColOffset(bool Tile) const;

    // Accessors
    void SetFollowerID(int follow);
    float GetYPosition() const override { return position.y; }
    std::string GetLocation() {return location;}
    Vector2 GetPosition() const;
    int GetPlayerDir() const;
    int GetPlayerSpeed() const;
    int GetPlayerFollower() const;
    bool IsPlayerMoving() const;
    bool IsPlayerAnimating() const;
    int CheckForNPCInFront(const std::vector<NPC>& npcs) const;
    void SetShadow(Texture2D load);
    void StopUI_Element();
    void SetFollowDir(Vector2 Dir);
    void SetLocation (std::string loc);
    void SetPlayerGender(std::string playerGender);
    std::string GetPlayerGender() const;
    std::string GetPlayerName() const;
    int GetStage() const;
    bool IntGridValueAtPosition(const ldtk::Layer& collisions, int value, Vector2 IntGridOffset);
    void StopPlayer();

private:
    void Growth();
    void BumpAgainstObject();
    std::string Gender;
    std::string Name;
    int Stage;

    // Graphics
    Texture2D playerTexture;
    Texture2D playerTextureIdle;
    Sound Bump;
    Rectangle framesUp[3];
    Rectangle framesDown[3];
    Rectangle framesRight[3];
    Rectangle framesLeft[3];
    std::vector<Rectangle> framesGrowth;
    Texture2D shadowTexture;

    // Animation
    enum Animation {
        ANIM_UP,
        ANIM_DOWN,
        ANIM_RIGHT,
        ANIM_LEFT,
        ANIM_GROWTH
    };
    enum UI_Element {
        PAUSE,
        DIALOGUE,
        ACTION,
        NONE
    };
    int FRAME_X;
    int FRAME_Y;
    int currentAnimation;
    int currentFrame;
    std::vector<std::string> PLAYER_DEF;

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
    int LoadUI_Element;
    int wantedStage;
    bool animating;
    bool move;
    bool canMove;
    bool inGrowthPhase;
    int player_speed;
    float frameRate;
    float frameTimer;
    float keyPressTimer;
    std::string location;
    int lastFrame;

    // Collision detection
    Rectangle collisionMask;
    const int COLLISION_MASK_WIDTH = 16;
    const int COLLISION_MASK_HEIGHT = 16;
    int lastNPCId;
    int colID;
    Camera2D camera;
};

#endif // PLAYER_H
