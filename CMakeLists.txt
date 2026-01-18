cmake_minimum_required(VERSION 4.1)
project(nbomed_win)

set(CMAKE_CXX_STANDARD 98)

add_executable(nbomed_win
        nbomed-core/display.cpp
        nbomed-core/display.h
        nbomed-core/application.cpp
        nbomed-core/application.h
        platform.cpp
        platform.h
        main.cpp
        nbomed-core/common.h
        common.cpp
        nbomed-core/input.cpp
        nbomed-core/input.h
        nbomed-core/gadgets/TextGadget.cpp
        nbomed-core/gadgets/TextGadget.h
        nbomed-core/gadgets/TestGadget.cpp
        nbomed-core/gadgets/TestGadget.h
)

# Добавляем текущую директорию (корень) в пути поиска
# CMAKE_CURRENT_SOURCE_DIR указывает на папку, где лежит этот CMakeLists.txt
target_include_directories(nbomed_win PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")