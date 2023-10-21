#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <algorithm>
#include <imgui.h>

#include "GravSim/logging.hh"
#include "GravSim/window.hh"
#include "GravSim/GravitySimulation.hh"
#include "GravSim/shader.hh"
#include "GravSim/VertexObject.hh"
#include "GravSim/square.hh"
#include "GravSim/TrajectoryLine.hh"
#include "GravSim/utils.hh"

class Game : public Window {
public:
    Game(int width, int height, const std::string &title) :
    Window(width, height, title),
    simulation(*this){}

protected:


    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj;

    Line targetingLine;

    GravitySimulation simulation;
    bool paused = false;

    glm::mat4 createOrtho(float size, float zNear = .1f, float zFar = 100.0f) {
        float halfW = size * width;
        float halfH = size * height;
        return glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);
    }

    void OnResize() override {
        proj = createOrtho(1.f);
    }

    GravBodyPhysical *spawningGravBody = nullptr; // grav body currently being spawned
    float spawnMass = 10;
    float spawnRadius = 10;
    float spawnColor[3] = {1.f,1.f,1.f};

    void UpdateSpawningBodyVel() {
        if (spawningGravBody == nullptr) return;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec2 pos = screen2WorldPos(glm::vec2(xpos, ypos), proj, *this);
        glm::vec2 vel = spawningGravBody->pos - pos;
        spawningGravBody->vel = vel;
    }


    void OnInput(int key, int action, int mods) override {
        switch (action) {
            case GLFW_RELEASE:

                if (key == GLFW_MOUSE_BUTTON_RIGHT) {
                    spawningGravBody->active = true;
                    spawningGravBody = nullptr;
                }
                break;
            case GLFW_PRESS:
                if (key == GLFW_KEY_G) {
                    simulation.gravity = !simulation.gravity;
                    logger->info("Gravity enabled: {}", simulation.gravity);
                }
                if (key == GLFW_KEY_C) {
                    simulation.collision = !simulation.collision;
                    logger->info("Collision enabled: {}", simulation.collision);
                }
                if (key == GLFW_KEY_SPACE) {
                    paused = !paused;
                    logger->info("Paused enabled: {}", paused);
                }

                if (key == GLFW_MOUSE_BUTTON_RIGHT) {
                    double xpos, ypos;
                    glfwGetCursorPos(window, &xpos, &ypos);
                    glm::vec2 pos = screen2WorldPos(glm::vec2(xpos, ypos), proj, *this);
                    logger->debug("Spawn object at {},{}", pos.x, pos.y);
                    spawningGravBody = &AddBody(pos.x, pos.y, spawnRadius, spawnMass, false);
                }
                break;
            default:
                break;
        }


    }
    GravBodyPhysical &AddBody(float x, float y, float radius, float mass, bool active = true){
        return simulation.AddBody(x, y, radius, mass, spawnColor, active);
    }
    void Load() override {
        logger->info("Hello world!");
        simulation.load();
        targetingLine.load();
        AddBody(-200, 10, 1 + 5, 10);
        AddBody(-50, 0, 2 + 5, 20);
        AddBody(800, 0, 1 + 5, 10);
        AddBody(0, 0, 15, 500);
        AddBody(0, -500, 1 + 5, 10);
        AddBody(200, -100, 4 + 5, 40);
        AddBody(200, -500, 4 + 5, 40);
    }

    void Update(float dt) override {

        if (paused) return;
        simulation.update();
    }

    void OnImGui_Draw() override {
        ImGui::Begin("Simulation Config");
        ImGui::Text("Bodies count: %d", (int) simulation.physicalBodies.size());
        ImGui::Text("FPS: %f", 1.f / frameTimer.delta);
        ImGui::Text("FPS AVG: %f", 1.f / frameTimer.avg_delta);
        ImGui::Text("TPS: %f", 1.f / updateTimer.delta);
        ImGui::Text("TPS AVG: %f", 1.f / updateTimer.avg_delta);
        ImGui::Checkbox("Paused [Spacebar]", &paused);
        ImGui::Checkbox("Enable Gravity [G]", &simulation.gravity);
        ImGui::Checkbox("Enable Collisions [C]", &simulation.collision);
        ImGui::SliderFloat("Gravity Constant", &simulation.gravityConstant, -100.f, 100.f);
        if (ImGui::CollapsingHeader("Gravity Bodies",ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Spawn Mass", &spawnMass, 1, 1000);
            ImGui::SliderFloat("Spawn Radius", &spawnRadius, 5, 500);
            ImGui::ColorEdit3("Spawn Color", spawnColor);
            if (ImGui::Button("Clear Bodies")) {
                simulation.bodies.clear();
                simulation.physicalBodies.clear();
            }
        }
        if (ImGui::CollapsingHeader("Trajectory Line",ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Thickness", &targetingLine.thick, 1, 100);
            ImGui::ColorEdit3("Color", targetingLine.color);
        }
        ImGui::End();
    }

    void Draw(float dt) override {
        UpdateSpawningBodyVel();

        simulation.draw(view,proj);

        targetingLine.active = spawningGravBody != nullptr;
        if (targetingLine.active) {
            targetingLine.origin.x = spawningGravBody->pos.x;
            targetingLine.origin.y = spawningGravBody->pos.y;
            targetingLine.direction.x = spawningGravBody->vel.x;
            targetingLine.direction.y = spawningGravBody->vel.y;
            targetingLine.update();
            targetingLine.draw(view, proj);
        }
    }
};


int main() {

    auto window = std::make_shared<Game>(1000, 800, "GravSim");

    window->run();

    return 0;
}
