#pragma once

#include <string>
#include <memory>
#include <spdlog/logger.h>
#include <GLFW/glfw3.h>
#include <map>
#include "Timer.hh"


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

    Timer frameTimer;
    Timer updateTimer;

    virtual void OnResize() {};

    virtual void Load() {};

    virtual void Update(float dt) {};

    virtual void Draw(float dt) {};

private:


    static inline bool glfw_initialised = false;

    static void Init();

    static void static_framebuffer_size_callback(GLFWwindow *window, int width, int height);

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

};
