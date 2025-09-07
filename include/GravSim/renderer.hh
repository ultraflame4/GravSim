#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "GravSim/Line.hh"
#include "GravSim/simulation.hh"
#include "GravSim/vertex_object.hh"
#include "GravSim/shader.hh"

struct Vertex {
    float x;
    float y;
    float radius;
    float r;
    float g;
    float b;
};
static_assert(
    sizeof(Vertex) == sizeof(float) * 6,
    "ERROR GravBodyVertex struct contains padding!"
);

class SimulationRenderer {
  public:
    std::vector<Vertex> vertices;
    /** Additional vertices to draw. */
    std::vector<Vertex> addVertices;
    Shader shader;

  private:
    std::shared_ptr<spdlog::logger> logger = logging::get<SimulationRenderer>();

    VertexObject* vo = nullptr;

    int feathering_loc;
    int viewport_loc;
    int model_loc;
    int view_loc;
    int proj_loc;

    std::vector<Line> debugLines;

  public:
    SimulationRenderer() {
        auto* bodiesArr = reinterpret_cast<float*>(vertices.data());
        int stride      = sizeof(Vertex);
        int size        = stride * vertices.size();
        vo              = new VertexObject(size, bodiesArr, stride);

        vo->CreateAttrib(2);  // Position attribute
        vo->CreateAttrib(1);  // radius attribute
        vo->CreateAttrib(3);  // color attribute

        shader.addShader("./assets/gravbody.vertex.glsl", ShaderType::VERTEX);
        shader.addShader("./assets/gravbody.fragment.glsl", ShaderType::FRAGMENT);
        shader.build();
        shader.use();

        feathering_loc = shader.getUniformLoc("feathering");
        viewport_loc   = shader.getUniformLoc("viewport");
        model_loc      = shader.getUniformLoc("model");
        view_loc       = shader.getUniformLoc("view");
        proj_loc       = shader.getUniformLoc("proj");
    }

    /**
     * @brief Updates the vertices with information from the simulation.
     *
     * @param sim
     */
    void update_vertices(Simulation& sim) {
        int vertex_count = sim.bodies.size();
        // Call reserve early to avoid allocation on resize and insert
        vertices.reserve(vertex_count + addVertices.size());
        vertices.resize(vertex_count);
        for (int i = 0; i < sim.bodies.size(); i++) {
            auto& body  = sim.bodies[i];
            vertices[i] = Vertex{body.pos.x, body.pos.y, body.radius, 1.0f, 255.0f, 1.0f};
        }
        vertices.insert(vertices.end(), addVertices.begin(), addVertices.end());
    }

    void draw(glm::mat4 view, glm::mat4 proj, int window_width, int window_height) {
        auto* bodiesArr = reinterpret_cast<float*>(vertices.data());
        int stride      = sizeof(Vertex);
        int size        = stride * vertices.size();

        glm::mat4 model = glm::mat4(1.0f);

        vo->SetVertices(bodiesArr, size);

        shader.use();
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

        glUniform2f(viewport_loc, window_width, window_height);
        glUniform1f(feathering_loc, 1);

        vo->drawPoints();
    }

    void debug_draw(glm::mat4 view, glm::mat4 proj, Simulation& sim) {

        debugLines.resize(sim.bodies.size());
        for (int i = 0; i < sim.bodies.size(); ++i) {
            auto& line  = debugLines[i];
            auto& bodyp = sim.bodies[i];

            line.color[0]    = 0.4f;
            line.color[1]    = 0.9f;
            line.color[2]    = 0.1f;
            line.direction.x = bodyp.vel.x;
            line.direction.y = bodyp.vel.y;
            line.origin.x    = bodyp.pos.x;
            line.origin.y    = bodyp.pos.y;
            line.thick       = 1;
            line.update_line_vertices();
            line.draw(view, proj);
        }
    }
};