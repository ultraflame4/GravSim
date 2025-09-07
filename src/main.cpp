#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <algorithm>
#include <numbers>
#include "GravSim/window.hh"
#include "GravSim/utils.hh"
#include "GravSim/line.hh"
#include "GravSim/meta.hh"

class Game : public Window {
public:
    Game(int width, int height, const std::string &title)
        : Window(width, height, title) {}

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

    glm::mat4 createOrtho(float size, float zNear = .1f, float zFar = 100.0f) {
        float halfW = size * width;
        float halfH = size * height;
        return glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);
    }

protected:
    // ---- EVENTS ----

    void OnScroll(double x_offset, double y_offset) override {
        target_zoom -= y_offset * zoom_speed;
    }

    void OnInput(int key, int action, int mods) override {
        switch (action) {
        case GLFW_RELEASE:
            if (key == GLFW_MOUSE_BUTTON_RIGHT) {
                // TODO
            }
            if (key == GLFW_KEY_W)
                cameraMove -= VEC_UP;
            if (key == GLFW_KEY_A)
                cameraMove -= VEC_LEFT;
            if (key == GLFW_KEY_S)
                cameraMove -= VEC_DOWN;
            if (key == GLFW_KEY_D)
                cameraMove -= VEC_RIGHT;
            break;
        case GLFW_PRESS:
            if (key == GLFW_KEY_W)
                cameraMove += VEC_UP;
            if (key == GLFW_KEY_A)
                cameraMove += VEC_LEFT;
            if (key == GLFW_KEY_S)
                cameraMove += VEC_DOWN;
            if (key == GLFW_KEY_D)
                cameraMove += VEC_RIGHT;

            if (key == GLFW_KEY_G) {
            }
            if (key == GLFW_KEY_C) {
            }
            if (key == GLFW_KEY_SPACE) {
                paused = !paused;
                logger->info("Paused enabled: {}", paused);
            }

            if (key == GLFW_MOUSE_BUTTON_RIGHT) {
            }

            break;
        default:
            break;
        }
    }

protected:
    Line targetingLine;

    bool paused = false;

    void OnResize() override { proj = createOrtho(current_zoom); }

    float spawnMass = 10;
    float spawnRadius = 10;
    float spawnColor[3] = {1.f, 1.f, 1.f};
    int spawnCount = 1;

    glm::vec2 spawnPosition;
    glm::vec2 spawnVel;

    void Load() override {
        logger->info("Hello world!");
        this->updateTps = 20;
        // simulation.load();
        Line::load();
    }

    void Start() override {
        // addBody(-200, 10, 1 + 5, 10);
        // addBody(-50, 0, 2 + 5, 20);
        // addBody(800, 0, 1 + 5, 10);
        // addBody(0, 0, 15, 500);
        // addBody(0, -500, 1 + 5, 10);
        // addBody(200, -100, 4 + 5, 40);
        // addBody(200, -500, 4 + 5, 40);
    }

    void Update(float dt) override {

        if (paused)
            return;
        // simulation.step();
    }

    void OnImGui_Draw() override {
        ImGui::Begin("Simulation");
        // ImGui::Text("Bodies count: %d",
        // (int)simulation.physicalBodies.size());
        ImGui::Text("FPS: %f", 1.f / frameTimer.delta);
        ImGui::Text("FPS AVG: %f", 1.f / frameTimer.avg_delta);
        ImGui::Text("TPS: %f", 1.f / updateTimer.delta);
        ImGui::Text("TPS AVG: %f", 1.f / updateTimer.avg_delta);
        ImGui::Checkbox("Paused [Spacebar]", &paused);
        // ImGui::SliderFloat("Gravity Constant", &simulation.gravityConstant,
        //                    -1000.f, 1000.f);
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Position: %f,%f", cameraPos.x, cameraPos.y);
            ImGui::SliderFloat("Zoom", &target_zoom, min_zoom, max_zoom);
        }
        if (ImGui::CollapsingHeader("Spawning",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Mass", &spawnMass, 1, 2000);
            ImGui::SliderFloat("Radius", &spawnRadius, 5, 500);
            ImGui::ColorEdit3("Color", spawnColor);
            ImGui::SliderInt("Count", &spawnCount, 1, 20);
            // glm::vec2 text_vel =
            //     spawningGravBodies.empty() ? glm::vec2(0, 0) : spawnVel;
            // ImGui::Text("Velocity %f,%f", text_vel.x, text_vel.y);
            // if (ImGui::Button("Clear Bodies")) {
            //     simulation.clear();
            // }
        }
        if (ImGui::CollapsingHeader("Trajectory Line",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Thickness", &targetingLine.thick, 1, 100);
            ImGui::ColorEdit3("Line Color", targetingLine.color);
        }
        if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
            // ImGui::Checkbox("Enable Gravity [G]", &simulation.gravity);
            // ImGui::Checkbox("Enable Collisions [C]", &simulation.collision);
            // ImGui::Checkbox("Debug Velocity", &simulation.debug);
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

        targetCameraPos =
            targetCameraPos + cameraMove * dt * 2000.f * current_zoom;
        if (abs(length(cameraPos - targetCameraPos)) < (0.01f * current_zoom)) {
            cameraPos = glm::mix(cameraPos, targetCameraPos, 10 * dt);
        } else {
            cameraPos = glm::mix(cameraPos, targetCameraPos, 10 * dt);
        }

        view = glm::lookAt(cameraPos, cameraPos + VEC_FORWARD, VEC_UP);

        // simulation.update_positions();
        // simulation.draw(view, proj);

        // targetingLine.active = !spawningGravBodies.empty();
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
