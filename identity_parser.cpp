/**
 * @file identity_parser.cpp
 * @brief Fake Identity Parser - Scrapes and stores fake identity data from fakenamegenerator.com
 * @author SweetLikeTwinkie
 * @version 2.0
 * @date 2025-06-20
 * 
 * This application scrapes fake identity data from fakenamegenerator.com and stores it
 * in a MySQL database. It includes features like rate limiting, retry logic, and
 * configurable settings via JSON config file or environment variables.
 * 
 * SECURITY NOTICE: This tool is for development and testing purposes only.
 * Do not use in production environments without proper security review.
 */

#include "ConfigLoader.hpp"
#include "Logger.hpp"
#include "HttpClient.hpp"
#include "HtmlParser.hpp"
#include "JsonWriter.hpp"
#include <csignal>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
    AppConfig global_config;
    try {
        global_config = load_config();
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to load configuration: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    set_logger_config(&global_config);
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    log(LogLevel::INFO, "Starting Fake Identity Parser v2.0");
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        log(LogLevel::ERROR, "CURL Global initialization failed");
        return EXIT_FAILURE;
    }
    std::string gender = "male";
    std::string country = "US";
    std::string nameSet = "common";
    int amount = 1;
    std::vector<std::string> parseErrors;
    if (!parseArguments(argc, argv, gender, country, nameSet, amount, global_config, parseErrors)) {
        for (const auto& error : parseErrors) {
            std::cerr << "Error: " << error << std::endl;
        }
        std::cerr << std::endl;
        printUsage(argv[0]);
        curl_global_cleanup();
        return EXIT_FAILURE;
    }
    log(LogLevel::DEBUG, "Gender: " + gender);
    log(LogLevel::DEBUG, "Country: " + country);
    log(LogLevel::DEBUG, "Name Set: " + nameSet);
    log(LogLevel::DEBUG, "Amount: " + std::to_string(amount));
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>> all_identities;
    for (int i = 0; i < amount; i++) {
        if (stop_flag) {
            log(LogLevel::INFO, "Received termination signal, stopping gracefully");
            break;
        }
        std::string url = constructURL(gender, country, nameSet);
        log(LogLevel::DEBUG, "Generated URL: " + url);
        std::string htmlContent = fetchHTML(url, global_config);
        std::string cleanedHTMLContent = cleanHTML(htmlContent);
        if (!cleanedHTMLContent.empty()) {
            auto identities = parseHTML(cleanedHTMLContent, gender, country, nameSet);
            all_identities.insert(all_identities.end(), identities.begin(), identities.end());
        } else {
            log(LogLevel::ERROR, "Failed to retrieve HTML content for Identity " + std::to_string(i+1));
        }
        if (i < amount - 1) {
            log(LogLevel::INFO, "Waiting " + std::to_string(global_config.request_delay_sec) + " seconds before next request...");
            std::this_thread::sleep_for(std::chrono::seconds(global_config.request_delay_sec));
        }
    }
    if (!all_identities.empty()) {
        writeIdentitiesToJson(all_identities);
    } else {
        log(LogLevel::WARNING, "No identities were parsed, no JSON file created.");
    }
    log(LogLevel::INFO, "Application completed successfully");
    if (log_stream.is_open()) {
        log_stream.close();
    }
    curl_global_cleanup();
    return EXIT_SUCCESS;
}