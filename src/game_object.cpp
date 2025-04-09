#include "game_object.h"

GameObject::GameObject() 
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity) 
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

BallObject::BallObject()
    : GameObject(), Radius(12.5f), IsCollide(false) { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), IsCollide(false) { }

glm::vec2 BallObject::Move(float dt, unsigned int window_width,unsigned int window_height)
{
    this->IsCollide = false;
    this->Position += this->Velocity * dt;
    // then check if outside window bounds and if so, reverse velocity and restore at correct position
    if (this->Position.x <= 0.0f)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = 0.0f;
        this->IsCollide = true;
    }
    else if (this->Position.x + this->Size.x >= window_width)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = window_width - this->Size.x;
        this->IsCollide = true;
    }
    if (this->Position.y <= 0.0f)
    {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = 0.0f;
        this->IsCollide = true;
    }
    else if (this->Position.y + this->Size.y >= window_height)
    {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = window_height - this->Size.y;
        this->IsCollide = true;
    }
    return this->Position;
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
}