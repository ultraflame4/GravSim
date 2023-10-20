#pragma once

#include <string>
#include <memory>
#include <spdlog/logger.h>
#include <GLFW/glfw3.h>
#include <map>


class Window {
public:
    Window(int width, int height, const std::string &title);

    GLFWwindow *window;
    static inline std::map<GLFWwindow *, Window *> windows_list{};

    void run();

protected:
    std::shared_ptr<spdlog::logger> logger;
    int width;
    int height;
    float deltaTime = 0;
    double lastFrameTime = 0;

    virtual void OnResize() {};

    virtual void Load() {};

    virtual void Update() {};

    virtual void Draw() {};

private:


    static inline bool glfw_initialised = false;

    static void Init();

    static void static_framebuffer_size_callback(GLFWwindow *window, int width, int height);

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

};
