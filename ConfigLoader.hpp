#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct AppConfig {
    int request_delay_sec = 2;
    int max_http_retries = 3;
    std::string log_file = "identity_parser.log";
    std::string log_level = "INFO";
};

AppConfig load_config();
void printUsage(const char* programName);
bool parseArguments(int argc, char* argv[], std::string& gender, std::string& country, std::string& nameSet, int& amount, const AppConfig& config, std::vector<std::string>& errors); 