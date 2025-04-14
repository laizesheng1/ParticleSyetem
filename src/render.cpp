#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "render.h"

Render::Render(int width, int height)
{
    this->m_width = width;
    this->m_height = height;
    camera= Camera(glm::vec3(0.0f, 0.0f, 200.0f));
    lastX = m_width / 2.0f;
    lastY = m_height / 2.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Render::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Render::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Render::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

BallRender::BallRender(int width, int height)
    :Render(width,height)
{

}

void BallRender::initShader()
{
    Shader particleShader;
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
    std::string vertexCode = ResourceManager::readShaderSource("../src/particle.vs");
    std::string fragmentCode = ResourceManager::readShaderSource("../src/particle.fs");

    particleShader.Compile(vertexCode.c_str(), fragmentCode.c_str());
    particleShader.Use();
    particleShader.SetMatrix4("projection", projection);

    Texture2D particle_texture, ball_texture;
    particle_texture = ResourceManager::LoadTexture("../resources/textures/particle.png", true, "particle");
    ball_texture = ResourceManager::LoadTexture("../resources/textures/awesomeface.png", true, "face");

    particles = new ParticleGenerator(particleShader, particle_texture, 500); // 500个粒子
    glm::vec2 playerPos = glm::vec2(m_width / 2.0f - play_size.x / 2.0f, m_height - play_size.y);
    glm::vec2 ballPos = playerPos + glm::vec2(play_size.x / 2.0f - 12.5f, -12.5f * 2.0f);
    Ball = new BallObject(ballPos, 12.5f, glm::vec2(100.0f, -350.0f), ball_texture);
}

void BallRender::render()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(m_width, m_height, "Ball-Particle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initShader();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        // 渲染
        Ball->Move(deltaTime, m_width,m_height);
        particles->Update(deltaTime, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
        if (Ball->IsCollide)
        {
            particles->Update(0.0f, *Ball, 100, glm::vec2(Ball->Radius / 2.0f));
        }
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制粒子
        particles->Draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //ResourceManager::Clear();
    delete particles;
    glfwTerminate();
}

BallRender::~BallRender()
{

}

ParicleRender::ParicleRender(int width, int height)
    :Render(width, height)
{

}

void ParicleRender::initShader()
{
    Shader updateShader;
    updateShader.CompileComputeShader(ResourceManager::readShaderSource("../src/particle.cs").c_str());
    renderShader.Compile(
        ResourceManager::readShaderSource("../src/particle_render.vs").c_str(),
        ResourceManager::readShaderSource("../src/particle_render.fs").c_str()
    );
    //glm::mat4 projection = {
    //    0.562500f,0.000000f, 0.000000f, 0.000000f,
    //    0.000000f,1.000000f, 0.000000f, 0.000000f,
    //    0.000000f,0.000000f, -1.000100f, -1.000000f,
    //    0.000000f,0.000000f, -1.000050f,0.000000f,
    //};
    //glm::mat4 view = {
    //    0.831538f,0.269314f, -0.485813f, 0.000000f,
    //    0.000000f,0.874601f, 0.484843f, 0.000000f,
    //    0.555468f,-0.403165f, 0.727264f, 0.000000f,
    //    0.000000f,-0.000004f, -103.126190f, 1.000000f,
    //};
    particles = new ParticleGen(updateShader, renderShader, 1000000);
}

void ParicleRender::render()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(m_width, m_height, "Particle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //鼠标移动视角、滚轮进行摄像机前后移动
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
            Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
            render->mouse_callback(window, xpos, ypos);
        });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
            Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
            render->scroll_callback(window, xpos, ypos);
        });
    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initShader();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        // 渲染
        particles->Update(deltaTime, currentFrame);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 每一帧改变矩阵变换
        renderShader.Use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)m_width / (float)m_height, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        renderShader.SetMatrix4("projection", projection);
        renderShader.SetMatrix4("view", view);
        // 绘制粒子
        particles->Draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //ResourceManager::Clear();
    delete particles;
    glfwTerminate();
}