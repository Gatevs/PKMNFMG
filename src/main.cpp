#include <raylib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "GameManager.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(256,192,"Pokemon FMG");
    SetWindowMinSize(256, 192);

    GameManager Game;
    Game.GameInitialization();

    //gameloop
    while (WindowShouldClose() == false)
    {
        Game.GameLoop();
        Game.CameraUpdate();
        // Draw
        BeginDrawing();
        {
           Game.DrawWorld();
           Game.DrawBars();

        }
        EndDrawing();
    }
    Game.Unload();
    CloseWindow();
    return 0;
}
