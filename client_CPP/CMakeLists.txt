cmake_minimum_required(VERSION 3.16)
project(client_CPP)

set(CMAKE_CXX_STANDARD 14)

add_executable(client_CPP
        messages.h
        constants.h
        main.cpp
        makeRequest.h packetStructs.h handleResponse.h multiPlayer.h)

TARGET_LINK_LIBRARIES( client_CPP pthread)