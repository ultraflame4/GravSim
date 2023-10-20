#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
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

    int feathering_loc;
    int viewport_loc;
    int model_loc;
    int view_loc;
    int proj_loc;

    VertexObject *bodies_draw;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj;

    glm::mat4 createOrtho(float size, float zNear = .1f, float zFar = 100.0f){
        float halfW = size * width ;
        float halfH = size * height ;
        return glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);
    }

    void OnResize() override {
        proj = createOrtho(5);
        logger->info("Resized");
    }

    void Load() override {
        logger->info("Hello world!");


        bodies.push_back(GravBodyVertex{
                0, 0,
                50,
                1, 1, 1
        });

        bodies.push_back(GravBodyVertex{
                -.9f, 0,
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

        feathering_loc = shader.getUniformLoc("feathering");
        viewport_loc = shader.getUniformLoc("viewport");
        model_loc = shader.getUniformLoc("model");
        view_loc = shader.getUniformLoc("view");
        proj_loc = shader.getUniformLoc("proj");

    }

    void Update() override {
        // Make circles go in circle. this is temp for testing
        for (int i = 0; i < bodies.size(); ++i) {
            auto &body = bodies[i];

            auto time = glfwGetTime() + i *10;
            body.x = sin(time) * 100;
            body.y = cos(time) * 100;
        }
    }

    void Draw() override {
        auto *bodiesArr = reinterpret_cast<float *>(bodies.data());
        int stride = sizeof(GravBodyVertex);
        int size = stride * bodies.size();
        bodies_draw->SetVertices(bodiesArr, size);


        shader.use();
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

        glUniform2f(viewport_loc, width, height);
        glUniform1f(feathering_loc, 1);
        bodies_draw->drawPoints();

    }
};


int main() {

    auto window = std::make_shared<Game>(1000, 800, "GravSim");
    window->run();

    return 0;
}
