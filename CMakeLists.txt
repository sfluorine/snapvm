cmake_minimum_required(VERSION 3.7...3.27)

project(snapvm LANGUAGES C)

add_executable(snap src/main.c src/vm.c src/object.c)
