cmake_minimum_required(VERSION 3.20)
project(Overlay_BMP_image_on_YUV420_video LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
if (UNIX) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse4.1")
endif()

# source
list(APPEND SOURCES "Overlay_BMP_image_on_YUV420_video.cpp")
list(APPEND SOURCES "stdafx.h")
list(APPEND SOURCES "Frame.h")
list(APPEND SOURCES "Frame.cpp")
list(APPEND SOURCES "BMPImage.h")
list(APPEND SOURCES "BMPImage.cpp")

add_executable(Overlay_BMP_image_on_YUV420_video ${SOURCES})

set_target_properties(Overlay_BMP_image_on_YUV420_video PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin)
