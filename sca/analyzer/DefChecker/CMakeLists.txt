cmake_minimum_required(VERSION 3.5)

project(DefChecker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(AnalyzerLibrary)

add_executable(DefChecker main.cpp)

target_link_libraries(DefChecker AnalyzerLibrary)

#target_link_directories(DefChecker PRIVATE ${projectDir}\\out\\build\\AnalyzerLibrary)

install(TARGETS DefChecker)
