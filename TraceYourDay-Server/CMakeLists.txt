cmake_minimum_required(VERSION 3.17)
project(TraceYourDay_Server C)

set(CMAKE_C_STANDARD 99)

link_libraries(mysqlclient)

add_executable(TraceYourDay_Server main.c)