//
// Created by powew on 20/10/2023.
//


#include <fstream>
#include <filesystem>
#include <sstream>
#include "GravSim/shader.hh"

Shader::Shader() = default;

void Shader::addShader(const std::string &source_path, ShaderType shaderType) {
    std::fstream infile;
    infile.open(source_path);
    if (!infile.is_open()) {
        std::string abs_path = std::filesystem::absolute(source_path).string();
        logger->error("Could not find path: {} (resolved: {})", source_path, abs_path);
        return;
    }

    std::stringstream shaderSrcStream;
    shaderSrcStream << infile.rdbuf();
    const std::string shaderSrc = shaderSrcStream.str();
    const char *shaderSrc_c = shaderSrc.c_str();

    unsigned int shader = glCreateShader(shaderType);

    switch (shaderType) {
        case VERTEX:
            vertexShader = shader;
            break;
        case FRAGMENT:
            fragmentShader = shader;
            break;
    }

    glShaderSource(shader, 1, &shaderSrc_c, nullptr);
    glCompileShader(shader);
    if (checkCompileSuccess(shader)) {
        logger->trace("Successfully compiled shader (id: {}) from path {}", shader, source_path);
    }
}

bool Shader::checkCompileSuccess(unsigned int shader) {
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        logger->error("Failed to compile shader {} because of {}", shader,infoLog);
        return false;
    }
    return true;
}

void Shader::build() {
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        logger->error("Failed to link shaders (shaderProgram: {}) because of {}", shaderProgram, infoLog);
        return;
    }
    logger->trace("Successfully linked shaders!");
}

void Shader::use() {
    glUseProgram(shaderProgram);
}
