#include "JsonWriter.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Logger.hpp"

void writeIdentitiesToJson(const std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>>& identities) {
    nlohmann::json j = nlohmann::json::array();
    for (const auto& identity : identities) {
        nlohmann::json item;
        item["name"] = std::get<0>(identity);
        item["address"] = std::get<1>(identity);
        item["city"] = std::get<2>(identity);
        item["state"] = std::get<3>(identity);
        item["zip_code"] = std::get<4>(identity);
        item["email"] = std::get<5>(identity);
        item["phone"] = std::get<6>(identity);
        item["username"] = std::get<7>(identity);
        item["password"] = std::get<8>(identity);
        item["latitude"] = std::get<9>(identity);
        item["longitude"] = std::get<10>(identity);
        item["user_agent"] = std::get<11>(identity);
        item["gender"] = std::get<12>(identity);
        item["country"] = std::get<13>(identity);
        item["name_set"] = std::get<14>(identity);
        j.push_back(item);
    }
    auto t = std::time(nullptr);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << "identities_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".json";
    std::string filename = oss.str();
    std::ofstream out(filename);
    if (out) {
        out << j.dump(4);
        log(LogLevel::INFO, "Wrote all identities to " + filename);
    } else {
        log(LogLevel::ERROR, "Failed to write identities to " + filename);
    }
} 