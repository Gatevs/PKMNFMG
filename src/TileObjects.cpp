#include "TileObjects.h"
#include "raylib.h"

tileObj::tileObj(int objID, std::string loc, Vector2 objPos, ldtk::IntRect rec, Texture2D objTexture){
    ID = objID;
    position = objPos;
    ObjSprite = (Rectangle){static_cast<float>(rec.x),static_cast<float>(rec.y),static_cast<float>(rec.width),static_cast<float>(rec.height)};
    Texture = objTexture;
    location = loc;
}

tileObj::~tileObj() {

}

void tileObj::Draw(){
    DrawTextureRec(Texture,ObjSprite,position,WHITE);
}

void tileObj::AnimateGrass(){
    float frameRate = 0.25f;
    GrassTimer += GetFrameTime();
    if (GrassTimer >= frameRate && GrassFrame < 4){
        ObjSprite = (Rectangle){static_cast<float>(16 * (GrassFrame)), 0,16,16};
        GrassFrame += 1;
        GrassTimer = 0.0f;
        grassAnimFinished = false;
    }else if (GrassFrame >= 4){
        grassAnimFinished = true;
    }
}

void tileObj::ResetGrass(){
    if (ID == 2 || ID == 3 || ID == 4){
        position = {0,0};
    }
}
