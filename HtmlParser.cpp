#include "HtmlParser.hpp"
#include "Logger.hpp"
#include <pugixml.hpp>
#include <unordered_map>
#include <sstream>


std::string trim(const std::string& str) {
    if (str.empty()) return str;
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;
    if (start == str.end()) return "";
    auto end = str.end();
    do { end--; } while (end > start && std::isspace(*end));
    return std::string(start, end + 1);
}

std::string convertStateAbbreviationToFullName(const std::string& abbreviation) {
    static const std::unordered_map<std::string, std::string> stateAbbreviationToName = {
        {"AL", "Alabama"}, {"AK", "Alaska"}, {"AZ", "Arizona"}, {"AR", "Arkansas"},
        {"CA", "California"}, {"CO", "Colorado"}, {"CT", "Connecticut"}, {"DE", "Delaware"},
        {"FL", "Florida"}, {"GA", "Georgia"}, {"HI", "Hawaii"}, {"ID", "Idaho"},
        {"IL", "Illinois"}, {"IN", "Indiana"}, {"IA", "Iowa"}, {"KS", "Kansas"},
        {"KY", "Kentucky"}, {"LA", "Louisiana"}, {"ME", "Maine"}, {"MD", "Maryland"},
        {"MA", "Massachusetts"}, {"MI", "Michigan"}, {"MN", "Minnesota"}, {"MS", "Mississippi"},
        {"MO", "Missouri"}, {"MT", "Montana"}, {"NE", "Nebraska"}, {"NV", "Nevada"},
        {"NH", "New Hampshire"}, {"NJ", "New Jersey"}, {"NM", "New Mexico"}, {"NY", "New York"},
        {"NC", "North Carolina"}, {"ND", "North Dakota"}, {"OH", "Ohio"}, {"OK", "Oklahoma"},
        {"OR", "Oregon"}, {"PA", "Pennsylvania"}, {"RI", "Rhode Island"}, {"SC", "South Carolina"},
        {"SD", "South Dakota"}, {"TN", "Tennessee"}, {"TX", "Texas"}, {"UT", "Utah"},
        {"VT", "Vermont"}, {"VA", "Virginia"}, {"WA", "Washington"}, {"WV", "West Virginia"},
        {"WI", "Wisconsin"}, {"WY", "Wyoming"}
    };
    auto it = stateAbbreviationToName.find(abbreviation);
    if (it != stateAbbreviationToName.end()) return it->second;
    return abbreviation;
}

std::string cleanHTML(const std::string& htmlContent) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(htmlContent.c_str());
    if (!result) return htmlContent;
    pugi::xpath_node_set script_nodes = doc.select_nodes("//script");
    for (auto it = script_nodes.begin(); it != script_nodes.end(); ++it) it->node().parent().remove_child(it->node());
    pugi::xpath_node_set style_nodes = doc.select_nodes("//style");
    for (auto it = style_nodes.begin(); it != style_nodes.end(); ++it) it->node().parent().remove_child(it->node());
    pugi::xpath_node_set meta_nodes = doc.select_nodes("//meta");
    for (auto it = meta_nodes.begin(); it != meta_nodes.end(); ++it) it->node().parent().remove_child(it->node());
    pugi::xpath_node_set link_nodes = doc.select_nodes("//link");
    for (auto it = link_nodes.begin(); it != link_nodes.end(); ++it) it->node().parent().remove_child(it->node());
    std::ostringstream oss;
    doc.save(oss, "", pugi::format_default, pugi::encoding_utf8);
    return oss.str();
}

std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>> parseHTML(const std::string& htmlContent, const std::string& gender, const std::string& country, const std::string& nameSet) {
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>> identities;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(htmlContent.c_str());
    if (!result) {
        log(LogLevel::ERROR, "HTML Failed to parse HTML content: " + std::string(result.description()));
        return identities;
    }
    
    // Ищем имя в новой структуре
    pugi::xpath_node name_node = doc.select_node("//div[@class='address']/h3");
    if (name_node) {
        std::string name = trim(name_node.node().child_value());
        log(LogLevel::DEBUG, "Parsed Name: " + name);
        
        // Ищем адрес
        pugi::xpath_node adr_node = doc.select_node("//div[@class='address']/div[@class='adr']");
        if (adr_node) {
            std::string address, city, state, zip_code;
            pugi::xml_node adr_content = adr_node.node();
            std::vector<std::string> address_lines;
            
            for (pugi::xml_node child : adr_content.children()) {
                if (child.type() == pugi::node_pcdata) {
                    std::string text = trim(child.value());
                    if (!text.empty()) address_lines.push_back(text);
                } else if (child.type() == pugi::node_element && std::string(child.name()) == "br") {
                    if (!address_lines.empty()) address_lines.push_back("");
                }
            }
            
            if (address_lines.size() >= 1) {
                address = address_lines[0];
                if (address_lines.size() >= 2) {
                    std::string city_state_zip = address_lines[1];
                    std::size_t comma_pos = city_state_zip.find(',');
                    if (comma_pos != std::string::npos) {
                        city = city_state_zip.substr(0, comma_pos);
                        std::string state_zip = city_state_zip.substr(comma_pos + 1);
                        std::istringstream state_zip_stream(state_zip);
                        state_zip_stream >> state >> zip_code;
                        state = convertStateAbbreviationToFullName(trim(state));
                    } else {
                        city = city_state_zip;
                    }
                }
            }
            
            log(LogLevel::DEBUG, "Parsed Address: " + address);
            log(LogLevel::DEBUG, "Parsed City: " + city);
            log(LogLevel::DEBUG, "Parsed State: " + state);
            log(LogLevel::DEBUG, "Parsed Zip Code: " + zip_code);
            
            // Email
            pugi::xpath_node email_node = doc.select_node("//dt[text()='Email Address']/following-sibling::dd");
            std::string email = email_node ? trim(email_node.node().child_value()) : "";
            log(LogLevel::DEBUG, "Parsed Email: " + email);
            
            // Phone
            pugi::xpath_node phone_node = doc.select_node("//dt[text()='Phone']/following-sibling::dd");
            std::string phone = phone_node ? trim(phone_node.node().child_value()) : "";
            log(LogLevel::DEBUG, "Parsed Phone: " + phone);
            
            // Username
            pugi::xpath_node username_node = doc.select_node("//dt[text()='Username']/following-sibling::dd");
            std::string username = username_node ? trim(username_node.node().child_value()) : "";
            log(LogLevel::DEBUG, "Parsed Username: " + username);
            
            // Password
            pugi::xpath_node password_node = doc.select_node("//dt[text()='Password']/following-sibling::dd");
            std::string password = password_node ? trim(password_node.node().child_value()) : "";
            log(LogLevel::DEBUG, "Parsed Password: [REDACTED]");
            
            // Geo coordinates
            pugi::xpath_node geo_node = doc.select_node("//a[@id='geo']");
            std::string latitude, longitude;
            if (geo_node) {
                std::string geo_coordinates = trim(geo_node.node().child_value());
                std::size_t comma_pos = geo_coordinates.find(',');
                if (comma_pos != std::string::npos) {
                    latitude = trim(geo_coordinates.substr(0, comma_pos));
                    longitude = trim(geo_coordinates.substr(comma_pos + 1));
                }
            }
            log(LogLevel::DEBUG, "Parsed Latitude: " + latitude);
            log(LogLevel::DEBUG, "Parsed Longitude: " + longitude);
            
            // User agent
            pugi::xpath_node ua_node = doc.select_node("//dt[text()='Browser user agent']/following-sibling::dd");
            std::string user_agent = ua_node ? trim(ua_node.node().child_value()) : "";
            log(LogLevel::DEBUG, "Parsed User Agent: " + user_agent);
            
            // Validation
            if (name.empty() || address.empty() || city.empty()) {
                log(LogLevel::WARNING, "Skipping identity with missing required fields");
                return identities;
            }
            
            if (!email.empty() && email.find('@') == std::string::npos) {
                log(LogLevel::WARNING, "Invalid email format: " + email);
                email = "";
            }
            
            if (!phone.empty() && phone.length() < 10) {
                log(LogLevel::WARNING, "Phone number too short: " + phone);
                phone = "";
            }
            
            identities.push_back(std::make_tuple(name, address, city, state, zip_code, email, phone, username, password, latitude, longitude, user_agent, gender, country, nameSet));
        }
    }
    
    return identities;
} 