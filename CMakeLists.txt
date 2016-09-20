cmake_minimum_required(VERSION 3.5)

project(Hexapod)

file(GLOB SERVO_CONTROLLER_SRC servo_controller/src/*.cpp)
file(GLOB ADAFRUIT_SERVO_DRIVER_SRC adafruit_servo_driver/src/*.cpp)

include_directories(servo_controller/inc
    adafruit_servo_driver/inc)


find_package (Threads)

add_executable(Hexapod main.cc ${SERVO_CONTROLLER_SRC}
    ${ADAFRUIT_SERVO_DRIVER_SRC})
target_link_libraries (Hexapod ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(Hexapod PRIVATE cxx_range_for)
