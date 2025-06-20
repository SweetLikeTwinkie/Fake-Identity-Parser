#include "Logger.hpp"
#include "ConfigLoader.hpp"
#include <iostream>
#include <csignal>
#include <mutex>

std::ofstream log_stream;
volatile std::sig_atomic_t stop_flag = 0;

void log(LogLevel level, const std::string& message) {
    static bool initialized = false;
    static AppConfig* config_ptr = nullptr;
    static std::mutex log_mutex;
    std::lock_guard<std::mutex> lock(log_mutex);
    if (!initialized) {
        if (config_ptr) {
            log_stream.open(config_ptr->log_file, std::ios::app);
        }
        initialized = true;
    }
    int min_level = static_cast<int>(LogLevel::INFO);
    if (config_ptr) {
        if (config_ptr->log_level == "ERROR") min_level = static_cast<int>(LogLevel::ERROR);
        else if (config_ptr->log_level == "WARNING") min_level = static_cast<int>(LogLevel::WARNING);
        else if (config_ptr->log_level == "DEBUG") min_level = static_cast<int>(LogLevel::DEBUG);
    }
    if (static_cast<int>(level) < min_level) return;
    const char* level_str = "[INFO]";
    switch (level) {
        case LogLevel::INFO: level_str = "[INFO]"; break;
        case LogLevel::WARNING: level_str = "[WARNING]"; break;
        case LogLevel::ERROR: level_str = "[ERROR]"; break;
        case LogLevel::DEBUG: level_str = "[DEBUG]"; break;
    }
    std::ostream& out = (level == LogLevel::ERROR || level == LogLevel::WARNING) ? std::cerr : std::cout;
    out << level_str << " " << message << std::endl;
    if (log_stream.is_open()) {
        log_stream << level_str << " " << message << std::endl;
    }
}

void set_logger_config(AppConfig* config) {
    static AppConfig* config_ptr = config;
    if (config_ptr && !log_stream.is_open()) {
        log_stream.open(config_ptr->log_file, std::ios::app);
    }
}

void handle_signal(int signal) {
    stop_flag = 1;
    log(LogLevel::INFO, "Received termination signal. Cleaning up and exiting...");
    if (log_stream.is_open()) {
        log_stream.close();
    }
} 