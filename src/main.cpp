#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "game_object.h"
#include "particle_generator.h"
#include "shader.h"
#include "texture.h"
#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
// 屏幕大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);


// 回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 辅助函数：从文件加载shader源码
std::string readShaderSource(const char* filePath)
{
    std::ifstream shaderFile;
    std::stringstream shaderStream;
    shaderFile.open(filePath);
    if (shaderFile.fail())
    {
        std::cerr << "Failed to load shader: " << filePath << std::endl;
        exit(-1);
    }
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 初始化Shader
    Shader particleShader;
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT), 0.0f, -1.0f, 1.0f);
    std::string vertexCode = readShaderSource("../src/particle.vs");
    std::string fragmentCode = readShaderSource("../src/particle.fs");
    
    /*string csCode = readShaderSource("../src/particle.cs");
    GLuint csProgarm = glCreateShaderProgramv(GL_COMPUTE_SHADER, 1, csCode.c_str());*/
    particleShader.Compile(vertexCode.c_str(), fragmentCode.c_str());
    particleShader.Use();
    particleShader.SetMatrix4("projection", projection);
    // 创建粒子系统
    Texture2D particle_texture,ball_texture;
    particle_texture=ResourceManager::LoadTexture("../resources/textures/particle.png", true, "particle");
    ball_texture=ResourceManager::LoadTexture("../resources/textures/awesomeface.png", true, "face");
    
    ParticleGenerator* particles = new ParticleGenerator(particleShader, particle_texture, 500); // 500个粒子
    glm::vec2 playerPos = glm::vec2(SCR_WIDTH / 2.0f - PLAYER_SIZE.x / 2.0f, SCR_HEIGHT - PLAYER_SIZE.y);
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - 12.5f, -12.5f * 2.0f);
    auto Ball = new BallObject(ballPos, 12.5f, glm::vec2(100.0f, -350.0f), ball_texture);
    // 渲染循环
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // 输入
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // 渲染
        Ball->Move(deltaTime, SCR_WIDTH,SCR_HEIGHT);
        particles->Update(deltaTime, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
        if (Ball->IsCollide)
        {
            particles->Update(0.0f, *Ball, 100, glm::vec2(Ball->Radius / 2.0f));
        }
        //particles->Update(deltaTime, ballPos, glm::vec2(100.0f, -350.0f),2, glm::vec2(12.5f / 2.0f));
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制粒子
        particles->Draw();
        glfwSwapBuffers(window);       
    }    
    //ResourceManager::Clear();
    delete particles;
    glfwTerminate();
    return 0;
}
