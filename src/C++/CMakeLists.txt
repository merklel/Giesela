cmake_minimum_required (VERSION 2.9)

project (gisela)

add_definitions(-std=c++11)



# The version number.
set (Tutorial_VERSION_MAJOR 0)
set (Tutorial_VERSION_MINOR 1)


add_executable(gisela main.cpp gisela.cpp mailInterface.cpp nlohmann/json.hpp UnixSocket.h UnixSocket.cpp hardware.cpp restapi.cpp restapi.hpp restapi_controller.cpp restapi_controller.h DTOs.hpp)
target_link_libraries(gisela stdc++ curl pthread pigpio oatpp atomic)
