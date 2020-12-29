cmake_minimum_required(VERSION 3.17)
project(StrangeGameEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(include/SGE)

add_library(StrangeGameEngine
        include/SGE/api.h
        include/SGE/debug.h
        include/SGE/display.h
        include/SGE/fileformats.h
        include/SGE/gui.h
        include/SGE/inputs.h
        include/SGE/render.h
        include/SGE/sge.h
        include/SGE/sound.h
        include/SGE/system.h
        include/SGE/utility.h
        debug.cpp
        display.cpp
        fileformats.cpp
        gui.cpp
        inputs.cpp
        render.cpp
        sge.cpp
        sound.cpp
        system.cpp
        utility.cpp)

add_executable(SGETest
        test/main.cpp
        test/main.h)

#OpenGL in general is required
find_package(OpenGL REQUIRED)
target_link_libraries(StrangeGameEngine OpenGL::GL)

#Find GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(StrangeGameEngine glfw)

find_package(rtaudio REQUIRED)
target_link_libraries(StrangeGameEngine rtaudiod)

if(WIN32)
    target_link_libraries(StrangeGameEngine mingw32)
endif(WIN32)

target_link_libraries(SGETest StrangeGameEngine)


#Copy test files to build directory
configure_file(assets/TestSample.wav TestSample.wav COPYONLY)
configure_file(assets/TestImage.bmp TestImage.bmp COPYONLY)
configure_file(assets/hyper.mod test1.mod COPYONLY)
configure_file(assets/enigma.mod test2.mod COPYONLY)
configure_file(assets/stardstm.mod test3.mod COPYONLY)



