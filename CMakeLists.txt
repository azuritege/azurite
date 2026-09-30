cmake_minimum_required(VERSION 3.21)
project(Azurite CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendor/Lua)
add_subdirectory(vendor/raylib)
add_subdirectory(vendor/sol2)

add_executable(azurite src/main.cpp)
target_include_directories(azurite PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(azurite PRIVATE raylib Lua::Library sol2)
set_target_properties(azurite PROPERTIES OUTPUT_NAME "_azurite")