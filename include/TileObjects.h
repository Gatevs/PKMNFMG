#pragma once

#include <raylib.h>
#include "GameObject.h"
#include <LDtkLoader/Project.hpp>
#include <string>
#include <vector>

class tileObj :public GameObject {
public:
    tileObj(int objID, std::string loc, Vector2 objPos, ldtk::IntRect rec, Texture2D objTexture);
    ~tileObj();

    void Draw() override;
    float GetYPosition() const override { return position.y + offset; }
    std::string GetLocation() const {return location;}
    int GetID() const {return ID;}
    void SetPosition(Vector2 pos){position = pos;}
    void ResetGrass();
    void SetOffet(int value){offset = value;}
    void ResetFrames(int value){GrassFrame = value; grassAnimFinished = false;}
    void AnimateGrass();
    bool GetGrassAnim() const {return grassAnimFinished;}
    int GetOffset() const {return GrassFrame;}

private:
    int ID;
    Vector2 position;
    Rectangle ObjSprite;
    Texture2D Texture;
    std::string location;
    int offset = 1;
    float GrassTimer = 0.0f;
    int GrassFrame = 0;
    bool grassAnimFinished = false;
};
