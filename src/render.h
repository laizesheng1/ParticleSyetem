#ifndef RENDER_H
#define RENDER_H
#include "game_object.h"
#include "particle_generator.h"
#include "shader.h"
#include "texture.h"
#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Render
{
public:
	Render(int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);

protected:
	Camera camera;
	float lastX;
	float lastY;
	int m_width;
	int m_height;
	bool firstMouse = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
};

class BallRender:public Render
{
public:
	BallRender(int width,int height);
	void initShader();
	void render();
	~BallRender();
private:
	glm::vec2 play_size= glm::vec2(100.0f, 20.0f);
	BallObject* Ball;
	ParticleGenerator* particles;

};

class ParicleRender :public Render
{
public:
	ParicleRender(int width, int height);
	void initShader();
	void render();
private:
	ParticleGen* particles;
	Shader renderShader;
};

#endif
