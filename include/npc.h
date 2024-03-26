#ifndef NPC_H
#define NPC_H

#include <raylib.h>
#include "GameObject.h"
#include <string>
#include <vector>

class NPC :public GameObject {
public:
    NPC(int npcID, Vector2 NPCPos);
    ~NPC();

    // Core functionality
    void Update();
    void Draw() override;

    // Position and Movement
    void UpdatePosition();
    void npcMove(Vector2 past_direction, Vector2 past_position, int speed, bool moving, bool anim, int step);

    // Animation
    void updateAnimationFrameDimensions(const std::string& animation);
    void updateTexture(const std::string& animation);

    // Event handling
    void setNextStage(int nextStage);
    void lookAtPlayer(int lookingAt);

    // CSV parsing
    void parseCSV(const std::string& filename);

    // Accessors
    float GetYPosition() const override { return position.y; }
    int GetID() const { return ID; }
    Rectangle GetCollisionMask() const { return collisionMask; }
    std::string GetCombinedValues(int Event);
    bool IsNPCGrowing() const { return inGrowthPhase; }
    std::vector<std::vector<std::string>> GetNPCDialogue() {return m_data;}
    bool following_Player;
    void GetShadow(Texture2D load1, Texture2D load2);
    void setLimit(int newLimit);
    int GetLimit();
    int GetStage();

private:
    // Graphics
    Texture2D NPCTexture;
    Texture2D ShadowCentered;
    Texture2D ShadowOffCenter;
    Rectangle framesUp[3];
    Rectangle framesDown[3];
    Rectangle framesRight[3];
    Rectangle framesLeft[3];
    std::vector<Rectangle> framesGrowth;

    // Animation
    enum Animation {
        ANIM_UP,
        ANIM_DOWN,
        ANIM_RIGHT,
        ANIM_LEFT,
        ANIM_GROWTH
    };
    int FRAME_X;
    int FRAME_Y;
    int currentAnimation;
    int currentFrame;
    bool animating;
    float frameRate;
    float frameTimer;
    int lastFrame;

    // Position and Movement
    Vector2 position;
    int direction;
    int m_dir;
    int step_timer;
    bool move;
    bool inGrowthPhase;

    // Collision detection
    Rectangle collisionMask;
    const int COLLISION_MASK_WIDTH = 16;
    const int COLLISION_MASK_HEIGHT = 16;

    // NPC properties
    int ID;
    int Stage;
    int wantedStage;
    int lvl;
    std::vector<std::string> NPC_DEF;
    std::vector<std::vector<std::string>> m_data;
};

#endif // NPC_H
