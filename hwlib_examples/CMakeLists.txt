cmake_minimum_required(VERSION 3.2)
project(IPASS)

add_custom_target(
        BUILD ALL
        COMMAND bmptk/tools/bmptk-make.exe build
)

add_custom_target(
        UPLOAD ALL
        COMMAND bmptk/tools/bmptk-make.exe run
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
        CLEAN ALL
        COMMAND ../bmptk/tools/bmptk-make.exe clean
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/IPASS
)


add_executable(IPASS IPASS/main.cpp IPASS/Serial.hpp test/Serial.h)