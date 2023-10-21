//
// Created by powew on 20/10/2023.
//

#include <GLFW/glfw3.h>
#include "GravSim/Timer.hh"

float Timer::tick() {
    double now = glfwGetTime();
    delta = now - lastTime;
    lastTime = now;
//    ticks += 1;
    calc_avg();
    return delta;
}

void Timer::calc_avg() {
    avg_delta = avg_delta + (delta - avg_delta) / average_factor;
}
