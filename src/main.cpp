#include <glad/glad.h>
#include "GravSim/logging.hh"
#include "GravSim/window.hh"
#include "GravSim/GravitationalBody.hh"
#include "GravSim/shader.hh"
#include "GravSim/VertexObject.hh"
#include "GravSim/square.hh"


class Game : public Window {
public:
    Game(int width, int height, const std::string &title) : Window(width, height, title) {}

protected:
    std::vector<GravBodyVertex> bodies;
    Shader shader;
    std::unique_ptr<VertexObject> square;

    int bodyPositions_location;
    int viewport_location;

    void Load() override {
        logger->info("Hello world!");
        square = Square::CreateSquare();
        shader.addShader("./assets/gravbody.vertex.glsl", ShaderType::VERTEX);
        shader.addShader("./assets/gravbody.fragment.glsl", ShaderType::FRAGMENT);
        shader.build();
        shader.use();
        bodyPositions_location = shader.getUniformLoc("bodies_pos");
        viewport_location = shader.getUniformLoc("viewport");






    }

    void Update() override {

    }

    void Draw() override {
        shader.use();

        float bodiesPos[] = {
                0.5f, 0.5f,
                -1.f, 0.f,
        };

        glUniform2fv(bodyPositions_location, sizeof(bodiesPos) / sizeof(float), bodiesPos);
        glUniform4f(viewport_location, 10,10, width, height);

        square->draw();

    }
};


int main() {

    auto window = std::make_shared<Game>(1000, 800, "GravSim");
    window->run();

    return 0;
}
