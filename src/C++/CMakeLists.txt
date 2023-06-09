cmake_minimum_required (VERSION 2.6)

project (gisela)

add_definitions(-std=c++11)



# The version number.
set (Tutorial_VERSION_MAJOR 0)
set (Tutorial_VERSION_MINOR 1)


add_executable(gisela main.cpp gisela.cpp mailInterface.cpp UnixSocket.h UnixSocket.cpp hardware.cpp)
target_link_libraries(gisela stdc++ curl pthread pigpio)
