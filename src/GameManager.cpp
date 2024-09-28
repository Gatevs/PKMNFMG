#include "GameManager.h"
#include <iostream>
#include "GameObject.h"
#include "TileObjects.h"
#include "player.h"
#include "npc.h"
#include "ActionHandler.h"
#include <cstdlib>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include "TileMap.h"
#include "raylib.h"
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <cstdlib>
#include "ControllerSingleton.h"

GameManager::GameManager(){
    EntityGrowth = LoadSound("assets/SFX/GROWTH.ogg");
    ShadowCentered = LoadTexture("assets/MISC/Shadow_0.png");
    ShadowOffCenter = LoadTexture("assets/MISC/Shadow_1.png");
}

GameManager::~GameManager(){
    scaleFactor = MIN(GetScreenWidth() / gameHeight, GetScreenHeight() / gameHeight);
}

void GameManager::DebugIntro(){
    if (GetTouchPointCount() > 0 || IsKeyPressed(KEY_U)){
        ControllerSingleton::GetInstance().EnableTouchController(true);
        std::cout << "Touch Controller activated" << std::endl;
    }

    if (ControllerSingleton::GetInstance().IsRightPressed() && DebugID < 2){
        SelectPos.x += 128;
        DebugID = 2;
    }
    if (ControllerSingleton::GetInstance().IsLeftPressed() && DebugID > 1){
        SelectPos.x -= 128;
        DebugID = 1;
    }
    if (ControllerSingleton::GetInstance().IsAPressed() || IsKeyPressed(KEY_ENTER)){
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

void GameManager::GameInitialization(std::string map){
    Outside.loadLDtkMap("assets/TILEMAPS/Outside.ldtk",[&]() {
        Outside.initialize(map);
        Outside.loadPlayer(player);
        Outside.loadNPCs(Outside.GetCurLevelName(),player, npcs, true);
        Outside.loadTileObjs(Outside.GetCurLevelName(), tileObjs);
    });
    // Outside.loadWarps(warps);
    JsonLoadNPCData();
    SetMasterVolume(0.9f);

    player.parseCSV("assets/CSV/NPC_OW_DEF.csv");
    Menu.SetPlayerName(player.GetPlayerName());

    for (auto& npc : npcs) {
        npc.parseCSV("assets/CSV/Dataset.csv");
        npc.parseCSV("assets/CSV/NPC_OW_DEF.csv");
        npc.SetShadow(ShadowCentered,ShadowOffCenter);
    }

    //Only at initialization
    player.SetShadow(ShadowCentered);
    player.UpdatePositionAndCamera();
    targetPos = {(floor((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32), floor((player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f))};
    if (Outside.IsCameraLockNear(player)){
        lockCamera = true;
        targetPos = {Outside.InitLockDirection(player).x,targetPos.y};
    }else{
        lockCamera = false;
    }
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    Menu.SetFadePos({targetPos.x - 32, targetPos.y});
    Menu.SetFade(255);
    WarpingPlayer = 2;
}

void GameManager::CameraUpdate(){
    if (!IntroFinished){
        targetPos = {static_cast<float>(+32), static_cast<float>(0)};
    }else if (lockCamera){
        if (Outside.GetLockCameraAxis().x == 1 && Outside.GetLockCameraAxis().y != 1){
            targetPos.x = targetPos.x;
            if (Outside.GetLockCameraAxis().y != 1){
                targetPos.y = float((player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f));
            }
        }
        if(Outside.GetLockCameraAxis().y == 1 && Outside.GetLockCameraAxis().x != 1){
            targetPos.y = targetPos.y;
            if (Outside.GetLockCameraAxis().x != 1){
                targetPos.x = (float((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32);
            }
        }
        if(Outside.GetLockCameraAxis().y == 1 && Outside.GetLockCameraAxis().x == 1){
            targetPos.y = targetPos.y;
            targetPos.x = targetPos.x;
        }
    }else{
        targetPos = {(float((player.GetPosition().x - (256 / 2.0f)) + (32 / 2.0f)) + 32), float((player.GetPosition().y- (192 / 2.0f)) + (32/ 2.0f))};
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
    Outside.update(Outside.GetCurLevelName(), cur, player.GetPosition());
    // std::cout << Menu.stopPlayerInput << std::endl;
    if (WarpingPlayer == 1){
        WarpPlayer(Outside.IndoorWarpTo(player));
    } else if (WarpingPlayer == 2){
        if(!Menu.IsFadeOutComplete()){
            Menu.fadeOut();
        }else{
            Menu.SetFade(-1);
            WarpingPlayer = 0;
        }
    }
    if (player.InvokeUIElement() != NONE && !Menu.stopPlayerInput){
        int npcIdInFront = player.CheckForNPCInFront(npcs);
        switch (player.InvokeUIElement()){
            case PAUSE:
                Menu.handleAction(ActionType::Pause_M,camera.target);
                break;
            case DIALOGUE:
                if (npcIdInFront != -1){
                    for (auto& npc : npcs) {
                        if (npc.GetID() == npcIdInFront && !npc.IsNPCGrowing()) {
                            npc.GetCombinedValues(1);
                            npc.lookAtPlayer(player.GetPlayerDir());
                            if (!Menu.stopPlayerInput){
                                Menu.getNPCInfo(npc.GetID(),npcs, 1);
                                Menu.handleAction(ActionType::Dialogue_Box,camera.target);
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
                    Menu.handleAction(ActionType::Action_M,camera.target);
                }
                if (npcIdInFront != -1) {
                    for (auto& npc : npcs) {
                        if (npc.GetID() == npcIdInFront && !npc.IsNPCGrowing()) {
                            npc.GetCombinedValues(1);
                            if (!Menu.stopPlayerInput){
                                Menu.getNPCInfo(npc.GetID(),npcs, 1);
                                Menu.handleAction(ActionType::Action_M,camera.target);
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
    if (!Menu.stopPlayerInput && WarpingPlayer == 0){
        player.HandleInput(npcs);
    } else if (Menu.stopPlayerInput){
        Menu.InputUI(npcs, player);
    }
    player.Update();
    if (player.IsPlayerMoving()){
        if (Outside.IsCameraLockNear(player)){
            lockCamera = true;
        } else{
            lockCamera = false;
        }
        if (Outside.IndoorWarpTo(player) != "NULL"){WarpingPlayer = true;}
        Outside.IsWarpClose(player);
        Outside.PlayerInTallGrass(player);
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
            Outside.loadNPCs(Outside.GetSwapLevelName(),player,npcs,false);
            for (auto& npc : npcs) {
                if (npc.GetLocation() == Outside.GetSwapLevelName()){
                    npc.parseCSV("assets/CSV/Dataset.csv");
                    npc.parseCSV("assets/CSV/NPC_OW_DEF.csv");
                    npc.SetShadow(ShadowCentered,ShadowOffCenter);
                }
            }
            Outside.SetDrawNextLevel();
        }else{
            Outside.update(Outside.GetSwapLevelName(), swapper, player.GetPosition());
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
        Outside.DrawGrass(player,tileObjs);
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
    ClearBackground(BLACK);
    BeginMode2D(camera);
    {
        DrawTexture(IntroBG,0,0,WHITE);
        DrawTextureRec(IntroSelect,Select, SelectPos,WHITE);
        DrawTextEx(BagFont,"DEBUG",(Vector2){15,148},BagFont.baseSize, -5,WHITE);
        DrawTextEx(BagFont,"Select character to test.",(Vector2){15,160},BagFont.baseSize, -5,WHITE);
    }
    EndMode2D();
}

void GameManager::JsonSaveNPCData() {
    using json = nlohmann::json;

    // Read existing JSON data from file
    std::ifstream inFile("npc_data.json");
    json existingData;
    if (inFile.is_open()) {
        inFile >> existingData;
        inFile.close();
    }

    // Create or update NPCs array in the existing JSON data
    json npcArray;
    if (existingData.contains("NPCs")) {
        npcArray = existingData["NPCs"];
    }

    // Iterate through NPCs and update existing or add new entries
    for (auto& npc : npcs) {
        bool found = false;
        for (auto& npcData : npcArray) {
            if (npcData["ID"] == npc.GetID()) {
                // Update existing NPC entry
                npcData["Location"] = Outside.GetCurLevelName();
                npcData["Pos_X"] = npc.GetPosition().x;
                npcData["Pos_Y"] = npc.GetPosition().y;
                npcData["Stage"] = npc.GetStage();
                npcData["Follower"] = (npc.GetID() == player.GetPlayerFollower()) ? "Yes" : "No";
                npcData["IdleAnim"] = (npc.GetIdleAnim()) ? "Yes" : "No";
                npcData["TimesGrown"] = npc.GetNPCEventState().timesGrown;
                found = true;
                break;
            }
        }
        if (!found) {
            // Add new NPC entry
            json npcObject;
            npcObject["ID"] = npc.GetID();
            npcObject["Location"] = Outside.GetCurLevelName();
            npcObject["Pos_X"] = npc.GetPosition().x;
            npcObject["Pos_Y"] = npc.GetPosition().y;
            npcObject["Stage"] = npc.GetStage();
            npcObject["Follower"] = (npc.GetID() == player.GetPlayerFollower()) ? "Yes" : "No";
            npcObject["IdleAnim"] = (npc.GetIdleAnim()) ? "Yes" : "No";
            npcObject["TimesGrown"] = npc.GetNPCEventState().timesGrown;
            npcArray.push_back(npcObject);
        }
    }

    // Update existing JSON data with the updated NPCs array
    existingData["NPCs"] = npcArray;

    // Write updated JSON data back to the file
    std::ofstream outFile("npc_data.json");
    outFile << existingData.dump(4); // Pretty-print JSON with indentation of 4 spaces
    outFile.close();

    std::cout << "NPC data saved to npc_data.json" << std::endl;
}

void GameManager::JsonLoadNPCData() {
    using json = nlohmann::json;

    std::ifstream inFile("npc_data.json");
    if (!inFile.is_open()) {
        std::cerr << "INFO: Temporary file npc_data.json doesn't exist yet." << std::endl;
        return;
    }

    json npcData;
    inFile >> npcData;
    inFile.close();

    json npcArray = npcData["NPCs"];

    for (const auto& npcObject : npcArray) {
        int npcID = npcObject["ID"];
        Vector2 npcPos = {npcObject["Pos_X"], npcObject["Pos_Y"]};

        // Check if NPC already exists
        auto existingNPC = std::find_if(npcs.begin(), npcs.end(), [npcID](const NPC& npc) { return npc.GetID() == npcID; });
        if (existingNPC != npcs.end()) {
            // Update NPC data if in the same location
            if (npcObject["Location"] == Outside.GetCurLevelName()) {
                existingNPC->SetPosition(npcPos);
                existingNPC->SetStage(npcObject["Stage"]);
                if (npcObject["IdleAnim"] == "Yes") {
                    existingNPC->updateTexture("idle");
                    existingNPC->updateAnimationFrameDimensions("idle");
                }
            }
            // Remove NPC if not in the current location and not a follower
            else if (npcObject["Location"] != Outside.GetCurLevelName() && npcObject["Follower"] != "Yes") {
                npcs.erase(existingNPC);
            }
            // Update position and stage if NPC is a follower but not in the current location
            else if (npcObject["Location"] != Outside.GetCurLevelName() && npcObject["Follower"] == "Yes") {
                existingNPC->SetPosition(npcPos);
                existingNPC->SetStage(npcObject["Stage"]);
                if (npcObject["IdleAnim"] == "Yes") {
                    existingNPC->updateTexture("idle");
                    existingNPC->updateAnimationFrameDimensions("idle");
                }
            }
        } else {
            // Add new NPC if it doesn't exist and is in the current location
            if (npcObject["Location"] == Outside.GetCurLevelName()) {
                NPC newNPC(npcID, Outside.GetCurLevelName(), npcPos);
                newNPC.SetStage(npcObject["Stage"]);
                if (npcObject["IdleAnim"] == "Yes") {
                    newNPC.updateTexture("idle");
                    newNPC.updateAnimationFrameDimensions("idle");
                }
                npcs.push_back(newNPC);
            }
        }
    }
}


void GameManager::WarpPlayer(std::string where){
    Menu.SetFadePos({camera.target.x - 32, camera.target.y});
    Menu.fadeIn();
    if (Menu.IsFadeInComplete()){
        lockCamera = false;
        JsonSaveNPCData();
        npcs.clear();
        tileObjs.clear();
        Outside.Unload();
        player.StopPlayer();
        GameInitialization(where);
        WarpingPlayer = 2;
    }
}

void GameManager::Unload(){
    Outside.Unload();
    if (std::remove("npc_data.json") != 0) {
        std::cerr << "Error deleting file npc_data.json" << std::endl;
    } else {
        std::cout << "npc_data.json deleted successfully" << std::endl;
    }

}

void GameManager::DrawOutsideItems(){
    int actualGameWidth = gameHeight * scaleFactor;
    int actualGameHeight = gameHeight * scaleFactor;
    // Calculate the offset to center the game in the window
    int offsetX = (GetScreenWidth() - actualGameWidth) / 2;
    int offsetY = (GetScreenHeight() - actualGameHeight) / 2;
    bigCamera.zoom = scaleFactor;
    bigCamera.target = (Vector2){0,0};
    bigCamera.offset = (Vector2){ (float)offsetX, (float)offsetY };
    ControllerSingleton::GetInstance().SetOffset(bigCamera.offset);
    ControllerSingleton::GetInstance().SetScale(scaleFactor);
    BeginMode2D(bigCamera);
    {
        Outside.DrawLocationCard();
        ControllerSingleton::GetInstance().Draw();
    }
    EndMode2D();
}
