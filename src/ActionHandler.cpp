#include "ActionHandler.h"
#include <cstring>
#include <raylib.h>
#include "npc.h"

ActionHandler::ActionHandler() {
    atlasTexture = LoadTexture("assets/UI_Atlas.png");
    MainFont = LoadFont("assets/SpriteFont_Main.png");
    screenTexture = LoadTexture("assets/Screens_Atlas.png");
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
    textFinished = true;
    TEXT_SPEED = 3.0f;
    InteractionID = 0;
    selection = 0;
    Fade = 0;
    fadeInComplete = false;
    fadeOutComplete = false;
}


ActionHandler::~ActionHandler() {
    // Clean up any resources if needed
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
    switch (actionType) {
        case ActionType::Pause_M:
            MainPos = Vector2{drawPos.x + 42, drawPos.y - 74};
            MainSelPos = Vector2{MainPos.x + 4, MainPos.y + 5};
            ICOPos = (Vector2){MainSelPos.x + 3, MainSelPos.y};
            MainMap = pauseMap;
            MainSelector = selectorMap;
            ICO[1] = MenuICOMap.width;
            stopPlayerInput = true;
            inUI = PAUSE;
            InputUI();
            break;
        case ActionType::Action_M:
            MainPos = Vector2{drawPos.x - 110, drawPos.y - 74};
            MainSelPos = Vector2{MainPos.x + 4, MainPos.y + 5};
            ICOPos = (Vector2){MainSelPos.x + 3, MainSelPos.y};
            fadePos = (Vector2){drawPos.x - 112, drawPos.y - 80};
            MainMap = actionMap;
            MainSelector = selectorMap;
            ICO[1] = MenuICOMap.width;
            stopPlayerInput = true;
            inUI = ACTION;
            InputUI();
            break;
        case ActionType::Dialogue_Box:
            MainPos = Vector2{drawPos.x - 110, drawPos.y + 65};
            MainMap = DialogueMap;
            stopPlayerInput = true;
            inUI = DIALOGUE;
            InputUI();
            break;

    }
}

void ActionHandler::InputUI(){
    if (inUI == PAUSE){
        pause();
    }
    if (inUI == DIALOGUE){
        dialogue();
    }
    if (inUI == ACTION){
        action();
    }
}


// Function to handle pause action
void ActionHandler::pause() {
    if (textTimer < 10){
        textTimer += 1;
    }
    if (IsKeyPressed(KEY_DOWN) and menuID < 7){
        switch (selection){
            case 0:
                for (int i = 0; i < 8; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y + 24;
                menuID += 1;
                ICO[menuID] = MenuICOMap.width;
                break;
            case 2:
                break;
        }
    }
    if (IsKeyPressed(KEY_UP) and menuID > 1){
        switch (selection){
            case 0:
                for (int i = 0; i < 8; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y - 24;
                menuID -= 1;
                ICO[menuID] = MenuICOMap.width;
                break;
            case 2:
                break;
        }
    }
    if (IsKeyPressed(KEY_A) && textTimer >= 10 && selection == 0){
        menuID = 1;
        stopPlayerInput = false;
        textTimer = 0;
        selection = 0;
        for (int i = 0; i < 8; ++i) {
            ICO[i] = 0;
        }
    }
    if (IsKeyPressed(KEY_X)){
        menuID = 1;
        stopPlayerInput = false;
        textTimer = 0;
    }

    //std::cout << "Pause action with value: " << menuID << std::endl;
}

// Function to handle generic action
void ActionHandler::action() {
    if (textTimer < 10){
        textTimer += 1;
    }
    if (IsKeyPressed(KEY_DOWN) and menuID < 4){
        switch (selection){
            case 0:
                for (int i = 0; i < 5; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y + 24;
                menuID += 1;
                ICO[menuID] = MenuICOMap.width;
                break;
            case 2:
                break;
        }
    }
    if (IsKeyPressed(KEY_UP) and menuID > 1){
        switch (selection){
            case 0:
                for (int i = 0; i < 5; ++i) {
                    ICO[i] = 0;
                }
                MainSelPos.y = MainSelPos.y - 24;
                menuID -= 1;
                ICO[menuID] = MenuICOMap.width;
                break;
            case 2:
                break;
        }
    }
    if (IsKeyPressed(KEY_S) && textTimer >= 10 && selection == 0){
        menuID = 1;
        stopPlayerInput = false;
        textTimer = 0;
        selection = 0;
        for (int i = 0; i < 5; ++i) {
            ICO[i] = 0;
        }
    }
    if (IsKeyPressed(KEY_X)){
        switch (selection){
            case 0:
                menuID = 1;
                stopPlayerInput = false;
                textTimer = 0;
                selection = 0;
                for (int i = 0; i < 5; ++i) {
                    ICO[i] = 0;
                }
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
    if (IsKeyPressed(KEY_Z)){
        selection = menuID;
    }

    switch (selection){
        case 1:
            UpdateScreenState();
            break;
        case 2:
            SubMap = YesNoMap;
            SubPos = Vector2{MainPos.x + 102, MainPos.y};
            break;
        case 3:
            UpdateScreenState();
            break;
        case 4:
            UpdateScreenState();
            break;
    }
    // Add code to handle generic action here
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

void ActionHandler::getNPCInfo(int ID, std::vector<NPC>& NPC_objs) {
    for (auto& npc : NPC_objs) {
        if (npc.GetID() == ID) {
            std::string targetValue = npc.GetCombinedValues(1);
            // Iterate over each row of NPC data
            for (const auto& row : npc.GetNPCDialogue()) {
                // Check if the value in the first column matches the target value
                if (row[0] == targetValue) {
                    // If a match is found, set DialogueText to the value in the seventh column
                    DialogueText = row[6];
                    break; // Stop searching once a match is found
                }
            }
            break; // Stop searching for NPCs after finding the one with the matching ID
        }
    }
}


void ActionHandler::dialogue() {
    replaceAll(DialogueText,"PLAYERNAME", "Player");
    typewriterEffect(DialogueText);

    if (IsKeyDown(KEY_Z)){

        TEXT_SPEED = 1.5f;
    }else{
        TEXT_SPEED = 3.0f;
    }
    if (IsKeyPressed(KEY_Z)){
        if (textFinished){
            stopPlayerInput = false;
            claenText();
        }
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
        DrawTextureRec(atlasTexture, MainMap, MainPos, WHITE);
        switch (inUI){
            case PAUSE:
                DrawPauseUI();
                break;
            case DIALOGUE:
                DrawTextBoxed(MainFont, DestTXT.c_str(), (Rectangle){ MainPos.x + 16, MainPos.y +8, 220, 60 }, MainFont.baseSize, -5, wordWrap, WHITE);
                break;
            case ACTION:
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
            }
            break;
        case 2:
            DrawTextureRec(atlasTexture, SubMap, SubPos, WHITE);
            break;
        case 3:
            if (screenState == ON || screenState == WAIT){
                DrawTextureRec(screenTexture, GrowthMap, fadePos, WHITE);
            }
            break;
        case 4:
            if (screenState == ON || screenState == WAIT){
                DrawTextureRec(screenTexture, OutfitMap, fadePos, WHITE);
            }
            break;
    }
}
