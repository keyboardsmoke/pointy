cmake_minimum_required(VERSION 3.5)
project(pointy)

# Setup extra module directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

# Modules
find_package(V8 REQUIRED)
find_package(Fcgi REQUIRED)

# Add module include directories
include_directories(${V8_INCLUDE_DIR})
include_directories(${FCGI_INCLUDE_DIR})

#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Execute.cpp Execute.h pointy.h pointy.cpp Interface/GlobalInterface.cpp Interface/GlobalInterface.h FCGI.cpp FCGI.h Interfaces.h)
add_executable(pointy ${SOURCE_FILES})

# Link module libraries
target_link_libraries(pointy ${V8_LIBRARY})
target_link_libraries(pointy ${FCGI_LIBRARY})
target_link_libraries(pointy "/usr/lib/libfcgi++.a")

#if (FCGIPP_FOUND)
#    target_link_libraries(pointy ${FCGIPP_LIBRARY})
#endif(FCGIPP_FOUND)