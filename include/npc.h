#ifndef NPC_H
#define NPC_H

#include <raylib.h>
#include "GameObject.h"
#include <string>
#include <vector>

struct NPCState {
    bool hasGrown;
    int newGrowthStage = 0;
    int maxGrowthStage;
    int timesGrown;
    // Add more flags and properties as needed
};

class NPC :public GameObject {
public:
    NPC(int npcID, std::string loc, Vector2 NPCPos);
    ~NPC();

    // Core functionality
    void Update();
    void Draw() override;

    // Position and Movement
    void npcMove(Vector2 past_direction, Vector2 past_position, int speed, bool moving, bool anim, int step);

    // Animation
    void updateAnimationFrameDimensions(const std::string& animation);
    void updateTexture(const std::string& animation);

    // Event handling
    void setNextStage(int nextStage);
    void lookAtPlayer(int lookingAt);
    void followPlayer();

    // CSV parsing
    void parseCSV(const std::string& filename);

    // Accessors
    float GetYPosition() const override { return position.y; }
    std::string GetLocation() const {return location;}
    int GetID() const { return ID; }
    Rectangle GetCollisionMask() const { return collisionMask; }
    std::string GetCombinedValues(int Event);
    bool IsNPCGrowing() const { return inGrowthPhase; }
    std::vector<std::vector<std::string>> GetNPCDialogue() {return m_data;}
    NPCState GetNPCEventState() const {return NPCEventData;}
    bool following_Player;
    void SetShadow(Texture2D load1, Texture2D load2);
    void setLimit(int newLimit);
    void SetNewGrowthStage();
    int GetLimit();
    int GetStage() const {return Stage;};
    void SetStage(int newStage);
    Vector2 GetPosition() const { return position;}
    std::string GetNPCLocation() const {return location;}
    void SetPosition(Vector2 pos);
    bool GetIdleAnim() const {return idle;}

private:
    // Graphics
    Texture2D NPCTexture;
    Texture2D NPCTextureIdle;
    Texture2D DrawnTexture;
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
    bool idle;

    // Position and Movement
    Vector2 position;
    int direction;
    int m_dir;
    int step_timer;
    bool move;
    bool inGrowthPhase;

    // Collision detection
    Rectangle collisionMask;
    int COLLISION_MASK_WIDTH = 16;
    int COLLISION_MASK_HEIGHT = 16;

    // NPC properties
    int ID;
    int Stage;
    int wantedStage;
    int lvl;
    std::string location;
    std::vector<std::string> NPC_DEF;
    std::vector<std::vector<std::string>> m_data;
    NPCState NPCEventData;
};

#endif // NPC_H
