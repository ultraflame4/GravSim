import testmodel;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <algorithm>
#include <numbers>
#include "GravSim/window.hh"
#include "GravSim/GravitySimulation.hh"
#include "GravSim/utils.hh"
#include "GravSim/line.hh"
#include "GravSim/meta.hh"

class Game : public Window {
public:
    Game(int width, int height, const std::string &title) :
            Window(width, height, title),
            simulation(*this) {}

protected:

    glm::vec3 cameraPos = glm::vec3(0, 0, 0);
    glm::vec3 targetCameraPos = glm::vec3(0, 0, 0);
    glm::vec3 cameraMove = glm::vec3(0, 0, 0);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj;
    float current_zoom = 1.f;
    float target_zoom = 1.f;
    float zoom_speed = .1f;
    const float max_zoom = 5;
    const float min_zoom = .2f;

    Line targetingLine;

    GravitySimulation simulation;
    bool paused = false;

    glm::mat4 createOrtho(float size, float zNear = .1f, float zFar = 100.0f) {
        float halfW = size * width;
        float halfH = size * height;
        return glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);
    }

    void OnResize() override {
        proj = createOrtho(current_zoom);
    }

    std::vector<int> spawningGravBodies; // grav body currently being spawned
    float spawnMass = 10;
    float spawnRadius = 10;
    float spawnColor[3] = {1.f, 1.f, 1.f};
    int spawnCount = 1;

    glm::vec2 spawnPosition;
    glm::vec2 spawnVel;

    void UpdateSpawningBodyVel() {
        if (spawningGravBodies.empty()) return;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec2 pos = screen2WorldPos(glm::vec2(xpos, ypos), proj, view, *this);
        spawnVel = spawnPosition - pos;
    }

    int AddBody(float x, float y, float radius, float mass, bool active = true) {
        return simulation.addBody(x, y, radius, mass, spawnColor, active);
    }

    void OnScroll(double x_offset, double y_offset) override {
        target_zoom -= y_offset * zoom_speed;
    }

    void OnInput(int key, int action, int mods) override {
        switch (action) {
            case GLFW_RELEASE:
                if (key == GLFW_MOUSE_BUTTON_RIGHT && !spawningGravBodies.empty()) {
                    for (int bodyIndex: spawningGravBodies) {
                        GravBodyPhysical& body = simulation.physicalBodies[bodyIndex];
                        body.vel = spawnVel;
                        body.active = true;
                    }
                    spawningGravBodies.clear();
                }
                if (key == GLFW_KEY_W) cameraMove -= up;
                if (key == GLFW_KEY_A) cameraMove -= left;
                if (key == GLFW_KEY_S) cameraMove -= down;
                if (key == GLFW_KEY_D) cameraMove -= right;
                break;
            case GLFW_PRESS:
                if (key == GLFW_KEY_W) cameraMove += up;
                if (key == GLFW_KEY_A) cameraMove += left;
                if (key == GLFW_KEY_S) cameraMove += down;
                if (key == GLFW_KEY_D) cameraMove += right;

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
                    spawnPosition = screen2WorldPos(glm::vec2(xpos, ypos), proj, view, *this);

                    logger->debug("Spawning {} object(s) at {},{}", spawnCount, spawnPosition.x, spawnPosition.y);
                    spawningGravBodies.emplace_back(
                            AddBody(spawnPosition.x, spawnPosition.y, spawnRadius, spawnMass, false));
                    float dist = spawnRadius *2;
                    float angle = 0;
                    float max_angle = 360 * (std::numbers::pi / 180);
                    float layer_count = 0;
                    for (int i = 0; i < spawnCount - 1; ++i) {

                        // Distance from center

                        // Amount to increase angle by. Should increase just enough such that the bodies dont collide!
                        float dist_2sq = 2 * dist * dist;
                        float amt = std::acos((dist_2sq - (4 * spawnRadius * spawnRadius)) / dist_2sq);


                        // Direction offset from spawn position
                        glm::vec2 dir_off(std::sin(angle), std::cos(angle));
                        glm::vec2 final_pos = spawnPosition + (dir_off * dist);

                        spawningGravBodies.emplace_back(
                                AddBody(final_pos.x, final_pos.y, spawnRadius, spawnMass, false));

                        int max_layer_count = std::floor(max_angle / amt);
                        layer_count++;
                        if (layer_count >= max_layer_count) {
                            dist+=spawnRadius*2;
                            layer_count=0;
                        }
                        angle += amt;
                    }

                }
                break;
            default:
                break;
        }


    }

    void Load() override {
        logger->info("Hello world!");
        logger->info("Module result {}", add(13,45));
        this->updateTps = 20;
        simulation.load();
        Line::load();

    }
    
    void Start() override{
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
        simulation.step();
    }

    void OnImGui_Draw() override {
        ImGui::Begin("Simulation");
        ImGui::Text("Bodies count: %d", (int) simulation.physicalBodies.size());
        ImGui::Text("FPS: %f", 1.f / frameTimer.delta);
        ImGui::Text("FPS AVG: %f", 1.f / frameTimer.avg_delta);
        ImGui::Text("TPS: %f", 1.f / updateTimer.delta);
        ImGui::Text("TPS AVG: %f", 1.f / updateTimer.avg_delta);
        ImGui::Checkbox("Paused [Spacebar]", &paused);
        ImGui::SliderFloat("Gravity Constant", &simulation.gravityConstant, -1000.f, 1000.f);
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Position: %f,%f", cameraPos.x, cameraPos.y);
            ImGui::SliderFloat("Zoom", &target_zoom, min_zoom, max_zoom);
        }
        if (ImGui::CollapsingHeader("Spawning", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Mass", &spawnMass, 1, 2000);
            ImGui::SliderFloat("Radius", &spawnRadius, 5, 500);
            ImGui::ColorEdit3("Color", spawnColor);
            ImGui::SliderInt("Count", &spawnCount, 1, 20);
            glm::vec2 text_vel = spawningGravBodies.empty() ? glm::vec2(0, 0) : spawnVel;
            ImGui::Text("Velocity %f,%f", text_vel.x,  text_vel.y);
            if (ImGui::Button("Clear Bodies")) {
                simulation.clear();
            }
        }
        if (ImGui::CollapsingHeader("Trajectory Line", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Thickness", &targetingLine.thick, 1, 100);
            ImGui::ColorEdit3("Line Color", targetingLine.color);
        }
        if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Enable Gravity [G]", &simulation.gravity);
            ImGui::Checkbox("Enable Collisions [C]", &simulation.collision);
            ImGui::Checkbox("Debug Velocity", &simulation.debug);
        }
        ImGui::End();
    }

    void Draw(float dt) override {

        target_zoom = std::clamp(target_zoom, min_zoom, max_zoom);
        if (abs(current_zoom - target_zoom) < 0.01f) {
            current_zoom = target_zoom;
        } else {
            current_zoom = std::lerp(current_zoom, target_zoom, 10 * dt);
            proj = createOrtho(current_zoom);
        }

        targetCameraPos = targetCameraPos + cameraMove * dt * 2000.f * current_zoom;
        if (abs(length(cameraPos - targetCameraPos)) < (0.01f * current_zoom)) {
            cameraPos = glm::mix(cameraPos, targetCameraPos, 10 * dt);
        } else {
            cameraPos = glm::mix(cameraPos, targetCameraPos, 10 * dt);
        }


        view = glm::lookAt(cameraPos, cameraPos + forward, up);

        UpdateSpawningBodyVel();

        simulation.update_positions();
        simulation.draw(view, proj);

        targetingLine.active = !spawningGravBodies.empty();
        if (targetingLine.active) {
            targetingLine.origin.x = spawnPosition.x;
            targetingLine.origin.y = spawnPosition.y;
            targetingLine.direction.x = spawnVel.x;
            targetingLine.direction.y = spawnVel.y;
            targetingLine.update_line_vertices();
            targetingLine.draw(view, proj);
        }
    }


};


int main() {
    logging::get("main()")->info("GravSim version {}", GravSim_VERSION);
    std::string title = "GravSim v";
    
    title += GravSim_VERSION;
    auto window = std::make_shared<Game>(1000, 800, title);

    window->run();

    return 0;
}
