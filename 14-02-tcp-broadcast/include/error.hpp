#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>

template <typename... Args>
void log_info(Args&&... args) {
    std::cout << "[INFO]  ";
    (std::cout << ... << args) << "\n";
}

template <typename... Args>
void log_warn(Args&&... args) {
    std::cout << "[WARN]  ";
    (std::cout << ... << args) << "\n";
}

template <typename... Args>
void log_err(int err, Args&&... args) {
    std::cout << "[ERROR] ";
    (std::cout << ... << args) << ": " << strerror(err) << " (" << err << ")\n";
}
