#pragma once
#include <string>
#include <memory>
#include <spdlog/logger.h>
#include <GLFW/glfw3.h>


class Window {
public:
    Window(int width, int height, const std::string &title);
    GLFWwindow* window;

    void run();

protected:
    std::shared_ptr<spdlog::logger> logger;

private:
    static inline bool glfw_initialised =false;
    static void Init();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

};
