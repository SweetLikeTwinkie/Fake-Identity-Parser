#include "ConfigLoader.hpp"
#include <cstdlib>
#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>

AppConfig load_config() {
    AppConfig config;
    std::ifstream f("config.json");
    if (f) {
        try {
            nlohmann::json j;
            f >> j;
            if (j.contains("request_delay_sec")) config.request_delay_sec = j["request_delay_sec"].get<int>();
            if (j.contains("max_http_retries")) config.max_http_retries = j["max_http_retries"].get<int>();
            if (j.contains("log_file")) config.log_file = j["log_file"].get<std::string>();
            if (j.contains("log_level")) config.log_level = j["log_level"].get<std::string>();
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to parse config.json, using defaults" << std::endl;
        }
    }
    if (std::getenv("REQUEST_DELAY_SEC")) {
        try { config.request_delay_sec = std::stoi(std::getenv("REQUEST_DELAY_SEC")); } catch (...) {
            std::cerr << "Error: Invalid REQUEST_DELAY_SEC environment variable. Using default value." << std::endl;
        }
    }
    if (std::getenv("MAX_HTTP_RETRIES")) {
        try { config.max_http_retries = std::stoi(std::getenv("MAX_HTTP_RETRIES")); } catch (...) {
            std::cerr << "Error: Invalid MAX_HTTP_RETRIES environment variable. Using default value." << std::endl;
        }
    }
    if (config.log_level != "INFO" && config.log_level != "WARNING" && config.log_level != "ERROR" && config.log_level != "DEBUG") {
        std::cerr << "Error: Invalid log_level value '" << config.log_level << "'. Must be one of: INFO, WARNING, ERROR, DEBUG" << std::endl;
        exit(1);
    }
    return config;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --gender [male|female|random]     Gender of the identity (default: male)\n";
    std::cout << "  --country [US|UK|CA|AU|DE|FR|IT|ES]  Country code (default: US)\n";
    std::cout << "  --nameSet [common|arabic|chinese|japanese|russian]  Name set (default: common)\n";
    std::cout << "  --amount N                        Number of identities to generate (default: 1, max: 100)\n";
    std::cout << "  --randomize                       Randomize gender, country, and name set\n";
    std::cout << "  --randomGender                    Randomize gender\n";
    std::cout << "  --randomCountry                   Randomize country\n";
    std::cout << "  --randomNameSet                   Randomize name set\n";
    std::cout << "  --help                            Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --amount 5 --gender female --country US\n";
    std::cout << "  " << programName << " --randomize --amount 10\n";
}

bool parseArguments(int argc, char* argv[], std::string& gender, std::string& country, std::string& nameSet, int& amount, const AppConfig& config, std::vector<std::string>& errors) {
    std::vector<std::string> validGenders = {"male", "female", "random"};
    std::vector<std::string> validCountries = {"US", "UK", "CA", "AU", "DE", "FR", "IT", "ES"};
    std::vector<std::string> validNameSets = {"common", "arabic", "chinese", "japanese", "russian"};
    bool randomizeGender = false, randomizeCountry = false, randomizeNameSet = false;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printUsage(argv[0]);
            return false;
        } else if (arg == "--gender" && i + 1 < argc) {
            gender = argv[++i];
            if (std::find(validGenders.begin(), validGenders.end(), gender) == validGenders.end())
                errors.push_back("Invalid gender option '" + gender + "'. Use male, female, or random.");
        } else if (arg == "--country" && i + 1 < argc) {
            country = argv[++i];
            if (std::find(validCountries.begin(), validCountries.end(), country) == validCountries.end())
                errors.push_back("Invalid country option '" + country + "'.");
        } else if (arg == "--nameSet" && i + 1 < argc) {
            nameSet = argv[++i];
            if (std::find(validNameSets.begin(), validNameSets.end(), nameSet) == validNameSets.end())
                errors.push_back("Invalid name set option '" + nameSet + "'.");
        } else if (arg == "--amount" && i + 1 < argc) {
            try {
                amount = std::stoi(argv[++i]);
                if (amount <= 0) errors.push_back("Amount must be a positive integer.");
                if (amount > 100) errors.push_back("Amount cannot exceed 100 for rate limiting reasons.");
            } catch (...) {
                errors.push_back("Invalid amount '" + std::string(argv[i]) + "'. Must be a positive integer.");
            }
        } else if (arg == "--randomize") {
            randomizeGender = randomizeCountry = randomizeNameSet = true;
        } else if (arg == "--randomGender") {
            randomizeGender = true;
        } else if (arg == "--randomCountry") {
            randomizeCountry = true;
        } else if (arg == "--randomNameSet") {
            randomizeNameSet = true;
        } else {
            errors.push_back("Unknown argument: " + arg);
        }
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    if (randomizeGender) {
        std::uniform_int_distribution<> dis(0, validGenders.size() - 1);
        gender = validGenders[dis(gen)];
    }
    if (randomizeCountry) {
        std::uniform_int_distribution<> dis(0, validCountries.size() - 1);
        country = validCountries[dis(gen)];
    }
    if (randomizeNameSet) {
        std::uniform_int_distribution<> dis(0, validNameSets.size() - 1);
        nameSet = validNameSets[dis(gen)];
    }
    return errors.empty();
} 