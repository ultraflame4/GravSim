#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GravSim/window.hh"
#include "GravSim/logging.hh"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->width = width;
    this->height = height;
    glfwSetFramebufferSizeCallback(window, static_framebuffer_size_callback);
    glfwSetKeyCallback(window, static_key_callback);
    windows_list[window] = this;

    InitIMGUI();
}

void Window::static_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    windows_list[window]->framebuffer_size_callback(window, width, height);
}
void Window::static_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    windows_list[window]->OnInput(key,scancode,action,mods);
}

void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
    this->OnResize();
}

void Window::run() {
    framebuffer_size_callback(window, width, height); // Call once at the start
    Load();
    frameTimer.tick();
    updateTimer.tick();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        for (int i = 0; i < 20; ++i) {
            Update(updateTimer.tick());
        }

        glClearColor(.0f, .0f, .0f, .0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Draw(frameTimer.tick());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        OnImGui_Draw();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}
