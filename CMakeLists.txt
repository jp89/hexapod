cmake_minimum_required (VERSION 2.8)
project (Hexapod)
subdirs(Main ServoController)
# for clang-based code completion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

