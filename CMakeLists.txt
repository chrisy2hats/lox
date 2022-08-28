cmake_minimum_required(VERSION 3.16.3)
project(lox)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Werror -Wall -Wextra -Wreorder -Weffc++ -Wpedantic -Wimplicit-fallthrough -Wno-error=vla")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    #Force clang to build std lib function in debug. Otherwise GDB can't see stdlib objects properly
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif ()
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native")
add_executable(lox main.cpp)
