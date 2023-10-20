#pragma once
#include <string>
#include <memory>
#include <spdlog/logger.h>


class Window {
public:
    Window(int width, int height, const std::string &title);

protected:
    std::shared_ptr<spdlog::logger> logger;

private:
    static inline bool glfw_initialised =false;
    static void Init();
};
