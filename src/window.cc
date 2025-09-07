#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include "GravSim/window.hh"
#include "GravSim/logging.hh"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void Window::Init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfw_initialised = true;
}

Window::Window(int width, int height, const std::string& title) {
    if (!glfw_initialised) Init();
    logger = logging::get<Window>(title);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        logger->critical("Failed to create GLFW window");
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window!");
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logger->critical("Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD!");
    }

    glViewport(0, 0, width, height);
    glEnable(GL_PROGRAM_POINT_SIZE);
    CheckGLErrors();
    glEnable(GL_MULTISAMPLE);
    CheckGLErrors();
    glEnable(GL_BLEND);
    CheckGLErrors();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    glfwSwapInterval(0);
    CheckGLErrors();
    this->width  = width;
    this->height = height;
    glfwSetFramebufferSizeCallback(window, static_framebuffer_size_callback);
    glfwSetKeyCallback(window, static_key_callback);
    glfwSetMouseButtonCallback(window, static_mousebtn_callback);
    glfwSetScrollCallback(window, static_scroll_callback);

    windows_list[window] = this;

    InitIMGUI();
    CheckGLErrors();
}

void Window::static_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    windows_list[window]->framebuffer_size_callback(window, width, height);
}

void Window::static_key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
) {
    windows_list[window]->OnInput(key, action, mods);
}

void Window::static_mousebtn_callback(GLFWwindow* window, int key, int action, int mods) {
    windows_list[window]->OnInput(key, action, mods);
}

void Window::static_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    windows_list[window]->OnScroll(xoffset, yoffset);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    this->width  = width;
    this->height = height;
    glViewport(0, 0, width, height);
    this->OnResize();
}

void Window::_update_thread() {
    const int base_tps = this->updateTps;
    const float delay  = 1.0f / base_tps;

    float next_update = 0;
    while (is_running) {
        float now = glfwGetTime();
        if (next_update > now) { continue; }
        Update(updateTimer.tick());
        next_update = now + delay;
    }
}

void Window::run() {
    framebuffer_size_callback(window, width, height);  // Call once at the start
    Load();
    frameTimer.tick();
    updateTimer.tick();
    is_running    = true;
    pUpdateThread = new std::thread(&Window::_update_thread, this);

    Start();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(.0f, .0f, .0f, .0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Draw(frameTimer.tick());
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        OnImGui_Draw();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        CheckGLErrors();
        glfwSwapBuffers(window);
    }

    is_running = false;
    pUpdateThread->join();
    delete pUpdateThread;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void Window::InitIMGUI() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(
        window,
        true
    );  // Second param install_callback=true will install GLFW callbacks
        // and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

std::shared_ptr<spdlog::logger> logger = logging::get("OpenGl");

GLenum CheckGLErrors_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        logger->error("Error code {} : {} at \"{}\" line {}", errorCode, error, file, line);
    }
    return errorCode;
}