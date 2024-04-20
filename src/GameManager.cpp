#include "GameManager.h"
#include <iostream>
#include "GameObject.h"
#include "TileObjects.h"
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
    EntityGrowth = LoadSound("assets/SFX/GROWTH.ogg");
}

GameManager::~GameManager(){
    scaleFactor = MIN(GetScreenWidth() / gameHeight, GetScreenHeight() / gameHeight);
}
void GameManager::DebugIntro(){
    if (IsKeyPressed(KEY_RIGHT) && DebugID < 2){
        SelectPos.x += 128;
        DebugID = 2;
    }
    if (IsKeyPressed(KEY_LEFT) && DebugID > 1){
        SelectPos.x -= 128;
        DebugID = 1;
    }
    if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)){
        if (DebugID == 1){
            player.SetPlayerGender("PlayerM");
        } else{
            player.SetPlayerGender("PlayerF");
        }
        UnloadTexture(IntroBG);
        UnloadTexture(IntroSelect);
        Menu.SetPlayerGender(player.GetPlayerGender());
        IntroFinished = true;
    }
}

void GameManager::GameInitialization(){
    Outside.loadLDtkMap("assets/Outside.ldtk",[&]() {
            Outside.initialize("Swolie_Town");
            Outside.loadPlayer(player);
            Outside.loadNPCs(npcs);
            Outside.loadTileObjs(Outside.GetCurLevelName(), tileObjs);
        });
    // Outside.loadWarps(warps);
    SetMasterVolume(0.9f);
    ShadowCentered = LoadTexture("assets/Shadow_0.png");
    ShadowOffCenter = LoadTexture("assets/Shadow_1.png");

    player.parseCSV("assets/NPC_OW_DEF.csv");
    Menu.SetPlayerName(player.GetPlayerName());

    for (auto& npc : npcs) {
        npc.parseCSV("assets/Dataset.csv");
        npc.parseCSV("assets/NPC_OW_DEF.csv");
        npc.SetShadow(ShadowCentered,ShadowOffCenter);
    }

    //Only at initialization
    player.SetShadow(ShadowCentered);
    player.UpdatePositionAndCamera();
    camera.target = (Vector2){((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32, (player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f)};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void GameManager::CameraUpdate(){
    if (!IntroFinished){
        targetPos = {static_cast<float>(+32), static_cast<float>(0)};
    }else if (lockCamera){
        if (Outside.GetLockCameraAxis() == 1){
            targetPos = {camera.target.x, floor((player.GetPosition().y - (192 / 2.0f)) + (32/ 2.0f))};
        } else if(Outside.GetLockCameraAxis() == 2){
            targetPos = {(floor((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32), camera.target.y};
        }
    }else{
        targetPos = {(floor((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32), floor((player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f))};
    }
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
    camera.target = targetPos;
}

void GameManager::GameLoop(){
    Outside.update(Outside.GetCurLevelName(), cur, player);
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
                if (player.GetPlayerGender() == "PlayerF" && npcIdInFront == -1){
                    Menu.SetInteractionID(1);
                    Menu.getPlayerInfo(1,player,1);
                    Menu.handleAction(ActionType::Action_M,player.GetPosition());
                }
                if (npcIdInFront != -1) {
                    for (auto& npc : npcs) {
                        if (npc.GetID() == npcIdInFront && !npc.IsNPCGrowing()) {
                            npc.GetCombinedValues(1);
                            if (!Menu.stopPlayerInput){
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
        if (Outside.IsCameraLockNear(player)){
            lockCamera = true;
        } else{
            lockCamera = false;
        }
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
        if (npc.IsNPCGrowing() && !IsSoundPlaying(EntityGrowth)){
            PlaySound(EntityGrowth);
        }
    }

    if (Outside.StandingOnWarp()){
        Outside.SwapLevels(player);
    }
    if (Outside.IsNextLevelLoaded()){
        Outside.unloadFarAwayLevel(player,tileObjs);
        if (!Outside.GetDrawNextLevel()){
            Outside.loadTileObjs(Outside.GetSwapLevelName(), tileObjs);
            Outside.SetDrawNextLevel();
        }else{
            Outside.update(Outside.GetSwapLevelName(), swapper, player);
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

void GameManager::DrawIntro(){
    ClearBackground(green);
    BeginMode2D(camera);
    {
        DrawTexture(IntroBG,0,0,WHITE);
        DrawTextureRec(IntroSelect,Select, SelectPos,WHITE);
        DrawTextEx(BagFont,"DEBUG",(Vector2){15,148},BagFont.baseSize, -5,WHITE);
        DrawTextEx(BagFont,"Select character to test.",(Vector2){15,160},BagFont.baseSize, -5,WHITE);
    }
    EndMode2D();
}

void GameManager::Unload(){
    Outside.Unload();
}

void GameManager::DrawVN(){
    int scaleFactorB = MIN(GetScreenWidth() / gameWidth, GetScreenHeight() / gameHeight);
    int actualGameWidth = gameHeight * scaleFactor;
    int actualGameHeight = gameHeight * scaleFactor;
    // Calculate the offset to center the game in the window
    int offsetX = (GetScreenWidth() - actualGameWidth) / 2;
    int offsetY = (GetScreenHeight() - actualGameHeight) / 2;
    SetTextureFilter(VNTest,TEXTURE_FILTER_BILINEAR);
    bigCamera.zoom = 0.2 * scaleFactorB;
    bigCamera.target = (Vector2){0,0};
    bigCamera.offset = (Vector2){ (float)offsetX, (float)offsetY };
    BeginMode2D(bigCamera);
    {
        DrawTexture(VNTest,(VNTest.width / 2.25f) - (VNTest.width / 2.0f),(VNTest.height / 2.0f) - (VNTest.height / 2.0f),WHITE);
    }
}
