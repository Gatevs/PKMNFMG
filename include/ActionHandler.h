#pragma once

#include "npc.h"
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>

// Enum representing different types of actions
enum class ActionType {
    Pause_M,
    Action_M,
    Dialogue_Box
};

class ActionHandler {
public:
    ActionHandler();
    ~ActionHandler();

    void handleAction(ActionType actionType, Vector2 drawPos);
    void getNPCInfo(int ID, std::vector<NPC>& NPC_objs);
    void InputUI();
    void Draw();
    void claenText();
    void fadeIn();
    void fadeOut();
    bool stopPlayerInput;
    bool textFinished;
    int InteractionID;

private:
    void pause();
    void action();
    void dialogue();
    void UpdateScreenState();
    void DrawPauseUI();
    void DrawActionUI();
    static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
    static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);
    void typewriterEffect(std::string& text);
    Texture2D atlasTexture;
    Texture2D screenTexture;
    Font MainFont;
    Rectangle MainMap;
    Rectangle SubMap;
    Rectangle MainSelector;
    Rectangle SubSelector;
    Vector2 MainPos;
    Vector2 SubPos;
    Vector2 MainSelPos;
    Vector2 SubSelPos;
    Vector2 ICOPos;
    Vector2 fadePos;
    bool wordWrap;
    bool fadeInComplete;
    bool fadeOutComplete;
    std::string DestTXT;
    std::string DialogueText;
    int ICO[11]{0,0,0,0,0,0,0,0,0,0};
    int textTimer;
    int menuID;
    int inUI;
    int selection;
    int Fade;
    int screenState;
    float TEXT_SPEED;
    enum UI_Types{
        NONE,
        PAUSE,
        ACTION,
        DIALOGUE
    };
    enum Screen_States{
        OFF,
        ON,
        WAIT,
        SHUTTNG_OFF
    };

    // Mapping Atlas elements (element X position in atlas, element Y position in atlas, element width, element height)
    const Rectangle pauseMap = {
        static_cast<float>(1),
        static_cast<float>(1),
        static_cast<float>(100),
        static_cast<float>(180)
    };
    const Rectangle actionMap = {
        static_cast<float>(1),
        static_cast<float>(182),
        static_cast<float>(100),
        static_cast<float>(108)
    };
    const Rectangle selectorMap = {
        static_cast<float>(1),
        static_cast<float>(291),
        static_cast<float>(92),
        static_cast<float>(26)
    };
    const Rectangle DialogueMap = {
        static_cast<float>(1),
        static_cast<float>(318),
        static_cast<float>(252),
        static_cast<float>(46)
    };
    const Rectangle YesNoMap = {
        static_cast<float>(102),
        static_cast<float>(46),
        static_cast<float>(60),
        static_cast<float>(44)
    };
    const Rectangle MenuICOMap = {
        static_cast<float>(163),
        static_cast<float>(1),
        static_cast<float>(25),
        static_cast<float>(25)
    };
    const Rectangle StatsMap = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(256),
        static_cast<float>(192)
    };
        const Rectangle GrowthMap = {
        static_cast<float>(256),
        static_cast<float>(0),
        static_cast<float>(256),
        static_cast<float>(192)
    };
        const Rectangle OutfitMap = {
        static_cast<float>(0),
        static_cast<float>(192),
        static_cast<float>(256),
        static_cast<float>(192)
    };
};
