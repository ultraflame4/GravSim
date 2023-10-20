//
// Created by powew on 20/10/2023.
//

#pragma once

class Timer {
    double lastTime = 0;
    float dt = 0;

public:
    float tick();
};