#version 430 core
layout(location = 0) in vec2 dummy; // 不用，实例化绘制

layout(std430, binding = 0) buffer ParticleBuffer {
    struct Particle {
        vec2 Position;
        vec2 Velocity;
        vec4 Color;
        float Life;
    };
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