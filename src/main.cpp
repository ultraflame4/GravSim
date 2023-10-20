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
    std::vector<GravBodyPhysical> physicalBodies;
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

    glm::mat4 createOrtho(float size, float zNear = .1f, float zFar = 100.0f) {
        float halfW = size * width;
        float halfH = size * height;
        return glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);
    }

    void AddBody(float x, float y, float radius, float mass) {
        bodies.push_back(GravBodyVertex{
                x, y,
                radius,
                1, 1, 1
        });

        physicalBodies.push_back(GravBodyPhysical{
                glm::vec2(x, y),
                glm::vec2(0.0,0.0),
                radius,
                mass
        });
    }

    void OnResize() override {
        proj = createOrtho(1);
    }

    void Load() override {
        logger->info("Hello world!");


        AddBody(-800,0,50,100);
        AddBody(800,0,50,10);


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

    const float gravityConstant = 100.f;

    void Update() override {
        // Make circles go in circle. this is temp for testing
        for (int i = 0; i < bodies.size(); ++i) {
            auto &body = bodies[i];
            auto &bodyp = physicalBodies[i];

            for (int j = 0; j < bodies.size(); ++j) {
                if (i == j) continue; // Skip self
                auto &other = bodies[j];
                auto &otherp = physicalBodies[j];

                float distance = glm::distance(bodyp.pos, otherp.pos);
                float sharedForce = gravityConstant * ((bodyp.mass * otherp.mass) / pow(distance, 2));
                glm::vec2 forceVector = glm::normalize(otherp.pos - bodyp.pos) * (sharedForce/2);
                bodyp.vel += forceVector;
            }


            bodyp.pos += bodyp.vel * deltaTime;
            body.x = bodyp.pos.x;
            body.y = bodyp.pos.y;
//            logger->info("Pos {},{}", body.x,body.y);
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
