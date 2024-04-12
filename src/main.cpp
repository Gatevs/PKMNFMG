#include <raylib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "GameManager.h"

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Global variables

// Update and Draw one frame
void UpdateDrawFrame(GameManager& game) {
    // Update
    if (game.IsIntroFinished()){
        game.GameLoop();
        game.CameraUpdate();

        // Draw
        BeginDrawing();
        {
            game.DrawWorld();
            game.DrawBars();
            if (game.IsVNMenu()){
                game.DrawVN();
            }
        }
        EndDrawing();
    } else{
        game.DebugIntro();
        game.CameraUpdate();

        BeginDrawing();
        {
            game.DrawIntro();
            game.DrawBars();
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
    Game.GameInitialization();

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
