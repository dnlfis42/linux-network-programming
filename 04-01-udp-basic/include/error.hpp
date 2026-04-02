#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>

template <typename... Args>
void log_info(Args&&... args) {
    (std::cout << ... << args) << "\n";
}

template <typename... Args>
void log_err(Args&&... args) {
    int err = errno;
    (std::cerr << ... << args) << ": " << strerror(err) << " (" << err << ")\n";
}
