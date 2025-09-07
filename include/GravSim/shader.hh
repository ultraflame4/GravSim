//
// Created by powew on 20/10/2023.
//

#pragma once

#include <glad/glad.h>
#include <string>
#include "logging.hh"

enum ShaderType{
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
};

class Shader {
public:
    unsigned int shaderProgram;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    Shader();

    void addShader(const std::string &source_path, ShaderType shaderType);
    int getUniformLoc(const std::string &name);

    void build();

    void use();
private:
    static inline std::shared_ptr<spdlog::logger> logger = logging::get<Shader>();
    bool checkCompileSuccess(unsigned int shader);

};
