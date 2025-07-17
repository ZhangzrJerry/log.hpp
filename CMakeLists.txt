cmake_minimum_required(VERSION 3.10)
project(logger_example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For Windows, show console
if(WIN32)
    add_compile_definitions(_WIN32)
endif()

# Create the executable
add_executable(logger_example
    example.cpp
    log.hpp
)

# Set target properties
target_include_directories(logger_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# On Windows, link with kernel32 for console functions
if(WIN32)
    target_link_libraries(logger_example PRIVATE kernel32)
endif()

# Install target (optional)
install(TARGETS logger_example DESTINATION bin)