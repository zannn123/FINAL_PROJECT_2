cmake_minimum_required(VERSION 3.20)

project(finalProject2 CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(finalProject2
        main.cpp
        ui_core.cpp
        file_handler.cpp
        file_handler.h
        globals.h
        ui_core.h
        admin.cpp
        admin.h
        user.cpp
        user.h
        ui_admin.cpp
        ui_admin.h
        ui_user_global.cpp
        ui_user_global.h
        ui_user_message.cpp
        ui_user_message.h
        ui_user_connections.cpp
        ui_user_connections.h
)