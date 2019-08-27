cmake_minimum_required(VERSION 3.13)
project(msdalloc)

set(CMAKE_CXX_STANDARD 17)

add_executable(msdalloc msdAlloc.cpp msdAlloc.h allocHashtable.cpp allocHashtable.h)

add_executable(testing msdAlloc.cpp msdAlloc.h msdallocTesting.cpp allocHashtable.cpp catch.hpp)