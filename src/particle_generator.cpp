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
    std::vector<Particle3D> initialParticles(this->particleCount);
    for (auto& p : initialParticles)
    {
        glm::vec3 vec;
        vec.x = (rand() % 2000 - 1000) / (500.0);
        vec.y = (rand() % 2000 - 1000) / (500.0);
        vec.z = (rand() % 2000 - 1000) / (500.0);
        p.Position = vec;

        vec.x = (rand() % 100) / 500.0 - (rand() % 100) / 500.0;
        vec.y = (rand() % 100) / 500.0 - (rand() % 100) / 500.0;
        vec.z = (rand() % 100) / 500.0 - (rand() % 100) / 500.0;
        p.Velocity = glm::vec4(vec, 0.0);
           
        vec.x = (rand() % 500) / 30.0 - (rand() % 500) / 30.0;
        vec.y = (rand() % 500) / 30.0 - (rand() % 500) / 30.0;
        vec.z = (rand() % 500) / 30.0 - (rand() % 500) / 30.0;
        p.Attractors = glm::vec4(vec, 0.0);
            
        p.Life = rand() / (double)RAND_MAX;
    }

    glGenBuffers(1, &this->particleSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->particleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle3D) * this->particleCount, initialParticles.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->particleSSBO);
    std::cout << "SSBO id: " << particleSSBO << std::endl;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &this->particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->particleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle3D) * this->particleCount, initialParticles.data(), GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &this->quadVAO);
    glBindVertexArray(this->quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->particleVBO);

    // 位置 Position: vec3, location = 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (void*)offsetof(Particle3D, Position));

    // 生命 Life: float, location = 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (void*)offsetof(Particle3D, Life));

    glBindVertexArray(0);
}

void ParticleGen::Update(float dt)
{
    this->updateShader.Use();
    this->updateShader.SetFloat("dt", dt);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->particleSSBO);
    glDispatchCompute((this->particleCount + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleGen::Draw()
{
    this->renderShader.Use();
    //this->texture.Bind();
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->particleSSBO);

    // 将Compute Shader更新后的SSBO同步拷贝到绘制用VBO
    glBindBuffer(GL_COPY_READ_BUFFER, this->particleSSBO);
    glBindBuffer(GL_COPY_WRITE_BUFFER, this->particleVBO);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(Particle3D) * this->particleCount);

    // 绘制
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_POINTS, 0, this->particleCount);
    glBindVertexArray(0);
}


