cmake_minimum_required(VERSION 3.1.0)

project(helloworld)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

add_executable(helloworld
    src/window.h
    src/window.cpp
    src/main.cpp
    src/abstractscene.h
    src/basicusagescene.h
    src/glassert.h
    src/basicusagescene.cpp
    src/resources.qrc
)

target_link_libraries(helloworld Qt5::OpenGL Qt5::Widgets GL)
