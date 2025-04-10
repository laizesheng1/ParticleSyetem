#version 430 core

struct Particle {
    vec2 Position;
    vec2 Velocity;
    vec4 Color;
    float Life;
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};
layout (local_size_x = 256) in;

uniform float dt;

void main()
{
    uint id = gl_GlobalInvocationID.x;
    particles[id].Life -= dt;

    if (particles[id].Life > 0.0)
    {
        particles[id].Position -= particles[id].Velocity * dt;
        particles[id].Color.a -= dt * 2.5f;
    }
}
