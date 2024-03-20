// In GameObject.h

#pragma once

#include <raylib.h>

class GameObject {
    public:
        Texture2D texture;
        Vector2 position;

        virtual void Draw() = 0; // Pure virtual function
        virtual float GetYPosition() const = 0; // Pure virtual function
        // Other common properties and methods
};
