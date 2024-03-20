#include "GameManager.h"
#include <iostream>
#include "GameObject.h"
#include "player.h"
#include "npc.h"
#include "ActionHandler.h"
#include <string>
#include <vector>
#include <algorithm>
#include "TileMap.h"

GameManager::~GameManager(){
    scaleFactor = MIN(GetScreenWidth() / gameHeight, GetScreenHeight() / gameHeight);
}

void GameManager::GameInitialization(){
    Outside.loadLDtkMap("assets/Outside.ldtk");


    // Load UI Atlas

    Outside.initialize("Level_0");
    Outside.loadPlayer(player);
    Outside.loadNPCs(npcs);
    ShadowCentered = LoadTexture("assets/Shadow_0.png");
    ShadowOffCenter = LoadTexture("assets/Shadow_1.png");

    for (auto& npc : npcs) {
        npc.parseCSV("assets/Dataset.csv");
        npc.parseCSV("assets/NPC_OW_DEF.csv");
        npc.GetShadow(ShadowCentered,ShadowOffCenter);
    }

    //Only at initialization
    player.GetShadow(ShadowCentered);
    player.UpdatePositionAndCamera(camera, npcs);
    camera.target = (Vector2){((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32, (player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f)};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);
}

void GameManager::CameraUpdate(){
    // Set the camera zoom and offset
    scaleFactor = MIN(GetScreenWidth() / gameHeight, GetScreenHeight() / gameHeight);
    // Calculate the actual game width and height after applying scaleFactor
    int actualGameWidth = gameHeight * scaleFactor;
    int actualGameHeight = gameHeight * scaleFactor;
    // Calculate the offset to center the game in the window
    int offsetX = (GetScreenWidth() - actualGameWidth) / 2;
    int offsetY = (GetScreenHeight() - actualGameHeight) / 2;
    camera.zoom = scaleFactor;
    camera.offset = (Vector2){ (float)offsetX, (float)offsetY };
}

void GameManager::GameLoop(){
    // Tile animations
    Outside.update();

    // std::cout << Menu.stopPlayerInput << std::endl;

    // Handle player input and update player and npc state
    if (!Menu.stopPlayerInput){
        player.HandleInput(npcs, Menu);
    } else{
        Menu.InputUI();
    }
    player.Update();
    if (player.IsPlayerMoving()){
        player.checkCollisions(Outside.GetCOL());
        player.UpdateAnim();
        player.UpdatePositionAndCamera(camera, npcs);
        if (player.GetPlayerFollower() != 0){
            player.npcMoving(npcs);
        }
    }
    for (auto& npc : npcs) {
        npc.Update();
    }
}

void GameManager::DrawWorld(){
    ClearBackground(green);
    BeginMode2D(camera);
    {
        Outside.draw();
        // Create a vector to hold pointers to GameObjects
        std::vector<GameObject*> objects;

        // Add player and NPC objects to the vector
        objects.push_back(&player);
        for (auto& npc : npcs) {
            objects.push_back(&npc);
        }

        // Sorting objects based on Y position
        std::sort(objects.begin(), objects.end(), [](const GameObject* a, const GameObject* b) {
            return a->GetYPosition() < b->GetYPosition();
        });

        // Drawing loop
        for (const auto& obj : objects) {
            obj->Draw(); // Draw each object using polymorphism
        }
        Menu.Draw();
    }
    EndMode2D();
}

void GameManager::DrawBars(){
    if (scaleFactor > 1) {
        DrawRectangle(0, 0, (GetScreenWidth() - gameWidth * scaleFactor) / 2, GetScreenHeight(), BLACK); //Left Bar
        DrawRectangle((GetScreenWidth() + gameWidth * scaleFactor) / 2, 0, (GetScreenWidth() - gameWidth * scaleFactor) / 2, GetScreenHeight(), BLACK); //Right Bar
        DrawRectangle(0, 0, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2, BLACK); //Top Bar
        DrawRectangle(0, GetScreenHeight() - (GetScreenHeight() - gameHeight * scaleFactor) / 2, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2, BLACK); // Bottom Bar
    }
    else if (scaleFactor <= 1) {
        DrawRectangle(0, 0, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2, BLACK); //Top Bar
        DrawRectangle(0, (GetScreenHeight() + gameHeight * scaleFactor) / 2, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2, BLACK); //Bottom Bar
        DrawRectangle(0, 0, (GetScreenWidth() - gameWidth * scaleFactor) / 2, GetScreenHeight(), BLACK); //Left Bar
        DrawRectangle(GetScreenWidth() - (GetScreenWidth() - gameWidth * scaleFactor) / 2, 0, (GetScreenWidth() - gameWidth * scaleFactor) / 2, GetScreenHeight(), BLACK); //Right Bar
    }
}

void GameManager::Unload(){
    Outside.Unload();
}
