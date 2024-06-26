cmake_minimum_required(VERSION 3.5)

project(CodeEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS QuickWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS QuickWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        socketclient.h socketclient.cpp
        codetextedit.h codetextedit.cpp
        constants.h
        celistview.h celistview.cpp
        cemodel.h cemodel.cpp
        jsonutils.h jsonutils.cpp
        basicblock.h basicblock.cpp
        visitor.h visitor.cpp
        positionblock.h positionblock.cpp
        cfgmodel.h cfgmodel.cpp
        cfgedge.h cfgedge.cpp
        edgevisitor.h edgevisitor.cpp
        edge.h edge.cpp
        edgesmodel.h edgesmodel.cpp
        qml.qrc
        images.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CodeEditor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CodeEditor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CodeEditor SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CodeEditor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

qt_add_qml_module(CodeEditor
    URI com.model.cfgModel
    VERSION 1.0
    SOURCES
        cfgmodel.cpp cfgmodel.h
        cfgedge.cpp cfgedge.h
        edgesmodel.cpp edgesmodel.h
)

target_link_libraries(CodeEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(CodeEditor PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(CodeEditor PRIVATE Qt${QT_VERSION_MAJOR}::QuickWidgets)

set_target_properties(CodeEditor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS CodeEditor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CodeEditor)
endif()
