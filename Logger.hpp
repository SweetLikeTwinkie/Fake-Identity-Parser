#pragma once
#include <string>
#include <csignal>
#include <fstream>
#include "ConfigLoader.hpp"

extern std::ofstream log_stream;
extern volatile sig_atomic_t stop_flag;

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

void log(LogLevel level, const std::string& message);
void set_logger_config(AppConfig* config);
void handle_signal(int signal); 