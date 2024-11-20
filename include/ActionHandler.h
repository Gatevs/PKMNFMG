#pragma once

#include "PKMN.h"
#include "npc.h"
#include "player.h"
#include <iostream>
#include <math.h>
#include <raylib.h>
#include <string>
#include <vector>

// Enum representing different types of actions
enum class ActionType {
    Pause_M,
    Action_M,
    Dialogue_Box,
    Battle_M
};

class ActionHandler {
public:
    ActionHandler();
    ~ActionHandler();

    void handleAction(ActionType actionType, Vector2 drawPos);
    void getNPCInfo(int ID, std::vector<NPC>& NPC_objs, int Event);
    void getPlayerInfo(int ID, Player player_Obj, int Event);
    void getPKMNPartyInfo(std::vector<PKMN>& PKMNParty);
    void InputUI(std::vector<NPC>& NPC_objs, Player& player_Obj, std::vector<PKMN>& PKMNParty);
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
    bool FadeOutAtferMenu()const {return externalFadeOut;}
    void SetFade(int fadeAmount);
    bool stopPlayerInput;
    bool textFinished;
    void SetCamera(Camera2D& cam) {copyCam = cam;}

private:
        struct PKMNInfo {
        int SlotID;
        int Index;
        std::string Name;
        std::string MoveNames[4];
        int MoveIDs[4];
        int curPP[4];
        int MaxPP[4];
        int MoveSlots = 0;
        int Gender;
        int Lvl;
        int GStage;
        int HP = 1;
        float curHP = 1.0f;
        int UsingMove;
        float healthBar = (48.0f * (float(curHP)/HP));
        int healthBarColor = 0;
        int AttackPower = 0;
        bool StatusMoveSet = false;
        bool MoveActionSet = false;
        bool firstTurn = false;
        bool crithit = false;
        int blinkCounter = 0;
        bool visible = true;
        float yOffset = 0.0f;
        bool faintAnimDone = false;
        int EscapeAttempts = 0;
        bool BattleEnd = false;
    };

    void actionFollowMenu(std::vector<NPC>& NPC_objs, Player& player_Obj);
    void actionGrowthMenu(std::vector<NPC>& NPC_objs, Player& player_Obj);
    void actionStatsMenu();
    void actionBagMenu();
    void actionBattleMenu(Player& player, std::vector<PKMN>& PKMNParty);
    void pause(Player& p);
    void action(std::vector<NPC>& NPC_objs, Player& p);
    void dialogue(Player& player);
    void UpdateScreenState();
    void DrawPauseUI();
    void DrawActionUI();
    void unloadTextureFull(Texture2D& texture);
    void ScreenTransition(float posterizeAmount);
    void DrawTransitionEffect();

    void ExitBattle(Player& player);
    void DrawBattleUI();
    void Draw_BattleBG();
    void Draw_EnemyElements();
    void Draw_PlayerElements();
    void Draw_BattleTextBox();
    void BattleSpriteJiggle();
    void BattleUISelector(int max);
    void BattleMoveAction(PKMN pokeA, PKMN pokeB, PKMNInfo& pokeAInfo, PKMNInfo& pokeBInfo, int move);
    void SetMove(int move, PKMNInfo& poke, bool first);
    void NextPhase(PKMN& pokeA, PKMN& pokeB, PKMNInfo& pokeAInfo,PKMNInfo& pokeBInfo, int Phase);
    void FinishTurns(PKMN& pokeA, PKMN& pokeB, PKMNInfo& pokeAInfo,PKMNInfo& pokeBInfo);
    void Draw_Buttons(Vector2 pos, int offset);
    void DrawPokemonSprite(Vector2 pos, Texture2D sprite, float& yOffset, bool& animationDone, float health);
    bool IsPlayerEscaping(PKMNInfo& pokeInfo, PKMN& pokeA, PKMN& pokeB);

    void SetVNSprite();
    void SetNPCDialogue(std::string text);
    void SetDialogueAction(std::string action);
    void UpdateSelection(int pixels, int &menuID, int MAX_DOWN, int MAX_UP, Vector2 &pos);
    bool CompareDialogueConditions(std::string condition, std::string value, NPC& npc);
    void CloseUI(Player& player);
    void LeaveMenu();
    static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
    static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);
    void typewriterEffect(std::string& text);
    Camera2D copyCam;
    Shader fadeShader;

    Texture2D atlasTexture;
    Texture2D screenTexture;
    Texture2D TransitionTexture;

    Texture2D BattleBGTexture;
    Texture2D BaseTexture;
    Texture2D HPCardTexture;
    Texture2D PlayerBattleTexture;
    Texture2D PKMNBattleTexture;
    Texture2D BattleButtonsTexture;
    Texture2D MoveSelectorTexture;
    Texture2D PKMNTypeTexture;

    Texture2D StageTexture;
    Texture2D StatSprite;
    Texture2D VN_Sprite;
    Sound smallBeep;
    Sound GUIOpen;
    Sound GUIClose;
    Sound GUICursor;
    Font MainFont;
    Font BagFont;
    Font BattleFont;
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

    Vector2 BattleUIPos = {0,0};
    Vector2 HPCardEnemyPos;
    Vector2 HPCardFriendPos;
    Vector2 BattleButtonsPos;
    Vector2 EnemyBasePos;
    Vector2 PlayerBasePos;
    Vector2 PlayerSpritePos;
    Vector2 PlayerPKMNSpritePos;
    Vector2 EnemyPKMNSpritePos;
    Vector2 PlayerBackspriteFrame = {1,0};

    bool wordWrap;
    bool fadeInComplete = true;
    bool fadeOutComplete = true;
    bool externalFadeOut = false;
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
    int lastMenuID;
    int inUI;
    int selection;
    int Fade;
    int screenState;
    float TEXT_SPEED;
    int MAX_DOWN;
    int MAX_UP;
    int ActiveNPCVectorIndex = -1;

    int battlePhase;
    int TextBoxOpacity = 0;
    float battleTimer = 0.0f;
    int PlayerAnimFrame = 0;
    int UIJumpCount = 0;
    int PKMNSpriteJiggle = 0;

    float exposure = 80.0f;      // Starts at 0 (fully black)
    float posterizeLevel; // Number of levels for posterization
    float timer = 0.0f;      // Time elapsed (normalized between 0 and 1)

    PKMNInfo PlayerPKMNInfo;
    PKMNInfo EnemyPKMNInfo;
    std::vector<PKMN> EnemyMons;

    struct NPCInfo {
        std::string Name;
        int ID;
        int Limit;
        int Stage;
        bool FollowReject = false;
    } NPCInfo;
    enum Battle_States{
        SCREEN_TRANSITION,
        LOADING_ELEMENTS,
        SET_FIELD,
        ENEMY_INTRO,
        PLAYERPKMN_INTRO,
        WAIT_INPUT,
        SELECT_MOVE,
        TURN_A,
        TURN_B,
        FAINTING,
        VICTORY,
        EXIT,
        ESCAPE,
        ESCAPE_FAILED
    };
    enum Menu_Type{
        STATS,
        FOLLOW,
        GROWTH,
        OUTFIT
    };
    enum UI_Types{
        NONE,
        PAUSE,
        ACTION,
        DIALOGUE,
        BATTLE
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
    const int MainMove = 24;
    const int SubMove = 15;

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
        const Rectangle BaseEnemyMap = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(128),
        static_cast<float>(64)
    };
        const Rectangle BasePlayerMap = {
        static_cast<float>(0),
        static_cast<float>(64),
        static_cast<float>(256),
        static_cast<float>(32)
    };
        const Rectangle HPCardPlayerMap = {
        static_cast<float>(0),
        static_cast<float>(53),
        static_cast<float>(128),
        static_cast<float>(47)
    };
        const Rectangle HPCardEnemyMap = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(128),
        static_cast<float>(35)
    };
        Rectangle HealthPointMap = {
        static_cast<float>(0),
        static_cast<float>(36),
        static_cast<float>(48),
        static_cast<float>(3)
    };
        Rectangle GenderIconMap = {
        static_cast<float>(48),
        static_cast<float>(36),
        static_cast<float>(8),
        static_cast<float>(10)
    };
        Rectangle BattleButtonsMap = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(132),
        static_cast<float>(48)
    };
        const Rectangle MovementsUI = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(256),
        static_cast<float>(48)
    };
        const Rectangle MovementsButtons = {
        static_cast<float>(0),
        static_cast<float>(48),
        static_cast<float>(92),
        static_cast<float>(19)
    };
        const Rectangle MovementsSelector = {
        static_cast<float>(92),
        static_cast<float>(48),
        static_cast<float>(96),
        static_cast<float>(23)
    };
        const Rectangle PKMNTypes = {
        static_cast<float>(0),
        static_cast<float>(0),
        static_cast<float>(32),
        static_cast<float>(16)
    };
};
