include_directories(${Hexapod_SOURCE_DIR}/Main/inc)
include_directories(${Hexapod_SOURCE_DIR}/ServoController/inc)
link_directories(${Hexapod_SOURCE_DIR}/ServoController/src)
add_executable(Main main.cc)
SET_TARGET_PROPERTIES(Main PROPERTIES OUTPUT_NAME Hexapod)
target_link_libraries(Main ServoController pthread)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

