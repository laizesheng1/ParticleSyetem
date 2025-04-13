#version 430 core

struct Particle3D
{
    vec3 Position;
    vec4 Velocity, Attractors;
    float Life;
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle3D particles[];
};
layout (local_size_x = 256) in;

uniform float dt;
uniform vec3 vec;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float vecLen (vec3 v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vec3 normalize (vec3 v)
{
	return v / vecLen(v);
}

vec3 calcForceFor (vec3 forcePoint, vec3 pos)
{
	// Force:
	float gauss = 10000.0;
	float e = 2.71828183;
	float k_weak = 1.0;
	vec3 dir = forcePoint - pos.xyz;
	float g = pow (e, -pow(vecLen(dir), 2) / gauss);
	vec3 f = normalize(dir) * k_weak * (1+ mod(rand(dir.xy), 10) - mod(rand(dir.yz), 10)) / 10.0 * g;
	return f;
}

void main()
{
	uint index = gl_GlobalInvocationID.x;
	if(index>=particles.length()) return;
	int i;
	float newDT = dt * 100.0;
	Particle3D p=particles[index];
	
	vec3 forcePoint = vec3(0);

    particles[index].Attractors = vec4(vec, 0.0f);

    for (i = 0; i < 32; i++) {
        forcePoint += particles[i].Attractors.xyz;
    }	
	
	// Read the current position and velocity from the buffers
	vec4 vel = p.Velocity;
	vec3 pos = p.Position;
	float newW = p.Life;
	float k_v = 1.5;
	vec3 f = calcForceFor(forcePoint, pos) + rand(pos.xz)/100.0;
	
	// Velocity:
	vec3 v = normalize(vel.xyz + (f * newDT)) * k_v;
	v += (forcePoint-pos) * 0.00005;
	
	// Pos:
	vec3 Pos = pos + v * newDT;
	newW -= 0.0001f * newDT;
	if (newW <= 0) {
		Pos  = -Pos + rand(Pos.xy)*20.0 -rand(Pos.yz)*20.0;
		//v.xyz *= 0.01f;
		newW = 0.99f;
	}
	
    particles[index].Life=newW;
	particles[index].Position = Pos;	
	particles[index].Velocity = vec4(v,vel.w);
}