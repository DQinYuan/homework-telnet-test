cmake_minimum_required(VERSION 3.14)
project(telnet_test C)

set(CMAKE_C_STANDARD 99)

add_executable(telnet-server server.c common.h common.c)
add_executable(telnet-client client.c common.h common.c)
add_executable(little-test test.c)