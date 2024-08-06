#pragma once

#include "npc.h"
#include "player.h"
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
    void getNPCInfo(int ID, std::vector<NPC>& NPC_objs, int Event);
    void getPlayerInfo(int ID, Player player_Obj, int Event);
    void InputUI(std::vector<NPC>& NPC_objs, Player& player_Obj);
    void SetPlayerName(std::string player);
    void SetPlayerGender(std::string player);
    void SetInteractionID(int ID);
    void SetFadePos(Vector2 newFadePos){fadePos = newFadePos;}
    void Draw();
    void claenText();
    void fadeIn();
    void fadeOut();
    bool IsFadeInComplete() const {return fadeInComplete;}
    bool IsFadeOutComplete() const {return fadeOutComplete;}
    void SetFade(int fadeAmount);
    bool stopPlayerInput;
    bool textFinished;

private:
    void pause(Player& p);
    void action(std::vector<NPC>& NPC_objs, Player& p);
    void dialogue(Player& player);
    void PauseMenuBag();
    void UpdateScreenState();
    void DrawPauseUI();
    void DrawActionUI();
    void SetVNSprite();
    void SetNPCDialogue(std::string text);
    void SetDialogueAction(std::string action);
    bool CompareDialogueConditions(std::string condition, std::string value, NPC& npc);
    void CloseUI(Player& player);
    static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
    static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);
    void typewriterEffect(std::string& text);
    Texture2D atlasTexture;
    Texture2D screenTexture;
    Texture2D StageTexture;
    Texture2D StatSprite;
    Texture2D VN_Sprite;
    Sound smallBeep;
    Sound GUIOpen;
    Sound GUIClose;
    Sound GUICursor;
    Font MainFont;
    Font BagFont;
    Rectangle MainMap;
    Rectangle SubMap;
    Rectangle MainSelector;
    Rectangle SubSelector;
    Rectangle VN_Idle[2];
    Vector2 MainPos;
    Vector2 SubPos;
    Vector2 MainSelPos;
    Vector2 SubSelPos;
    Vector2 ICOPos;
    Vector2 fadePos;
    Vector2 CameraPos;
    bool wordWrap;
    bool fadeInComplete;
    bool fadeOutComplete;
    std::string DestTXT;
    std::string CurText;
    std::string NextText;
    std::string RemainingText;
    std::string DialogueText;
    std::string ActionFromCondition;
    std::string PLAYER_NAME;
    std::string PLAYER_GENDER;
    int curTextSize;
    int VN_Frame;
    float VN_Timer;
    int frameFlip;
    int ICO[11]{0,0,0,0,0,0,0,0,0,0};
    int textTimer;
    int menuID;
    int inUI;
    int selection;
    int Fade;
    int screenState;
    float TEXT_SPEED;
    int MAX_DOWN;
    int MAX_UP;
    struct NPCInfo {
        std::string Name;
        int ID;
        int Limit;
        int Stage;
        bool FollowReject = false;
    } NPCInfo;
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
    enum Column_Def{
        COMBINED_VALUES,
        ID,
        NAME,
        STAGE,
        LV,
        LOCATION,
        TEXT_DIALOGUE,
        EVENT,
        EVENT_CONDITION,
        EVENT_VALUE,
        GENDER
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
    const Rectangle BigSelectMap = {
        static_cast<float>(94),
        static_cast<float>(291),
        static_cast<float>(148),
        static_cast<float>(17)
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
        const Rectangle StageMap = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(51),
        static_cast<float>(51)
    };
        const Rectangle BagMap = {
        static_cast<float>(512),
        static_cast<float>(0),
        static_cast<float>(256),
        static_cast<float>(192)
    };
};
