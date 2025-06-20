cmake_minimum_required(VERSION 3.10)
project(fake_identity_parser)
set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PUGIXML REQUIRED pugixml)
find_package(nlohmann_json REQUIRED)

include_directories(
    ${CURL_INCLUDE_DIRS}
    ${PUGIXML_INCLUDE_DIRS}
    ${nlohmann_json_INCLUDE_DIRS}
)

add_executable(fake_identity_parser
    identity_parser.cpp
    ConfigLoader.cpp
    Logger.cpp
    HttpClient.cpp
    HtmlParser.cpp
    JsonWriter.cpp
)

target_link_libraries(fake_identity_parser
    ${CURL_LIBRARIES}
    ${PUGIXML_LIBRARIES}
    nlohmann_json::nlohmann_json
) 