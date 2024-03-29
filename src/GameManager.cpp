#include "GameManager.h"
#include <iostream>
#include "GameObject.h"
#include "player.h"
#include "npc.h"
#include "ActionHandler.h"
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include "TileMap.h"
#include "raylib.h"

GameManager::GameManager(){

}

GameManager::~GameManager(){
    scaleFactor = MIN(GetScreenWidth() / gameHeight, GetScreenHeight() / gameHeight);
}

void GameManager::GameInitialization(){
    Outside.loadLDtkMap("assets/Outside.ldtk",[&]() {
        std::cout << "Initializing map..." << std::endl;
            Outside.initialize("Route_1");
            std::cout << "Map initialized!" << std::endl;
            Outside.loadPlayer(player);
            Outside.loadNPCs(npcs);
            Outside.loadTileObjs(Outside.GetCurLevelName(), tileObjs);
        });
    // Outside.loadWarps(warps);
    ShadowCentered = LoadTexture("assets/Shadow_0.png");
    ShadowOffCenter = LoadTexture("assets/Shadow_1.png");

    for (auto& npc : npcs) {
        npc.parseCSV("assets/Dataset.csv");
        npc.parseCSV("assets/NPC_OW_DEF.csv");
        npc.SetShadow(ShadowCentered,ShadowOffCenter);
    }

    //Only at initialization
    player.SetShadow(ShadowCentered);
    player.UpdatePositionAndCamera();
    camera.target = (Vector2){((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32, (player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f)};
    cameraHelper = camera.target;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void GameManager::CameraUpdate(){
    Vector2 targetPos = {(floor((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32), floor((player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f))};
    // Set the camera zoom and offset
    scaleFactor = MIN(GetScreenWidth() / gameWidth, GetScreenHeight() / gameHeight);
    // Calculate the actual game width and height after applying scaleFactor
    int actualGameWidth = gameHeight * scaleFactor;
    int actualGameHeight = gameHeight * scaleFactor;
    // Calculate the offset to center the game in the window
    int offsetX = (GetScreenWidth() - actualGameWidth) / 2;
    int offsetY = (GetScreenHeight() - actualGameHeight) / 2;
    camera.zoom = scaleFactor;
    camera.offset = (Vector2){ (float)offsetX, (float)offsetY };
    if (player.IsPlayerMoving()){
        cameraHelper.x += (player.GetPlayerSpeed() * 60) * GetFrameTime();
    }else{
        cameraHelper = (Vector2){((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32, (player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f)};
    }
    camera.target = targetPos;
}

void GameManager::GameLoop(){
    // Tile animations
    Outside.update(Outside.GetCurLevelName(), cur);

    if (IsKeyPressed(KEY_P)){
        unrelated a = cur;
        unrelated b = swapper;
        cur = b;
        swapper = a;
        std::cout << Outside.GetSwapLevelName() << std::endl;
    }

    // std::cout << Menu.stopPlayerInput << std::endl;
    if (player.InvokeUIElement() != NONE && !Menu.stopPlayerInput){
        int npcIdInFront = player.CheckForNPCInFront(npcs);
        switch (player.InvokeUIElement()){
            case PAUSE:
                Menu.handleAction(ActionType::Pause_M,player.GetPosition());
                break;
            case DIALOGUE:
                if (npcIdInFront != -1){
                    for (auto& npc : npcs) {
                        if (npc.GetID() == npcIdInFront && !npc.IsNPCGrowing()) {
                            npc.GetCombinedValues(1);
                            npc.lookAtPlayer(player.GetPlayerDir());
                            if (!Menu.stopPlayerInput){
                                Menu.getNPCInfo(npc.GetID(),npcs, 1);
                                Menu.handleAction(ActionType::Dialogue_Box,player.GetPosition());
                            }
                            break;
                        }
                    }
                } else{
                    player.StopUI_Element();
                }
                break;
            case ACTION:
                if (npcIdInFront != -1) {
                    for (auto& npc : npcs) {
                        if (npc.GetID() == npcIdInFront && !npc.IsNPCGrowing()) {
                            npc.GetCombinedValues(1);
                            if (!Menu.stopPlayerInput){
                                Menu.SetInteractionID(npcIdInFront);
                                Menu.getNPCInfo(npc.GetID(),npcs, 1);
                                Menu.handleAction(ActionType::Action_M,player.GetPosition());
                            }
                            break;
                        }
                    }
                } else {
                    player.StopUI_Element();
                }
                break;
        }
    }
    // Handle player input and update player and npc state
    if (!Menu.stopPlayerInput){
        player.HandleInput(npcs);
    } else{
        Menu.InputUI(npcs, player);
    }
    player.Update();
    if (player.IsPlayerMoving()){
        Outside.IsWarpClose(player);
        player.checkCollisions(Outside.GetCOL(), npcs, Outside.GetlevelOffset());
        player.UpdateAnim();
        player.UpdatePositionAndCamera();
        if (player.GetPlayerFollower() != 0){
            player.npcMoving(npcs);
        }
    }
    for (auto& npc : npcs) {
        npc.Update();
    }

    if (Outside.StandingOnWarp()){
        Outside.SwapLevels(player);
    }
    if (Outside.IsNextLevelLoaded()){
        if (!Outside.GetDrawNextLevel()){
            Outside.loadTileObjs(Outside.GetSwapLevelName(), tileObjs);
            Outside.SetDrawNextLevel();
        }else{
            Outside.update(Outside.GetSwapLevelName(), swapper);
        }
    }
}

void GameManager::DrawWorld(){
    ClearBackground(green);
    BeginMode2D(camera);
    {
        Outside.draw("Current", Outside.GetCurLevel());
        if (Outside.GetDrawNextLevel()){
            Outside.draw("Swap", Outside.GetSwapLevel());
        }
        // Create a vector to hold pointers to GameObjects
        std::vector<GameObject*> objects;

        // Add player and NPC objects to the vector
        objects.push_back(&player);
        for (auto& npc : npcs) {
            objects.push_back(&npc);
        }
        for (auto& tObj : tileObjs) {
            objects.push_back(&tObj);
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
        DrawRectangle(0, GetScreenHeight() - (GetScreenHeight() - gameHeight * scaleFactor) / 2.0f, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2.0f, BLACK); // Bottom Bar
        DrawRectangle(0, GetScreenHeight() - (GetScreenHeight() - (gameHeight + 4) * scaleFactor) / 2.0f, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2.0f, BLACK); // Bottom Bar
    }
    else if (scaleFactor <= 1) {
        DrawRectangle(0, 0, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2, BLACK); //Top Bar
        DrawRectangle(0, (GetScreenHeight() + gameHeight * (scaleFactor)) / 2.0f, GetScreenWidth(), (GetScreenHeight() - gameHeight * scaleFactor) / 2.0f, BLACK); //Bottom Bar
        DrawRectangle(0, 0, (GetScreenWidth() - gameWidth * scaleFactor) / 2, GetScreenHeight(), BLACK); //Left Bar
        DrawRectangle(GetScreenWidth() - (GetScreenWidth() - gameWidth * scaleFactor) / 2, 0, (GetScreenWidth() - gameWidth * scaleFactor) / 2, GetScreenHeight(), BLACK); //Right Bar
    }
}

void GameManager::Unload(){
    Outside.Unload();
}
