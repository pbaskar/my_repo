cmake_minimum_required(VERSION 3.14)

project(CodeAnalyzer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

add_subdirectory(AnalyzerLibrary)

add_executable(CodeAnalyzer
  main.cpp
  socketserver.h socketserver.cpp
)

target_link_libraries(CodeAnalyzer Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(CodeAnalyzer Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(CodeAnalyzer AnalyzerLibrary)

install(TARGETS CodeAnalyzer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
