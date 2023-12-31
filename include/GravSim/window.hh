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
    Timer frameTimer;
    Timer updateTimer;

    int width;
    int height;

protected:
    ~Window() = default;

    std::shared_ptr<spdlog::logger> logger;
    bool is_running = false;
    std::thread* pUpdateThread = nullptr;

    virtual void OnResize() {};
    virtual void OnInput(int key, int action, int mods) {};
    virtual void OnScroll( double x_offset, double y_offset) {};
    virtual void OnImGui_Draw() {};

    virtual void Load() {};

    virtual void Update(float dt) {};

    virtual void Draw(float dt) {};

private:


    static inline bool glfw_initialised = false;

    static void Init();

    static void static_framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void static_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void static_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

    void InitIMGUI();

    static void static_mousebtn_callback(GLFWwindow* window, int key, int action, int mods);

    void _update_thread();

};
GLenum CheckGLErrors_(const char *file, int line);
#define CheckGLErrors() CheckGLErrors_(__FILE__,__LINE__)