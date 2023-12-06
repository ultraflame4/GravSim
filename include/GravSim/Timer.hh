//
// Created by powew on 20/10/2023.
//

#pragma once

class Timer {
public:
    double lastTime = 0;
    float delta = 0;
    float avg_delta = 0;
    int average_factor = 100;
    float tick(bool reset=false);
private:
    void calc_avg();
};