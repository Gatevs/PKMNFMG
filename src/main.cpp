#include <raylib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "GameManager.h"
#include "ControllerSingleton.h"

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Global variables
const float TARGET_FPS = 60.0f;
const float FIXED_TIMESTEP = 1.0f / TARGET_FPS;
float accumulator = 0.0f;

// Update and Draw one frame
void UpdateDrawFrame(GameManager& game) {
    float dt = GetFrameTime();
    accumulator += dt;

    ControllerSingleton::GetInstance().Update(); // Update controller inputs

    // Update
    if (game.IsIntroFinished()){
        while (accumulator >= FIXED_TIMESTEP) {
            game.GameLoop(FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
        }
        game.CameraUpdate();

        // Draw
        BeginDrawing();
        {
            game.DrawWorld();
            game.DrawOutsideItems();
            game.DrawBars();

        }
        EndDrawing();
    } else{
        game.DebugIntro();
        game.CameraUpdate();

        BeginDrawing();
        {
            game.DrawIntro();
            game.DrawBars();
            if (ControllerSingleton::GetInstance().IsTouchEnabled()) { game.DrawOutsideItems(); }
        }
        EndDrawing();
    }
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(768, 576, "Pokemon FMG");
    SetWindowMinSize(256, 192);
    InitAudioDevice();
    GameManager Game;
    Game.GameInitialization("Player_Home");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg((em_arg_callback_func)UpdateDrawFrame, &Game, 0, 1);
#else
    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose()) {
        UpdateDrawFrame(Game);
    }
#endif

    Game.Unload();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
