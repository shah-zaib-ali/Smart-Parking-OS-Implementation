cmake_minimum_required(VERSION 3.16)
project(SmartParkingOS VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Charts REQUIRED)

add_executable(SmartParkingOS
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    vehiclethread.cpp vehiclethread.h
    scheduler.cpp scheduler.h
    globals.cpp globals.h
)

target_link_libraries(SmartParkingOS PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts
)
