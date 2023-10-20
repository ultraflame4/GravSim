#include <glad/glad.h>
#include "GravSim/logging.hh"
#include "GravSim/window.hh"
#include "GravSim/GravitationalBody.hh"
#include "GravSim/shader.hh"
#include "GravSim/VertexObject.hh"


class Game : public Window {
public:
    Game(int width, int height, const std::string &title) : Window(width, height, title) {}

protected:
    std::vector<GravBodyVertex> bodies;
    Shader shader;
    std::unique_ptr<VertexObject> vo;

    void Load() override {
        logger->info("Hello world!");

        float vertices[] = {
                -1, 1,
                1, 1,
                1, -1,
                -1, -1

        };
        unsigned int indices[] = {
                2,1,0,
                0, 3, 2
        };

        vo = std::make_unique<VertexObject>(sizeof(vertices), vertices, 2 * sizeof(float));
        vo->SetTriangles(sizeof(indices), indices);
        vo->CreateAttrib(2);


        shader.addShader("./assets/gravbody.vertex.glsl", ShaderType::VERTEX);
        shader.addShader("./assets/gravbody.fragment.glsl", ShaderType::FRAGMENT);
        shader.build();

    }

    void Update() override {

    }

    void Draw() override {
        shader.use();
        vo->draw();

    }
};


int main() {

    auto window = std::make_shared<Game>(1000, 800, "GravSim");
    window->run();

    return 0;
}
