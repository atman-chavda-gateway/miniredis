
cmake_minimum_required(VERSION 3.25)

project(miniredis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(miniredis
    src/main.cpp
)

target_include_directories(miniredis
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/include
)

target_compile_definitions(miniredis
    PRIVATE
        ASIO_STANDALONE
        ASIO_NO_DEPRECATED
        _WIN32_WINNT=0x0A00
)

target_link_libraries(miniredis
    PRIVATE
        Threads::Threads
        ws2_32
        mswsock
)

if(MSVC)
    target_compile_options(miniredis PRIVATE /W4)
else()
    target_compile_options(miniredis PRIVATE -Wall -Wextra -Wpedantic)
endif()
