#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "texture.h"
#include "camera.h"

class GameObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;
    bool        IsSolid;
    bool        Destroyed;
    // render state
    Texture2D   Sprite;	
    // constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
};

class BallObject : public GameObject
{
public:
    // ball state	
    float   Radius;
    bool IsCollide;
    // constructor(s)
    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
    glm::vec2 Move(float dt, unsigned int window_width, unsigned int window_height);
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif