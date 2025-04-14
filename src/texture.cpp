#include <iostream>
#include "texture.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stb_image.h>

Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{    
    this->Width = width;
    this->Height = height;
    // create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

//类外实例化静态变量
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;
Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

std::string ResourceManager::readShaderSource(const char* filePath)
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

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (data)
    {
        texture.Generate(width, height, data);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load particle texture" << std::endl;
    }
    return texture;
}
