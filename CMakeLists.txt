cmake_minimum_required(VERSION 3.10)
project(Raycaster)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDL2_INCLUDE_DIRS "C:/SDL2/include")

set(SDL2_LIBRARIES "C:/SDL2/lib/x64/SDL2.lib" "C:/SDL2/lib/x64/SDL2main.lib")

#find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Raycaster RayCaster.cpp)

target_link_libraries(Raycaster ${SDL2_LIBRARIES})

if(MSVC)
    target_compile_options(Raycaster PRIVATE /W4)
else()
    target_compile_options(Raycaster PRIVATE -Wall -Wextra)
endif()