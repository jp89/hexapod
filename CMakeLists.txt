cmake_minimum_required(VERSION 3.5)

project(Hexapod)

file(GLOB SERVO_CONTROLLER_SRC servo_controller/src/*.cpp)
file(GLOB ADAFRUIT_SERVO_DRIVER_SRC adafruit_servo_driver/src/*.cpp)
file(GLOB KINEMATICS_SRC kinematics/src/*.cpp)
file(GLOB UTILS_SRC utils/src/*.cpp)


include_directories(servo_controller/inc
    adafruit_servo_driver/inc
    kinematics/inc
    utils/inc)


find_package (Threads)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libstdc++")

add_executable(Hexapod main.cc ${SERVO_CONTROLLER_SRC}
    ${ADAFRUIT_SERVO_DRIVER_SRC}
    ${UTILS_SRC})
target_link_libraries (Hexapod ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(Hexapod PRIVATE cxx_range_for)
