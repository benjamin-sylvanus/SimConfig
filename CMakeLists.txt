cmake_minimum_required(VERSION 3.5)

project(SimConfig VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "/Users/benjaminsylvanus/Qt/6.5.0/macos/lib/cmake/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui OpenGL Charts Network Widgets OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui OpenGL Charts Network Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        dialogconfig.cpp
        dialogconfig.h
        dialogconfig.ui
        node.cpp
        node.h
        configuration.h
        configuration.cpp
        util.h
        util.cpp
        tree.h
        tree.cpp
        tree.ui
        viewer.h
        viewer.cpp
        viewer.ui
        glwidget.h
        glwidget.cpp
        datatemplate.h
        datatemplate.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SimConfig
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SimConfig APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SimConfig SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SimConfig
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(SimConfig PRIVATE Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::Widgets OpenGL::GLU Qt6::Charts Qt6::Network Qt6::OpenGLWidgets ${GLUT_LIBRARIES})

set_target_properties(SimConfig PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS SimConfig
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SimConfig)
endif()
