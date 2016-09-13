cmake_minimum_required(VERSION 2.6)
project(XX_THREAD_DEMO)  

message(STATUS "Source DIR:" ${XX_THREAD_DEMO_SOURCE_DIR})

# Base compile define.	
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -lstdc++ -lpthread -g")
if(WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE
      	"${CMAKE_CXX_FLAGS_RELEASE}  /MT /Zi /Od /Ob2 /RTC1"
      	CACHE STRING "Release builds CMAKE CXX flags " FORCE )
endif(WIN32)

# Binary output path and binary name.
set(EXECUTABLE_OUTPUT_PATH ${FX_FUZZ_TIFF_SOURCE_DIR}/bin)


# exe
add_executable(demo demo.cpp xx_threadpool.cpp)

