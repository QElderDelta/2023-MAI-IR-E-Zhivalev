#pragma once

#include <chrono>
#include <string>

class Timer {
public:
    Timer(std::string operation);
    ~Timer();
private:
    std::string operation_;
    std::chrono::high_resolution_clock::time_point start_;
};
