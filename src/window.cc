#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GravSim/window.hh"
#include "GravSim/logging.hh"


void Window::Init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfw_initialised = true;
}

Window::Window(int width, int height, const std::string &title) {
    if (!glfw_initialised) Init();
    logger = logging::get<Window>(title);


    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        logger->critical("Failed to create GLFW window");
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window!");
    }


    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        logger->critical("Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD!");
    }

    glViewport(0, 0, width, height);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->width = width;
    this->height = height;
    glfwSetFramebufferSizeCallback(window, static_framebuffer_size_callback);
    windows_list[window] = this;
}

void Window::static_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    windows_list[window]->framebuffer_size_callback(window, width, height);
}

void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
}

void Window::run() {
    Load();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Update();
        glClearColor(.0f, .0f, .0f, .0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Draw();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}
