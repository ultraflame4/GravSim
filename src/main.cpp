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
    VertexObject *bodies_draw;

    void Load() override {
        logger->info("Hello world!");


        bodies.push_back(GravBodyVertex{
                0.5, 0.5,
                50,
                1, 1, 1
        });

        bodies.push_back(GravBodyVertex{
                0, 0,
                50,
                1, 1, 1
        });

        auto *bodiesArr = reinterpret_cast<float *>(bodies.data());
        int stride = sizeof(GravBodyVertex);
        int size = stride * bodies.size();
        bodies_draw = new VertexObject(size, bodiesArr, stride);

        bodies_draw->CreateAttrib(2); // Position attribute
        bodies_draw->CreateAttrib(1); // radius attribute
        bodies_draw->CreateAttrib(3); // color attribute



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
        glUniform2f(viewport_location, width, height);
        bodies_draw->drawPoints();

    }
};


int main() {

    auto window = std::make_shared<Game>(1000, 800, "GravSim");
    window->run();

    return 0;
}
