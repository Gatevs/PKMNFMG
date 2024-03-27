#include "TileObjects.h"
#include "raylib.h"

tileObj::tileObj(int objID, Vector2 objPos, ldtk::IntRect rec, Texture2D objTexture){
    ID = objID;
    position = objPos;
    ObjSprite = (Rectangle){static_cast<float>(rec.x),static_cast<float>(rec.y),static_cast<float>(rec.width),static_cast<float>(rec.height)};
    Texture = objTexture;
}

tileObj::~tileObj() {

}

void tileObj::Draw(){
    DrawTextureRec(Texture,ObjSprite,position,WHITE);
}
