#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>

#include <memory>
#include <numbers>
#include "GravSim/camera.hh"
#include "GravSim/renderer.hh"
#include "GravSim/simulation.hh"
#include "GravSim/window.hh"
#include "GravSim/utils.hh"
#include "GravSim/line.hh"
#include "GravSim/meta.hh"

class Game : public Window {
  public:
    Game(int width, int height, const std::string& title) : Window(width, height, title) {}

  protected:
    Simulation simulation;

    std::unique_ptr<SimulationRenderer> renderer = nullptr;

    Camera camera;
    glm::vec3 cameraMove = glm::vec3(0, 0, 0);

    glm::vec2 spawnPosition;
    glm::vec2 spawnVel;

    bool draw_debuglines = false;

    glm::mat4 createOrtho(float size, float zNear = .1f, float zFar = 100.0f) {
        float halfW = size * width;
        float halfH = size * height;
        return glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);
    }

    void updateSpawnParameters() {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec2 pos = camera.screen2World(glm::vec2(xpos, ypos));
        spawnVel      = spawnPosition - pos;
    }

  protected:
    // ---- EVENTS ----

    void OnScroll(double x_offset, double y_offset) override { camera.change_zoom(y_offset); }

    void OnInput(int key, int action, int mods) override {
        switch (action) {
            case GLFW_RELEASE:
                if (key == GLFW_MOUSE_BUTTON_RIGHT) {
                    targetingLine.active = false;

                    for (Vertex v : renderer->addVertices) {
                        simulation.spawnBody(
                            glm::vec2(v.x, v.y),
                            spawnVel,
                            spawnRadius,
                            spawnMass,
                            v.color
                        );
                    }

                    renderer->addVertices.clear();
                    // todo, add to simulation!
                }
                if (key == GLFW_KEY_W) cameraMove -= VEC_UP;
                if (key == GLFW_KEY_A) cameraMove -= VEC_LEFT;
                if (key == GLFW_KEY_S) cameraMove -= VEC_DOWN;
                if (key == GLFW_KEY_D) cameraMove -= VEC_RIGHT;
                break;
            case GLFW_PRESS:
                if (key == GLFW_KEY_W) cameraMove += VEC_UP;
                if (key == GLFW_KEY_A) cameraMove += VEC_LEFT;
                if (key == GLFW_KEY_S) cameraMove += VEC_DOWN;
                if (key == GLFW_KEY_D) cameraMove += VEC_RIGHT;

                if (key == GLFW_KEY_G) { simulation.enableGravity = !simulation.enableGravity; }
                if (key == GLFW_KEY_C) {
                    simulation.enableCollision = !simulation.enableCollision;
                }
                if (key == GLFW_KEY_SPACE) {
                    paused = !paused;
                    logger->info("Paused enabled: {}", paused);
                }

                if (key == GLFW_MOUSE_BUTTON_RIGHT) {
                    double xpos, ypos;
                    targetingLine.active = true;
                    glfwGetCursorPos(window, &xpos, &ypos);
                    spawnPosition = camera.screen2World(glm::vec2(xpos, ypos));

                    logger->debug(
                        "Spawning {} object(s) at {},{}",
                        spawnCount,
                        spawnPosition.x,
                        spawnPosition.y
                    );
                    renderer->addVertices.reserve(1 + spawnCount);
                    renderer->addVertices.push_back(
                        Vertex{
                            spawnPosition.x,
                            spawnPosition.y,
                            spawnRadius,
                            spawnColor[0],
                            spawnColor[1],
                            spawnColor[2],

                        }
                    );

                    float dist        = spawnRadius * 2;
                    float angle       = 0;
                    float max_angle   = 360 * (std::numbers::pi / 180);
                    float layer_count = 0;
                    for (int i = 0; i < spawnCount - 1; ++i) {
                        // Distance from center

                        // Amount to increase angle by. Should increase just enough such that
                        // the bodies dont collide!
                        float dist_2sq = 2 * dist * dist;
                        float amt =
                            std::acos((dist_2sq - (4 * spawnRadius * spawnRadius)) / dist_2sq);

                        // Direction offset from spawn position
                        glm::vec2 dir_off(std::sin(angle), std::cos(angle));
                        glm::vec2 final_pos = spawnPosition + (dir_off * dist);

                        renderer->addVertices.push_back(
                            Vertex{
                                final_pos.x,
                                final_pos.y,
                                spawnRadius,
                                spawnColor[0],
                                spawnColor[1],
                                spawnColor[2],
                            }
                        );

                        int max_layer_count = std::floor(max_angle / amt);
                        layer_count++;
                        if (layer_count >= max_layer_count) {
                            dist += spawnRadius * 2;
                            layer_count = 0;
                        }
                        angle += amt;
                    }
                }

                break;
            default:
                break;
        }
    }

  protected:
    Line targetingLine;

    bool paused = true;

    void OnResize() override { camera.resize_viewport(width, height); }

    float spawnMass     = 10;
    float spawnRadius   = 10;
    float spawnColor[3] = {1.f, 1.f, 1.f};
    int spawnCount      = 1;

    void Load() override {
        logger->info("Hello world!");
        updateTps           = 120;
        simulation.stepSize = 1.f / updateTps;
        renderer            = std::make_unique<SimulationRenderer>();
        Line::load();
    }

    void Start() override {
        simulation.spawnBody(glm::vec2(-200, 10), 1 + 5, 10);
        simulation.spawnBody(glm::vec2(-50, 0), 2 + 5, 20);
        simulation.spawnBody(glm::vec2(800, 0), 1 + 5, 10);
        simulation.spawnBody(glm::vec2(0, 0), 15, 500);
        simulation.spawnBody(glm::vec2(0, -500), 1 + 5, 10);
        simulation.spawnBody(glm::vec2(200, -100), 4 + 5, 40);
        simulation.spawnBody(glm::vec2(200, -500), 4 + 5, 40);
    }

    void Update(float dt) override {
        if (paused) return;
        simulation.step();
    }

    void OnImGui_Draw() override {
        ImGui::Begin("Simulation");
        ImGui::Text("Bodies count: %d", (int)simulation.bodies.size());
        ImGui::Text("FPS: %f", 1.f / frameTimer.delta);
        ImGui::Text("FPS AVG: %f", 1.f / frameTimer.avg_delta);
        ImGui::Text("TPS: %f", 1.f / updateTimer.delta);
        ImGui::Text("TPS AVG: %f", 1.f / updateTimer.avg_delta);
        ImGui::Checkbox("Paused [Spacebar]", &paused);
        // ImGui::SliderFloat("Gravity Constant", &simulation.gravityConstant,
        //                    -1000.f, 1000.f);
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Position: %f,%f", camera.position.x, camera.position.y);
            ImGui::SliderFloat("Zoom", &camera.target_zoom, camera.min_zoom, camera.max_zoom);
        }
        if (ImGui::CollapsingHeader("Spawning", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Mass", &spawnMass, 1, 2000);
            ImGui::SliderFloat("Radius", &spawnRadius, 5, 500);
            ImGui::ColorEdit3("Color", spawnColor);
            ImGui::SliderInt("Count", &spawnCount, 1, 20);

            ImGui::Text("Velocity %f,%f", spawnVel.x, spawnVel.y);
            if (ImGui::Button("Clear Bodies")) { simulation.clear(); }
        }
        if (ImGui::CollapsingHeader("Trajectory Line", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Right click & drag to spawn bodies");
            ImGui::SliderFloat("Thickness", &targetingLine.thick, 1, 100);
            ImGui::ColorEdit3("Line Color", targetingLine.color);
        }
        if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Enable Gravity [G]", &simulation.enableGravity);
            ImGui::Checkbox("Enable Collisions [C]", &simulation.enableCollision);
            ImGui::Checkbox("Debug Velocity", &draw_debuglines);
        }
        ImGui::End();
    }

    void Draw(float dt) override {
        camera.target_position += cameraMove * dt * 2000.f * camera.current_zoom;
        camera.update(dt);

        renderer->update_vertices(simulation, updateTpsFixedDelta());
        renderer->draw(camera.view, camera.proj, this->width, this->height);
        if (draw_debuglines) { renderer->debug_draw(camera.view, camera.proj, simulation); }

        if (targetingLine.active) {
            updateSpawnParameters();
            targetingLine.origin.x    = spawnPosition.x;
            targetingLine.origin.y    = spawnPosition.y;
            targetingLine.direction.x = spawnVel.x;
            targetingLine.direction.y = spawnVel.y;
            targetingLine.update_line_vertices();
            targetingLine.draw(camera.view, camera.proj);
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
