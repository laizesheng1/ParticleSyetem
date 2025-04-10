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

out vec4 ParticleColor;

uniform mat4 projection;

void main()
{
    Particle p = particles[gl_InstanceID];
    gl_Position = projection * vec4(p.Position, 0.0, 1.0);
    gl_PointSize = 2.0;
    ParticleColor = p.Color;
}
