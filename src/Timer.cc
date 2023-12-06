//
// Created by powew on 20/10/2023.
//

#include <GLFW/glfw3.h>
#include "GravSim/Timer.hh"

float Timer::tick(bool reset) {
    double now = glfwGetTime();
    if(!reset) {
        delta = now - lastTime;
        calc_avg();
    }
    lastTime = now;
    return delta;
}

void Timer::calc_avg() {
    avg_delta = avg_delta + (delta - avg_delta) / average_factor;
}
