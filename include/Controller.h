// Controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "raylib.h"

class Controller {
public:
    Controller();
    ~Controller();

    void Update();
    void Draw();

    bool IsUpPressed() const { return Up_Pressed; }
    bool IsDownPressed() const { return Down_Pressed; }
    bool IsLeftPressed() const { return Left_Pressed; }
    bool IsRightPressed() const { return Right_Pressed; }
    bool IsAPressed() const { return A_Pressed; }
    bool IsXPressed() const { return X_Pressed; }
    bool IsYPressed() const { return Y_Pressed; }
    bool IsBPressed() const { return B_Pressed; }

    bool IsUpHeld() const { return Up_Down; }
    bool IsDownHeld() const { return Down_Down; }
    bool IsLeftHeld() const { return Left_Down; }
    bool IsRightHeld() const { return Right_Down; }
    bool IsAHeld() const { return A_Down; }
    bool IsBHeld() const { return B_Down; }
    bool IsXHeld() const { return X_Down; }
    bool IsYHeld() const { return Y_Down; }

    bool IsTouchEnabled() const { return TouchControllerON;}

    void EnableTouchController(bool enable) { TouchControllerON = enable; }
    void SetOffset(Vector2 offset) { ScreenOffset = offset; }
    void SetScale(int scale) { ScreenScale = scale; }

private:
    void ResetFlags();
    void HandleTouchControls();
    void UpdateTouchTimers(bool& buttonTouched, float& buttonTimer, bool& pressedFlag);

    bool Up_Pressed = false, Down_Pressed = false, Left_Pressed = false, Right_Pressed = false;
    bool A_Pressed = false, B_Pressed = false, X_Pressed = false, Y_Pressed = false;
    bool Up_Down = false, Down_Down = false, Left_Down = false, Right_Down = false;
    bool A_Down = false, B_Down = false, X_Down = false, Y_Down = false;
    bool Up_Pressed_Flag = false, Down_Pressed_Flag = false, Left_Pressed_Flag = false, Right_Pressed_Flag = false;
    bool A_Pressed_Flag = false, B_Pressed_Flag = false, X_Pressed_Flag = false, Y_Pressed_Flag = false;

    bool TouchControllerON = false;
    Vector2 ScreenOffset = {0, 0};
    float ScreenScale = 1.0f;

    static constexpr float longPressThreshold = 0.05f; // Threshold for a long press in seconds

    Rectangle buttonA = {190, 140, 25, 25};
    Rectangle buttonB = {165, 165, 25, 25};
    Rectangle buttonX = {165, 115, 25, 25};
    Rectangle buttonY = {140, 140, 25, 25};
    Rectangle buttonRight = {25, 140, 25, 25};
    Rectangle buttonDown = {0, 165, 25, 25};
    Rectangle buttonUp = {0, 115, 25, 25};
    Rectangle buttonLeft = {-25, 140, 25, 25};

    float buttonATimer = 0.0f;
    float buttonBTimer = 0.0f;
    float buttonXTimer = 0.0f;
    float buttonYTimer = 0.0f;
    float buttonUpTimer = 0.0f;
    float buttonDownTimer = 0.0f;
    float buttonLeftTimer = 0.0f;
    float buttonRightTimer = 0.0f;

    Texture2D TouchLayout;
};

#endif // CONTROLLER_H
