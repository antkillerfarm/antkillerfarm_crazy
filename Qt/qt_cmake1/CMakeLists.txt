cmake_minimum_required(VERSION 2.8.8)
project(hello-world)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)

qt5_wrap_ui(ui_FILES src/mainwindow.ui)

add_executable(qt_cmake src/main.cpp src/mainwindow.cpp ${ui_FILES})

qt5_use_modules(qt_cmake Widgets)