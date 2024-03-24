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
    game.GameLoop();
    game.CameraUpdate();

    // Draw
    BeginDrawing();
    {
       game.DrawWorld();
       game.DrawBars();
    }
    EndDrawing();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(256, 192, "PokemonFMG");
    SetWindowMinSize(256, 192);
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
    CloseWindow();
    return 0;
}
