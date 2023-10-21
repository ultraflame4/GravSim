#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <algorithm>
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


    bool gravity = true;
    bool collision = true;
    bool paused = false;

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
                glm::vec2(x, y),
                glm::vec2(0, 0),
                glm::vec2(0, 0),
                mass,
                radius
        });

    }

    void OnResize() override {
        proj = createOrtho(1.f);
    }

    void OnInput(int key, int scancode, int action, int mods) override {
        if (action != GLFW_PRESS) return;
        if (key == GLFW_KEY_G) {
            gravity = !gravity;
            logger->info("Gravity enabled: {}", gravity);
        }
        if (key == GLFW_KEY_C) {
            collision = !collision;
            logger->info("Collision enabled: {}", collision);
        }
        if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            logger->info("Paused enabled: {}", paused);
        }
    }

    void Load() override {
        logger->info("Hello world!");


        AddBody(-200, 10, 1+5, 10);
        AddBody(-50, 0, 2+5, 20);
        AddBody(800, 0, 1+5, 10);
        AddBody(0, 0, 15, 500);
        AddBody(0, -500, 1+5, 10);
        AddBody(200, -100, 4+5, 40);
        AddBody(200, -500, 4+5, 40);


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

    const float gravityConstant = 50.f;

    void ApplyGravityForce(GravBodyPhysical &bodyp, GravBodyPhysical &otherp) {
        if (!gravity) return;
        float distance = glm::distance(bodyp.pos, otherp.pos);
        float sharedForce = gravityConstant * ((bodyp.mass * otherp.mass) / pow(distance, 2));
        glm::vec2 dirVector = glm::normalize(otherp.pos - bodyp.pos);
        bodyp.Accelerate(dirVector, sharedForce / 2);
    }

    void ApplyCollisionForces(GravBodyPhysical &bodyp, GravBodyPhysical &otherp) {
        if (!collision) return;

        glm::vec2 posA = bodyp.last_pos + bodyp.last_vel * updateTimer.dt;


        glm::vec2 posB = otherp.last_pos + bodyp.last_vel * updateTimer.dt;
        float collisionDist = (bodyp.radius + otherp.radius) * 2;

        float currentDist = glm::distance(posA, posB);


        if (currentDist > collisionDist) {
            return;
        }


        glm::vec2 normal = glm::normalize(posB - posA);
        glm::vec2 incoming = bodyp.last_vel - otherp.last_vel;
        glm::vec2 reflect = glm::reflect(incoming, normal);
//        logger->info("R {},{} F {}", reflect.x, reflect.y,glm::length(reflect*.5f));
        bodyp.vel = reflect * .5f;


    }

    void UpdateGravBodyPhysics(GravBodyPhysical &bodyp, int index) {
        for (int j = 0; j < bodies.size(); ++j) {
            if (index == j) continue; // Skip self
            auto &otherp = physicalBodies[j];
            ApplyGravityForce(bodyp, otherp);
        }

        bodyp.last_vel = bodyp.vel;
        for (int j = 0; j < bodies.size(); ++j) {
            if (index == j) continue; // Skip self
            auto &otherp = physicalBodies[j];
            ApplyCollisionForces(bodyp, otherp);
        }


        bodyp.pos += bodyp.vel * updateTimer.dt;
        bodyp.last_pos = bodyp.pos;

    }

    void Update(float dt) override {
        if (paused) return;

        // Make circles go in circle. this is temp for testing
        for (int i = 0; i < bodies.size(); ++i) {
            auto &body = bodies[i];
            auto &bodyp = physicalBodies[i];
            UpdateGravBodyPhysics(bodyp, i);
            body.x = bodyp.pos.x;
            body.y = bodyp.pos.y;
        }

    }

    void Draw(float dt) override {
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
