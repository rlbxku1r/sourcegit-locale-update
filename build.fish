#!/usr/bin/env fish

set source_files main.cpp tinyxml2/tinyxml2.cpp
set source_dir (status dirname)

set output_bin sourcegit-locale-update
set output_dir $source_dir/bin

if ! set -q CXX
    if command -q clang++
        set CXX clang++
    else
        set CXX g++
    end
end

mkdir -p $output_dir
$CXX -o $output_dir/$output_bin $source_dir/$source_files
