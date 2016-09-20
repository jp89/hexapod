SET(CMAKE_SYSTEM_NAME Linux)

SET(ROOTFS $ENV{HOME}/Toolchains/rootfs)
SET(RPI2_TOOLCHAIN $ENV{HOME}/Toolchains/rpi2)

SET(CMAKE_C_COMPILER ${RPI2_TOOLCHAIN}/bin/arm-unknown-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER ${RPI2_TOOLCHAIN}/bin/arm-unknown-linux-gnueabihf-g++)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
