#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "texture.h"
#include "game_object.h"
#include <iostream>

// Represents a single particle and its state
struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float     Life;
    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

struct Particle3D
{
    glm::vec3 Position;
    glm::vec4 Velocity, Attractors;
    float Life;
};

class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // update all particles
    void Update(float dt, BallObject&object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // render all particles
    void Draw();
    std::vector<Particle> particles;
private:
    // state   
    unsigned int amount;
    // render state
    Shader shader;
    Texture2D texture;
    unsigned int VAO;

    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle &particle, BallObject&object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

class ParticleGen
{
public:
    ParticleGen(Shader updateShader, Shader renderShader, Texture2D texture, unsigned int amount);
    void Update(float dt);
    void Draw();
private:
    Texture2D texture;
    unsigned int amount;
    GLuint particleSSBO;
    unsigned int particleCount;
    Shader updateShader;  // 新增：Compute Shader
    Shader renderShader;  // 新增：渲染用Shader
    GLuint quadVAO;
    GLuint particleVBO;
    void init();

};

#endif