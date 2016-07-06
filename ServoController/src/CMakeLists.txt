include_directories(${Hexapod_SOURCE_DIR}/ServoController/inc)
add_library(ServoController adafruit_servohat_driver.cpp servo_controller.cpp)
