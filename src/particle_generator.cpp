#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : shader(shader), texture(texture), amount(amount)
{
    this->init();
}

void ParticleGenerator::Update(float dt, BallObject&object, unsigned int newParticles, glm::vec2 offset)
{
    // add new particles 
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    // update all particles
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle &p = this->particles[i];
        p.Life -= dt; // reduce life
        if (p.Life > 0.0f)
        {	// particle is alive, thus update
            p.Position -= p.Velocity * dt; 
            p.Color.a -= dt * 2.5f;
        }
    }
}

// render all particles
void ParticleGenerator::Draw()
{
    // use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    for (Particle particle : this->particles)
    {
        if (particle.Life > 0.0f)
        {
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // set up mesh and attribute properties
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    }; 
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // create this->amount default particle instances
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, BallObject&object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    
    if(object.IsCollide)
    {
        float randomX = ((rand() % 500) - 250) / 5.0f;
        particle.Velocity = glm::vec2(randomX, randomX);
    }
    else particle.Velocity = object.Velocity * 0.1f;
}

ParticleGen::ParticleGen(Shader updateShader, Shader renderShader, Texture2D texture, unsigned int amount)
    : updateShader(updateShader), renderShader(renderShader), texture(texture), amount(amount)
{
    this->particleCount = amount;
    this->init();
}

void ParticleGen::init()
{
    // 生成初始粒子数据
    std::vector<Particle> initialParticles(this->particleCount);
    for (auto& p : initialParticles)
    {
        p.Position = glm::vec2(0.0f);
        p.Velocity = glm::vec2(0.0f);
        p.Color = glm::vec4(1.0f);
        p.Life = 0.0f;
    }

    glGenBuffers(1, &this->particleSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->particleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * this->particleCount, initialParticles.data(), GL_DYNAMIC_DRAW);
    std::cout << "SSBO id: " << particleSSBO << std::endl;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->particleSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // 一个空VAO，用来实例化绘制
    glGenVertexArrays(1, &this->quadVAO);
    glBindVertexArray(this->quadVAO);
    glBindVertexArray(0);
}

void ParticleGen::Update(float dt)
{
    this->updateShader.Use();
    this->updateShader.SetFloat("dt", dt);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->particleSSBO);
    glDispatchCompute((this->particleCount + 255) / 256, 1, 1);
    std::cout << "particleCount is " << particleCount << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleGen::Draw()
{
    this->renderShader.Use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->particleSSBO);

    glBindVertexArray(this->quadVAO);
    glDrawArraysInstanced(GL_POINTS, 0, 1, this->particleCount);
    glBindVertexArray(0);
}


