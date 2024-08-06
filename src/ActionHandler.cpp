#include "ActionHandler.h"
#include <cstring>
#include <raylib.h>
#include <string>
#include "npc.h"
#include "player.h"
#include "ControllerSingleton.h"

ActionHandler::ActionHandler() {
    atlasTexture = LoadTexture("assets/MISC/UI_Atlas.png");
    MainFont = LoadFont("assets/SPRITEFONT/SpriteFont_Main.png");
    BagFont = LoadFont("assets/SPRITEFONT/SpriteFont_Bag.png");
    screenTexture = LoadTexture("assets/MISC/Screens_Atlas.png");
    StageTexture = LoadTexture("assets/MISC/GStage.png");
    smallBeep = LoadSound("assets/SFX/BEEP1.ogg");
    GUIOpen = LoadSound("assets/SFX/GUI_OPEN.ogg");
    GUIClose = LoadSound("assets/SFX/GUI_CLOSE.ogg");
    GUICursor = LoadSound("assets/SFX/GUI_MOVE.ogg");
    MainPos = {0,0};
    SubPos = {0,0};
    MainSelPos = {0,0};
    SubSelPos = {0,0};
    stopPlayerInput = false;
    inUI = NONE;
    screenState = OFF;
    menuID = 1;
    wordWrap = true;
    textTimer = 0;
    DestTXT = "";
    CurText = "";
    textFinished = false;
    TEXT_SPEED = 3.0f;
    NPCInfo.ID = 0;
    selection = 0;
    Fade = 0;
    fadeInComplete = false;
    fadeOutComplete = false;
    MAX_DOWN = 0;
    MAX_UP = 0;
    VN_Frame = 0;
    VN_Timer = 0.0f;
    frameFlip = 1;
    curTextSize = 0;
}


ActionHandler::~ActionHandler() {
    // Clean up any resources if needed
}

void splitDialogue(const std::string& dialogue, std::string& firstPart, std::string& remainingPart) {
    size_t pos = dialogue.find('|');
    if (pos != std::string::npos) {
        firstPart = dialogue.substr(0, pos);
        remainingPart = dialogue.substr(pos + 1);
    } else {
        firstPart = dialogue;
        remainingPart.clear();
    }
}

void replaceAll(std::string& str, const std::string& oldWord, const std::string& newWord) {
    size_t pos = 0;
    while ((pos = str.find(oldWord, pos)) != std::string::npos) {
        str.replace(pos, oldWord.length(), newWord);
        pos += newWord.length();  // Move past the newly replaced word
    }
}

// Writes each character with a delay
void ActionHandler::typewriterEffect(std::string& text) {
    // While the lenght of the text is not the same as the source text, keep the timer going
    if (DestTXT.length() != text.length()){
        textTimer += 1;
        textFinished = false;
    //If the lenght is the same as the source text, set text as finished
    }else{
        curTextSize += CurText.length();
        textFinished = true;
    }
    // Append a portion of the source text once the timer finishes
    if (textTimer > TEXT_SPEED){
        DestTXT = DestTXT.append(text.substr((DestTXT.length() - 1) + 1,1));
        textTimer = 0;
    }
}

// Function to handle different types of actions
void ActionHandler::handleAction(ActionType actionType, Vector2 drawPos) {
    CameraPos = drawPos;
    switch (actionType) {
        case ActionType::Pause_M:
            MainPos = Vector2{drawPos.x + 122, drawPos.y + 6};
            MainSelPos = Vector2{MainPos.x + 4, MainPos.y + 5};
            ICOPos = (Vector2){MainSelPos.x + 3, MainSelPos.y};
            fadePos = (Vector2){drawPos.x - 32, drawPos.y};
            MainMap = pauseMap;
            MainSelector = selectorMap;
            ICO[1] = MenuICOMap.width;
            stopPlayerInput = true;
            PlaySound(GUIOpen);
            inUI = PAUSE;
            break;
        case ActionType::Action_M:
            MainPos = Vector2{drawPos.x - 30, drawPos.y + 6};
            MainSelPos = Vector2{MainPos.x + 4, MainPos.y + 5};
            ICOPos = (Vector2){MainSelPos.x + 3, MainSelPos.y};
            fadePos = (Vector2){drawPos.x - 32, drawPos.y};
            MainMap = actionMap;
            MainSelector = selectorMap;
            ICO[1] = MenuICOMap.width;
            stopPlayerInput = true;
            PlaySound(GUIOpen);
            inUI = ACTION;
            break;
        case ActionType::Dialogue_Box:
            MainPos = Vector2{drawPos.x - 30, drawPos.y + 144};
            fadePos = (Vector2){drawPos.x - 32, drawPos.y};
            MainMap = DialogueMap;
            stopPlayerInput = true;
            PlaySound(smallBeep);
            SetVNSprite();
            inUI = DIALOGUE;
            break;
    }
}

void ActionHandler::InputUI(std::vector<NPC>& NPC_objs, Player& player_Obj){
    if (inUI == PAUSE){
        pause(player_Obj);
    }
    if (inUI == DIALOGUE){
        dialogue(player_Obj);
    }
    if (inUI == ACTION){
        action(NPC_objs, player_Obj);
    }
}


// Function to handle pause action
void ActionHandler::pause(Player& p) {
    if (textTimer < 10){
        textTimer += 1;
    }
    if (ControllerSingleton::GetInstance().IsDownPressed() and menuID < MAX_DOWN){
        switch (selection){
            case 0:
                for (int i = 0; i < 8; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y + 24;
                menuID += 1;
                ICO[menuID] = MenuICOMap.width;
                PlaySound(GUICursor);
                break;
            case 3:
                SubSelPos.y = SubSelPos.y + 15;
                menuID += 1;
                break;
        }
    }
    if (ControllerSingleton::GetInstance().IsUpPressed() and menuID > MAX_UP){
        switch (selection){
            case 0:
                for (int i = 0; i < 8; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y - 24;
                menuID -= 1;
                ICO[menuID] = MenuICOMap.width;
                PlaySound(GUICursor);
                break;
            case 3:
                SubSelPos.y = SubSelPos.y - 15;
                menuID -= 1;
                break;
        }
    }
    if (ControllerSingleton::GetInstance().IsXPressed() && textTimer >= 10 && selection == 0){
        CloseUI(p);
    }
    if (ControllerSingleton::GetInstance().IsBPressed() && textTimer >= 10 && selection == 0){
        CloseUI(p);
    }
    if (ControllerSingleton::GetInstance().IsAPressed()){
        if (selection == 0){
            PlaySound(smallBeep);
            selection = menuID;
            textTimer = 0;
        }
    }
    switch (selection) {
        case 0:
            MAX_DOWN = 7;
            MAX_UP = 1;
            break;
        case 1:
            selection = 0;
            break;
        case 2:
            selection = 0;
            break;
        case 3:
            PauseMenuBag();
            break;
        case 4:
            selection = 0;
            break;
        case 5:
            selection = 0;
            break;
        case 6:
            selection = 0;
            break;
        case 7:
            CloseUI(p);
            break;
    }

    //std::cout << "Pause action with value: " << menuID << std::endl;
}

void ActionHandler::PauseMenuBag(){
    if (screenState == OFF){
        menuID = 1;
        SubSelPos = (Vector2){fadePos.x + 105, fadePos.y + 15};
    }
    UpdateScreenState();
}

void ActionHandler::SetVNSprite(){
    std::string text = "assets/VN_SPRITE/" + std::to_string(NPCInfo.ID) + "_" + std::to_string(NPCInfo.Stage) + ".png";
    VN_Sprite = LoadTexture(text.c_str());
    int FRAME_X;
    int FRAME_Y;
    int textureWidth = VN_Sprite.width;
    int textureHeight = VN_Sprite.height;
    // Customize frame dimensions for "walk" animation
    int frameCountX = 2;
    int frameCountY = 1;

    // Calculate frame size
    FRAME_X = textureWidth / frameCountX;
    FRAME_Y = textureHeight / frameCountY;

    for (int i = 0; i < frameCountX; i++) {
        VN_Idle[i] = { static_cast<float>(i * FRAME_X), static_cast<float>(0 * FRAME_Y), static_cast<float>(FRAME_X), static_cast<float>(FRAME_Y) };
    }
}

// Function to handle generic action
void ActionHandler::action(std::vector<NPC>& NPC_objs, Player& p) {
    if (textTimer < 10){
        textTimer += 1;
    }
    if (ControllerSingleton::GetInstance().IsDownPressed() and menuID < MAX_DOWN){
        switch (selection){
            case 0:
                for (int i = 0; i < 5; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y + 24;
                menuID += 1;
                ICO[menuID] = MenuICOMap.width;
                PlaySound(GUICursor);
                break;
            case 3:
                SubSelPos.y = SubSelPos.y + 15;
                menuID += 1;
                break;
        }
    }
    if (ControllerSingleton::GetInstance().IsUpPressed() and menuID > MAX_UP){
        switch (selection){
            case 0:
                for (int i = 0; i < 5; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y - 24;
                menuID -= 1;
                ICO[menuID] = MenuICOMap.width;
                PlaySound(GUICursor);
                break;
            case 3:
                SubSelPos.y = SubSelPos.y - 15;
                menuID -= 1;
                break;
        }
    }
    if (ControllerSingleton::GetInstance().IsYPressed() && textTimer >= 10 && selection == 0){
        CloseUI(p);
    }
    if (ControllerSingleton::GetInstance().IsBPressed()){
        switch (selection){
            case 0:
                CloseUI(p);
                break;
            case 1:
                if (screenState == ON && fadeOutComplete){
                    fadeIn();
                    screenState = WAIT;
                }
                break;
            case 2:
                selection = 0;
                break;
            case 3:
                if (screenState == ON && fadeOutComplete){
                    fadeIn();
                    screenState = WAIT;
                }
                break;
            case 4:
                if (screenState == ON && fadeOutComplete){
                    fadeIn();
                    screenState = WAIT;
                }
                break;
        }
    }
    if (ControllerSingleton::GetInstance().IsAPressed()){
        if (selection == 0){
            PlaySound(smallBeep);
            selection = menuID;
            textTimer = 0;
        if (selection == 1 && screenState == OFF){
            std::string text = "assets/STAT_SPRITES/" + std::to_string(NPCInfo.ID) + "_" + std::to_string(NPCInfo.Stage) + ".png";
            if (!IsTextureReady(StatSprite)){
                StatSprite = LoadTexture(text.c_str());
            }
        }
        }
        if (selection == 3 && screenState == OFF){
            menuID = 1;
            SubSelPos = (Vector2){fadePos.x + 105, fadePos.y + 15};
        }
        switch (selection){
            case 2:
                if (p.GetPlayerFollower() != NPCInfo.ID && textTimer >= 10){
                    int npcIdInFront = p.CheckForNPCInFront(NPC_objs);
                    if (npcIdInFront != -1) {
                        // Set NPC as follower and update directions
                        for (auto& npc : NPC_objs) {
                            if (npc.GetID() == npcIdInFront) {
                                getNPCInfo(npc.GetID(),NPC_objs, 3);
                                if (!NPCInfo.FollowReject){
                                    p.SetFollowerID(npcIdInFront);
                                    npc.following_Player = true;
                                    switch (p.GetPlayerDir()){
                                        case 270:
                                            p.SetFollowDir({0,-1});
                                            break;
                                        case 90:
                                            p.SetFollowDir({0,1});
                                            break;
                                        case 0:
                                            p.SetFollowDir({-1,0});
                                            break;
                                        case 180:
                                            p.SetFollowDir({1,0});
                                            break;
                                    }
                                }
                                npc.lookAtPlayer(p.GetPlayerDir());
                                menuID = 1;
                                textTimer = 0;
                                selection = 0;
                                for (int i = 0; i < 8; ++i) {
                                    ICO[i] = 0;
                                }
                                handleAction(ActionType::Dialogue_Box,CameraPos);
                                NPCInfo.FollowReject = false;
                                break;
                            }
                        }
                    }
                }else if (p.GetPlayerFollower() == NPCInfo.ID && textTimer >= 10){
                    for (auto& npc : NPC_objs) {
                        if (npc.GetID() == NPCInfo.ID) {
                            npc.following_Player = false;
                            npc.lookAtPlayer(p.GetPlayerDir());
                            getNPCInfo(npc.GetID(),NPC_objs, 4);
                            menuID = 1;
                            textTimer = 0;
                            selection = 0;
                            for (int i = 0; i < 8; ++i) {
                                ICO[i] = 0;
                            }
                            p.SetFollowerID(0);
                            handleAction(ActionType::Dialogue_Box,CameraPos);
                            break;
                        }
                    }
                }
                break;
            case 3:
                if (fadeOutComplete){
                    if (NPCInfo.ID == 1){
                        p.setNextStage(menuID - 1);
                        CloseUI(p);
                        break;
                    }
                    for (auto& npc : NPC_objs) {
                        if (npc.GetID() == NPCInfo.ID) {
                            npc.setNextStage(menuID - 1);
                            if (NPCInfo.ID == p.GetPlayerFollower()){
                                p.SetFollowerID(0);
                            }
                            CloseUI(p);
                            break;
                        }
                    }
                }
                break;
        }
    }

    switch (selection){
        case 0:
            MAX_DOWN = 4;
            MAX_UP = 1;
            break;
        case 1:
            UpdateScreenState();
            break;
        case 2:
            if (textTimer < 10){
                textTimer += 1;
            }
            if (p.GetPlayerFollower() != NPCInfo.ID){
                SubMap = (Rectangle){YesNoMap.x,YesNoMap.y,YesNoMap.width,YesNoMap.height};
            } else{
                SubMap = (Rectangle){YesNoMap.x,YesNoMap.y - 45,YesNoMap.width,YesNoMap.height};
            }
            SubPos = Vector2{MainPos.x + 102, MainPos.y};
            break;
        case 3:
            MAX_DOWN = NPCInfo.Limit + 1;
            MAX_UP = 1;
            UpdateScreenState();
            if (screenState == SHUTTNG_OFF){
                    menuID = 3;
                }
            break;
        case 4:
            UpdateScreenState();
            break;
    }
}

void ActionHandler::UpdateScreenState() {
    // Fade in or out based on current state
    if (!fadeInComplete) {
        fadeIn();
    } else if (!fadeOutComplete) {
        fadeOut();
    }

    // Update screen state based on fade-in and fade-out completion
    if (fadeInComplete && screenState == OFF) {
        // Screen is fully faded in and turned on
        screenState = ON;
    }
    if (fadeInComplete && screenState == WAIT && fadeOutComplete) {
        // Screen is fading out after waiting period
        screenState = SHUTTNG_OFF;
        fadeOutComplete = false;
    }
    if (fadeOutComplete && screenState == SHUTTNG_OFF) {
        // Screen is fully faded out and turned off
        selection = 0; // Reset any selections
        fadeInComplete = false;
        fadeOutComplete = false;
        screenState = OFF;
    }
}

void ActionHandler::getNPCInfo(int ID, std::vector<NPC>& NPC_objs, int Event) {
    std::string defaultLine = "";
    std::string defaultAction = "";
    bool foundNonEmpty = false;

    for (auto& npc : NPC_objs) {
        if (npc.GetID() == ID) {
            std::string targetValue = npc.GetCombinedValues(Event);
            NPCInfo.ID = npc.GetID();
            NPCInfo.Limit = npc.GetLimit();
            NPCInfo.Stage = npc.GetStage();

            for (const auto& row : npc.GetNPCDialogue()) {
                if (row[COMBINED_VALUES] == targetValue) {
                    NPCInfo.Name = row[NAME];
                    if (!row[EVENT_CONDITION].empty()) {
                        if (CompareDialogueConditions(row[EVENT_CONDITION],row[EVENT_VALUE],npc)){
                            foundNonEmpty = true;
                            SetDialogueAction(ActionFromCondition);
                            SetNPCDialogue(row[TEXT_DIALOGUE]);
                            break;
                        }
                    } else if (PLAYER_GENDER == row[GENDER] || row[GENDER].empty()) {
                        defaultLine = row[TEXT_DIALOGUE];
                        defaultAction = row[EVENT_VALUE];
                    }
                }
            }

            if (!foundNonEmpty) {
                SetDialogueAction(defaultAction);
                SetNPCDialogue(defaultLine);
            }
            return;
        }
    }
    // Error handling if NPC with the given ID is not found
    // You may want to log an error message or throw an exception
}

bool ActionHandler::CompareDialogueConditions(std::string condition, std::string value, NPC& npc){
    ActionFromCondition = value;
    if (condition == "HasGrown" && npc.GetNPCEventState().hasGrown) {
        return true;
    }
    if (condition == "TimesGrown" && npc.GetNPCEventState().timesGrown >= std::stoi(value)) {
        return true;
    }
    if (condition == "FirstTime" && npc.GetNPCEventState().newGrowthStage == npc.GetNPCEventState().maxGrowthStage && npc.GetNPCEventState().hasGrown) {
        npc.SetNewGrowthStage();  // Set new growth stage if condition is met
        return true;
    }
    return false;
}

void ActionHandler::SetDialogueAction(std::string action){
    if (action == "FollowNo"){
        NPCInfo.FollowReject = true;
    } else {
        return;
    }
}

void ActionHandler::SetNPCDialogue(std::string text){
    DialogueText = text;
    replaceAll(DialogueText,"PLAYERNAME", PLAYER_NAME);
    replaceAll(DialogueText,"¬", "\n");
    splitDialogue(DialogueText,CurText,RemainingText);
}

void ActionHandler::getPlayerInfo(int ID, Player player_Obj, int Event){
    NPCInfo.Limit = 1;
    NPCInfo.Stage = player_Obj.GetStage();
    NPCInfo.Name = player_Obj.GetPlayerName();
}


void ActionHandler::dialogue(Player& player) {
    const float frameRate = 0.400f;
    const float TEXT_SPEED_NORMAL = 3.0f;
    const float TEXT_SPEED_FAST = 1.5f;

    // Handle text typing effect
    if (!textFinished){
        typewriterEffect(CurText);
    }

    // Adjust text speed based on key state
    if (ControllerSingleton::GetInstance().IsAHeld()){
        TEXT_SPEED = TEXT_SPEED_FAST;
    }else{
        TEXT_SPEED = TEXT_SPEED_NORMAL;
    }

    // Process key press events
    if (ControllerSingleton::GetInstance().IsAPressed()){
        if (textFinished && curTextSize == DialogueText.length()){
            // Text finished displaying, reset and stop UI
            stopPlayerInput = false;
            claenText();
            curTextSize = 0;
            player.StopUI_Element();
        } else if (textFinished && curTextSize != DialogueText.length()){
            // Display next portion of text
            curTextSize += 1;
            NextText = RemainingText;
            splitDialogue(NextText,CurText,RemainingText);
            DestTXT = "";
            textFinished = false;
            PlaySound(smallBeep);
        }
    }
    //VN type sprite
    VN_Timer += GetFrameTime();
    if (VN_Timer >= frameRate) {
        VN_Frame += frameFlip;
        if (VN_Frame > 0) {
            VN_Frame = 1;
            frameFlip = -1;
        }
        if (VN_Frame < 0) {
            VN_Frame = 1;
            frameFlip = 1;
        }
        VN_Timer = 0.0f;
    }
}

void ActionHandler::fadeIn(){
    const int MAX_FADE_VALUE = 255;
    const int FADE_INCREMENT = 10; // Adjust the increment value as needed

    if (Fade < MAX_FADE_VALUE) {
        // Increment Fade by FADE_INCREMENT, but ensure it doesn't exceed MAX_FADE_VALUE
        Fade = std::min(Fade + FADE_INCREMENT, MAX_FADE_VALUE);
        fadeInComplete = false;
    } else {
        Fade = MAX_FADE_VALUE;
        fadeInComplete = true;
    }
}

void ActionHandler::fadeOut(){
    const int MIN_FADE_VALUE = 0;
    const int FADE_DECREMENT = 10; // Adjust the decrement value as needed

    if (Fade > MIN_FADE_VALUE) {
        // Decrement Fade by FADE_DECREMENT, but ensure it doesn't go below MIN_FADE_VALUE
        Fade = std::max(Fade - FADE_DECREMENT, MIN_FADE_VALUE);
        fadeOutComplete = false;
    } else {
        Fade = MIN_FADE_VALUE;
        fadeOutComplete = true;
    }
}

void ActionHandler::claenText(){
    DestTXT = "";
    DialogueText = "";
    textTimer = 0;
    textFinished = false;
    UnloadTexture(VN_Sprite);
    VN_Sprite.id = 0;
    VN_Sprite.format = 0;
    VN_Sprite.height = 0;
    VN_Sprite.width = 0;
    VN_Sprite.mipmaps = 0;
}

void ActionHandler::SetInteractionID(int ID){
    NPCInfo.ID = ID;
}

void ActionHandler::CloseUI(Player& player){
    menuID = 1;
    stopPlayerInput = false;
    textTimer = 0;
    selection = 0;
    for (int i = 0; i < 8; ++i) {
        ICO[i] = 0;
    }
    fadeInComplete = false;
    fadeOutComplete = false;
    screenState = OFF;
    if (IsTextureReady(StatSprite)){
        UnloadTexture(StatSprite);
        StatSprite.id = 0;
        StatSprite.format = 0;
        StatSprite.height = 0;
        StatSprite.width = 0;
        StatSprite.mipmaps = 0;
    }
    PlaySound(GUIClose);
    player.StopUI_Element();
}

// Draw text using font inside rectangle limits with support for text selection
void ActionHandler::DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;          // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/(float)font.baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;         // Index where to begin drawing (where a line begins)
    int endLine = -1;           // Index where to stop drawing (where a line ends)
    int lastk = -1;             // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width*scaleFactor : font.glyphs[index].advanceX*scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE)
        {
            // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // Ref: http://jkorpela.fi/chars/spaces.html
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1)? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize/2.0f)*scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize/2.0f)*scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize*scaleFactor) > rec.height) break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle){ rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize*scaleFactor }, selectBackTint);
                    isGlyphSelected = true;
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize/2.0f)*scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;  // avoid leading spaces
    }
}

void ActionHandler::DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}


void ActionHandler::Draw(){
    if (stopPlayerInput){
        switch (inUI){
            case PAUSE:
                DrawTextureRec(atlasTexture, MainMap, MainPos, WHITE);
                DrawPauseUI();
                break;
            case DIALOGUE:
                DrawTextureRec(VN_Sprite, VN_Idle[VN_Frame], (Vector2){(fadePos.x + 160) - (VN_Sprite.width / 2.0f),(fadePos.y + 170) - VN_Sprite.height}, WHITE);
                DrawTextureRec(atlasTexture, MainMap, MainPos, WHITE);
                DrawTextBoxed(MainFont, DestTXT.c_str(), (Rectangle){ MainPos.x + 16, MainPos.y +8, 220, 60 }, MainFont.baseSize, -5, wordWrap, WHITE);
                break;
            case ACTION:
                DrawTextureRec(atlasTexture, MainMap, MainPos, WHITE);
                DrawActionUI();
                break;
        }
    }
    // Fade in and out
    DrawRectangle(fadePos.x,fadePos.y,256,194,(Color) {0,0,0,static_cast<unsigned char>(Fade)});
}
void ActionHandler::DrawPauseUI(){
    DrawTextureRec(atlasTexture, MainSelector, MainSelPos, WHITE);
    for (int i = 1; i <= 7; ++i) {
        DrawTextureRec(atlasTexture, (Rectangle) {MenuICOMap.x + ICO[i], MenuICOMap.y + (MenuICOMap.height * (i - 1)), MenuICOMap.width, MenuICOMap.height},(Vector2){ICOPos.x + 1, ICOPos.y + (MenuICOMap.height * (i - 1)) - (i - 1)}, WHITE);
    }
    DrawTextBoxed(MainFont, "Player", (Rectangle){MainPos.x + 33, MainPos.y + 84, 60, 30}, MainFont.baseSize, -5, wordWrap, WHITE);
    switch (selection) {
        case 3:
            if (screenState == ON || screenState == WAIT){
                DrawTextureRec(screenTexture, BagMap, fadePos, WHITE);
                DrawTextureRec(atlasTexture,BigSelectMap,SubSelPos, WHITE );
            }
            break;

}
}

void ActionHandler::DrawActionUI(){
    DrawTextureRec(atlasTexture, MainSelector, MainSelPos, WHITE);
    for (int i = 1; i <= 4; ++i) {
        DrawTextureRec(atlasTexture, (Rectangle) {MenuICOMap.x + ICO[i], MenuICOMap.y + (MenuICOMap.height * ((i - 1) + 7)), MenuICOMap.width, MenuICOMap.height},(Vector2){ICOPos.x + 1, ICOPos.y + (MenuICOMap.height * (i - 1)) - (i - 1)}, WHITE);
    }
    switch (selection){
        case 1:
            if (screenState == ON || screenState == WAIT){
                DrawTextureRec(screenTexture, StatsMap, fadePos, WHITE);
                DrawTextBoxed(MainFont, NPCInfo.Name.c_str(), (Rectangle){fadePos.x + 185, fadePos.y + 6, 60, 30}, MainFont.baseSize, -5, wordWrap, WHITE);
                DrawTexture(StatSprite,(fadePos.x + 128) - (StatSprite.width / 2.0f),(fadePos.y + 175) - StatSprite.height,WHITE);
            }
            break;
        case 2:
            DrawTextureRec(atlasTexture, SubMap, SubPos, WHITE);
            break;
        case 3:
            if (screenState == ON || screenState == WAIT){
                DrawTextureRec(screenTexture, GrowthMap, fadePos, WHITE);
                DrawTextureRec(StageTexture,(Rectangle){StageMap.x, StageMap.y + (51 * (menuID - 1)), StageMap.width,StageMap.height},(Vector2){fadePos.x + 18, fadePos.y + 40},WHITE);
                DrawTextureRec(atlasTexture,BigSelectMap,SubSelPos, WHITE );
                for (int i = 1; i <= MAX_DOWN; ++i) {
                    std::string GrowText = "Stage: "+ std::to_string(i - 1);
                    DrawTextBoxed(MainFont, GrowText.c_str(), (Rectangle){fadePos.x + 115, (fadePos.y + 17) + ((i * 15) - 15), 60, 30}, MainFont.baseSize, -5, wordWrap, WHITE);
                }
            }
            break;
        case 4:
            if (screenState == ON || screenState == WAIT){
                DrawTextureRec(screenTexture, OutfitMap, fadePos, WHITE);
            }
            break;
    }
}

void ActionHandler::SetPlayerName(std::string player){
    PLAYER_NAME = player;
}

void ActionHandler::SetPlayerGender(std::string player){
    PLAYER_GENDER = player;
}

void ActionHandler::SetFade(int fadeAmount){
    if (fadeAmount == -1){
        fadeInComplete = false;
        fadeOutComplete = false;
        Fade = 0;
    } else{
        Fade = fadeAmount;
    }
}
