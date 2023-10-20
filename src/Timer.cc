//
// Created by powew on 20/10/2023.
//

#include <GLFW/glfw3.h>
#include "GravSim/Timer.hh"

float Timer::tick() {
    double now = glfwGetTime();
    dt = now - lastTime;
    lastTime = now;
    return dt;
}
