cmake_minimum_required(VERSION 3.21)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES
        src/main.cpp
)
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/requests.json ${CMAKE_BINARY_DIR}/requests.json COPYONLY)
include_directories(include)

add_subdirectory(nlohmann_json)
add_executable(search_engine src/main.cpp
        include/converterJSON.h
        src/converterJSON.cpp
        src/InvertedIndex.cpp
        src/SearchServer.cpp
        include/InvertedIndex.h
        include/SearchServer.h)

# Линковка с библиотекой
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)