#pragma once
#include <string>
#include "ConfigLoader.hpp"
#include <curl/curl.h>

class CurlHandle {
private:
    CURL* handle_;
public:
    CurlHandle();
    ~CurlHandle();
    CurlHandle(const CurlHandle&) = delete;
    CurlHandle& operator=(const CurlHandle&) = delete;
    CurlHandle(CurlHandle&& other) noexcept;
    CurlHandle& operator=(CurlHandle&& other) noexcept;
    bool init();
    CURL* get() const;
    explicit operator bool() const;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
std::string constructURL(const std::string& gender, const std::string& country, const std::string& nameSet);
std::string fetchHTML(const std::string& url, const AppConfig& config); 