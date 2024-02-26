#include "utils.h"

#include <iostream>

Timer::Timer(std::string operation) : operation_(std::move(operation)),
                                      start_(std::chrono::high_resolution_clock::now()) {}

Timer::~Timer() {
    std::cout << operation_ << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_).count() << "ms" << std::endl;
}
