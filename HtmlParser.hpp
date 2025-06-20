#pragma once
#include <string>
#include <vector>
#include <tuple>

std::string trim(const std::string& str);
std::string convertStateAbbreviationToFullName(const std::string& abbreviation);
std::string cleanHTML(const std::string& htmlContent);
std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>> parseHTML(const std::string& htmlContent, const std::string& gender, const std::string& country, const std::string& nameSet); 