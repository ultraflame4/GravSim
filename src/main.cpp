#include <glad/glad.h>
#include "GravSim/logging.hh"
#include "GravSim/window.hh"
#include "GravSim/GravitationalBody.hh"
#include "GravSim/shader.hh"


class Game : public Window {
public:
    Game(int width, int height, const std::string &title) : Window(width, height, title) {}

protected:
    std::vector<GravBodyVertex> bodies;
    Shader shader;

    void Load() override {
        logger->info("Hello world!");
        bodies.push_back(GravBodyVertex{0, 0, 10, 1, 1, 1});

        int trueLength = sizeof(GravBodyVertex) / sizeof(float);
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, bodies.size() * trueLength, bodies.data(), GL_STATIC_DRAW);

        shader.addShader("./assets/gravbody.vertex.glsl", ShaderType::VERTEX);
        shader.addShader("./assets/gravbody.fragment.glsl", ShaderType::FRAGMENT);
        shader.build();

    }

    void Update() override {

    }

    void Draw() override {
        shader.use();

    }
};


int main() {

    auto window = std::make_shared<Game>(1000, 800, "GravSim");
    window->run();

    return 0;
}
