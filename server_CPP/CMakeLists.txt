cmake_minimum_required(VERSION 3.16)
project(server_CPP)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(server_CPP
        constants.h
        handlRequest.h
        main.cpp
        funcitonalUnity.h
        messages.h
        packetStructs.h)
