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
    float GetYPosition() const override { return position.y; }
    std::string GetLocation() const {return location;}

private:
    int ID;
    Vector2 position;
    Rectangle ObjSprite;
    Texture2D Texture;
    std::string location;
};
