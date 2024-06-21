cmake_minimum_required(VERSION 3.28)
project(OsExperimentHomework C)

set(CMAKE_C_STANDARD 11)

add_executable(exp2
        main.c)

target_link_libraries(exp2 m)