#!/usr/bin/env fish

set SOURCE_FILES main.cpp tinyxml2/tinyxml2.cpp
set SOURCE_DIR (status dirname)

set OUTPUT_BIN sourcegit-locale-update
set OUTPUT_DIR $SOURCE_DIR/bin

if ! set -q CXX
    if command -q clang++
        set CXX clang++
    else
        set CXX g++
    end
end

mkdir -p $OUTPUT_DIR
$CXX -o $OUTPUT_DIR/$OUTPUT_BIN $SOURCE_DIR/$SOURCE_FILES
