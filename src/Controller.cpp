// Controller.cpp
#include "Controller.h"
#include "raylib.h"
#include <iostream>

Controller::Controller(){
    TouchLayout = LoadTexture("assets/MISC/TouchControl.png");
}

Controller::~Controller(){

}

void Controller::Update() {
    ResetFlags();

    // Keyboard controls
    if (IsKeyPressed(KEY_UP)) Up_Pressed = true;
    if (IsKeyPressed(KEY_DOWN)) Down_Pressed = true;
    if (IsKeyPressed(KEY_LEFT)) Left_Pressed = true;
    if (IsKeyPressed(KEY_RIGHT)) Right_Pressed = true;
    if (IsKeyPressed(KEY_Z)) A_Pressed = true;
    if (IsKeyPressed(KEY_X)) B_Pressed = true;
    if (IsKeyPressed(KEY_A)) X_Pressed = true;
    if (IsKeyPressed(KEY_S)) Y_Pressed = true;
    if (IsKeyDown(KEY_UP)) Up_Down = true;
    if (IsKeyDown(KEY_DOWN)) Down_Down = true;
    if (IsKeyDown(KEY_LEFT)) Left_Down = true;
    if (IsKeyDown(KEY_RIGHT)) Right_Down = true;
    if (IsKeyDown(KEY_Z)) A_Down = true;
    if (IsKeyDown(KEY_X)) B_Down = true;
    if (IsKeyDown(KEY_A)) X_Down = true;
    if (IsKeyDown(KEY_S)) Y_Down = true;

    if (TouchControllerON) {
        HandleTouchControls();
    }
}

void Controller::ResetFlags() {
    Up_Pressed = Down_Pressed = Left_Pressed = Right_Pressed = A_Pressed = B_Pressed = X_Pressed = Y_Pressed = false;
    Up_Down = Down_Down = Left_Down = Right_Down = A_Down = B_Down = X_Down = Y_Down = false;
}

void Controller::HandleTouchControls() {
    int touchCount = GetTouchPointCount();
    bool buttonATouched = false;
    bool buttonBTouched = false;
    bool buttonXTouched = false;
    bool buttonYTouched = false;
    bool buttonUpTouched = false;
    bool buttonDownTouched = false;
    bool buttonRightTouched = false;
    bool buttonLeftTouched = false;

    for (int i = 0; i < touchCount; i++) {
        Vector2 touchPosition = { (int(GetTouchPosition(i).x) / ScreenScale) - (ScreenOffset.x / ScreenScale),
                                  (int(GetTouchPosition(i).y) / ScreenScale) - (ScreenOffset.y / ScreenScale) };

        if (CheckCollisionPointRec(touchPosition, buttonA)) {
            buttonATouched = true;
            if (!A_Pressed_Flag) {
                A_Pressed = true;
                A_Pressed_Flag = true;
            }
            if (buttonATimer >= longPressThreshold) {
                A_Down = true;
            }
        }
        if (CheckCollisionPointRec(touchPosition, buttonB)) {
            buttonBTouched = true;
            if (!B_Pressed_Flag) {
                B_Pressed = true;
                B_Pressed_Flag = true;
            }
            if (buttonBTimer >= longPressThreshold) {
                B_Down = true;
            }
        }
        if (CheckCollisionPointRec(touchPosition, buttonX)) {
            buttonXTouched = true;
            if (!X_Pressed_Flag) {
                X_Pressed = true;
                X_Pressed_Flag = true;
            }
            if (buttonXTimer >= longPressThreshold) {
                X_Down = true;
            }
        }
        if (CheckCollisionPointRec(touchPosition, buttonY)) {
            buttonYTouched = true;
            if (!Y_Pressed_Flag) {
                Y_Pressed = true;
                Y_Pressed_Flag = true;
            }
            if (buttonYTimer >= longPressThreshold) {
                Y_Down = true;
            }
        }
        if (CheckCollisionPointRec(touchPosition, buttonUp)) {
            buttonUpTouched = true;
            if (!Up_Pressed_Flag) {
                Up_Pressed = true;
                Up_Pressed_Flag = true;
            }
            if (buttonUpTimer >= longPressThreshold) {
                Up_Down = true;
            }
        }
        if (CheckCollisionPointRec(touchPosition, buttonDown)) {
            buttonDownTouched = true;
            if (!Down_Pressed_Flag) {
                Down_Pressed = true;
                Down_Pressed_Flag = true;
            }
            if (buttonDownTimer >= longPressThreshold) {
                Down_Down = true;
            }
        }
        if (CheckCollisionPointRec(touchPosition, buttonLeft)) {
            buttonLeftTouched = true;
            if (!Left_Pressed_Flag) {
                Left_Pressed = true;
                Left_Pressed_Flag = true;
            }
            if (buttonLeftTimer >= longPressThreshold) {
                Left_Down = true;
            }
        }
        if (CheckCollisionPointRec(touchPosition, buttonRight)) {
            buttonRightTouched = true;
            if (!Right_Pressed_Flag) {
                Right_Pressed = true;
                Right_Pressed_Flag = true;
            }
            if (buttonRightTimer >= longPressThreshold) {
                Right_Down = true;
            }
        }
    }

    UpdateTouchTimers(buttonATouched, buttonATimer, A_Pressed_Flag);
    UpdateTouchTimers(buttonBTouched, buttonBTimer, B_Pressed_Flag);
    UpdateTouchTimers(buttonXTouched, buttonXTimer, X_Pressed_Flag);
    UpdateTouchTimers(buttonYTouched, buttonYTimer, Y_Pressed_Flag);
    UpdateTouchTimers(buttonUpTouched, buttonUpTimer, Up_Pressed_Flag);
    UpdateTouchTimers(buttonDownTouched, buttonDownTimer, Down_Pressed_Flag);
    UpdateTouchTimers(buttonLeftTouched, buttonLeftTimer, Left_Pressed_Flag);
    UpdateTouchTimers(buttonRightTouched, buttonRightTimer, Right_Pressed_Flag);
}

void Controller::UpdateTouchTimers(bool& buttonTouched, float& buttonTimer, bool& pressedFlag) {
    if (buttonTouched) {
        buttonTimer += GetFrameTime();
    } else {
        buttonTimer = 0.0f;
        pressedFlag = false;
    }
}

void Controller::Draw() {
    Color transparent = {255,255,255,120};
    if (TouchControllerON) {
        DrawTextureRec(TouchLayout,{0,0,25,25},{buttonA.x,buttonA.y},transparent);
        DrawTextureRec(TouchLayout,{0,25,25,25},{buttonB.x,buttonB.y},transparent);
        DrawTextureRec(TouchLayout,{0,50,25,25},{buttonX.x,buttonX.y},transparent);
        DrawTextureRec(TouchLayout,{0,75,25,25},{buttonY.x,buttonY.y},transparent);
        DrawTextureRec(TouchLayout,{25,0,25,25},{buttonLeft.x,buttonLeft.y},transparent);
        DrawTextureRec(TouchLayout,{25,25,25,25},{buttonDown.x,buttonDown.y},transparent);
        DrawTextureRec(TouchLayout,{25,50,25,25},{buttonRight.x,buttonRight.y},transparent);
        DrawTextureRec(TouchLayout,{25,75,25,25},{buttonUp.x,buttonUp.y},transparent);
    }
}
