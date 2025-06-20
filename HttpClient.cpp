#include "HttpClient.hpp"
#include "Logger.hpp"
#include <thread>
#include <chrono>

CurlHandle::CurlHandle() : handle_(nullptr) {}
CurlHandle::~CurlHandle() { if (handle_) curl_easy_cleanup(handle_); }
CurlHandle::CurlHandle(CurlHandle&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
CurlHandle& CurlHandle::operator=(CurlHandle&& other) noexcept { if (this != &other) { if (handle_) curl_easy_cleanup(handle_); handle_ = other.handle_; other.handle_ = nullptr; } return *this; }
bool CurlHandle::init() { handle_ = curl_easy_init(); return handle_ != nullptr; }
CURL* CurlHandle::get() const { return handle_; }
CurlHandle::operator bool() const { return handle_ != nullptr; }

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try { s->append((char*)contents, newLength); } catch (...) { return 0; }
    return newLength;
}

std::string constructURL(const std::string& gender, const std::string& country, const std::string& nameSet) {
    std::string baseURL = "https://www.fakenamegenerator.com/gen-";
    if (nameSet == "common") baseURL += "random"; else baseURL += nameSet;
    baseURL += "-";
    if (gender == "male") baseURL += "male";
    else if (gender == "female") baseURL += "female";
    else baseURL += "random";
    baseURL += "-" + country + "-en.php";
    return baseURL;
}

std::string fetchHTML(const std::string& url, const AppConfig& config) {
    std::string readBuffer;
    int max_retries = config.max_http_retries;
    int attempt = 0;
    while (attempt < max_retries) {
        readBuffer.clear();
        CurlHandle curl;
        if (curl.init()) {
            curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 30L);
            curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl.get(), CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
            curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYHOST, 2L);
            CURLcode res = curl_easy_perform(curl.get());
            if (res == CURLE_OK) {
                if (!readBuffer.empty()) {
                    log(LogLevel::DEBUG, "Received HTML length: " + std::to_string(readBuffer.length()));
                    log(LogLevel::DEBUG, "HTML preview: " + readBuffer.substr(0, 200));
                } else {
                    log(LogLevel::WARNING, "Received empty HTML response");
                }
                break;
            } else {
                log(LogLevel::ERROR, "HTTP Request failed (attempt " + std::to_string(attempt+1) + "/" + std::to_string(max_retries) + "): " + curl_easy_strerror(res));
                std::this_thread::sleep_for(std::chrono::seconds(config.request_delay_sec));
            }
        } else {
            log(LogLevel::ERROR, "Failed to initialize CURL (attempt " + std::to_string(attempt+1) + "/" + std::to_string(max_retries) + ")");
            std::this_thread::sleep_for(std::chrono::seconds(config.request_delay_sec));
        }
        attempt++;
    }
    return readBuffer;
} 