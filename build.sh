#!/bin/bash

# TODO:
# - add paring cmd line arguments
# - handle compilation errors before deploying
# - add syncing sysroot before bulding
# - add better deploy configuration

readonly PROGNAME=$(basename $0)
readonly PROGDIR=$(readlink -m $(dirname $0))
readonly ARGS="$@"

SHOULD_CLEAN=false
#SHOULD_CLEAN=true;
BUILD_DIR_NAME="build"
TOOLCHAIN_CMAKE_PATH="$HOME/code/rpi2/toolchain/toolchain.cmake"
SHOULD_DEPLOY=true
EXECUTABLE_NAME=Hexapod

parse_args() {
	echo "> Parsing cmd line args "
}

create_build_dir() {
	if [ ! -d $BUILD_DIR_NAME ]; then
 		echo "> Creating build dir"
		mkdir $BUILD_DIR_NAME
        fi
}

clean_build_dir() {
	echo "> Cleaning build directory"
	rm -rf $BUILD_DIR_NAME/*
}

run_cmake() {
	echo "> Running cmake"
	cd $BUILD_DIR_NAME
	cmake -D CMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_CMAKE_PATH ..
	# -D CMAKE_BUILD_TYPE=Debug ..
}

run_make() {
	echo "> Running make"
	make
	mv compile_commands.json ../
}

deploy() {
	echo "> Deploying"
	# already in build dir
	scp Main/src/$EXECUTABLE_NAME pi@192.168.0.128:/home/pi/
}

main () {
	parse_args ARGS
	create_build_dir
	if [ !$SHOULD_CLEAN ]; then
 		clean_build_dir
	fi
	run_cmake
	run_make
	if [ !$SHOULD_DEPLOY ]; then
		deploy
	fi
}

main
