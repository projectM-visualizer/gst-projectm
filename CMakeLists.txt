cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

include(CMakeDependentOption)
include(CheckSymbolExists)

set(OpenGL_GL_PREFERENCE "GLVND")

project(gstprojectm VERSION 0.0.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(projectM4 REQUIRED)
find_package(GStreamer REQUIRED COMPONENTS gstreamer-audio gstreamer-gl gstreamer-pbutils gstreamer-video)
find_package(GLIB2 REQUIRED)

add_library(gstprojectm SHARED
    src/caps.h
    src/caps.c
    src/debug.h
    src/debug.c
    src/config.h
    src/enums.h
    src/plugin.h
    src/plugin.c
    src/projectm.h
    src/projectm.c
    src/gstglbaseaudiovisualizer.h
    src/gstglbaseaudiovisualizer.c
)

target_include_directories(gstprojectm
    PUBLIC
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_BASE_INCLUDE_DIRS}
        ${GSTREAMER_AUDIO_INCLUDE_DIRS}
        ${GSTREAMER_GL_INCLUDE_DIRS}
        ${GLIB2_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

message(STATUS "GSTREAMER_INCLUDE_DIRS: ${GSTREAMER_INCLUDE_DIRS}")
message(STATUS "GSTREAMER_LIBRARIES: ${GSTREAMER_LIBRARIES}")
message(STATUS "GSTREAMER_BASE_INCLUDE_DIRS: ${GSTREAMER_BASE_INCLUDE_DIRS}")
message(STATUS "GSTREAMER_BASE_LIBRARIES: ${GSTREAMER_BASE_LIBRARIES}")
message(STATUS "GSTREAMER_AUDIO_INCLUDE_DIRS: ${GSTREAMER_AUDIO_INCLUDE_DIRS}")
message(STATUS "GSTREAMER_AUDIO_LIBRARIES: ${GSTREAMER_AUDIO_LIBRARIES}")
message(STATUS "GSTREAMER_GL_INCLUDE_DIRS: ${GSTREAMER_GL_INCLUDE_DIRS}")
message(STATUS "GSTREAMER_GL_LIBRARIES: ${GSTREAMER_GL_LIBRARIES}")
message(STATUS "GLIB2_INCLUDE_DIR: ${GLIB2_INCLUDE_DIR}")
message(STATUS "GLIB2_LIBRARIES: ${GLIB2_LIBRARIES}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# GLEW needs to be initialized if libprojectM depends on it.
if(TARGET GLEW::glew OR TARGET GLEW::glew_s)
    target_compile_definitions(gstprojectm
            PRIVATE
            USE_GLEW
    )
endif()

target_link_libraries(gstprojectm
    PRIVATE
        libprojectM::projectM
    PUBLIC
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_BASE_LIBRARIES}
        ${GSTREAMER_AUDIO_LIBRARIES}
        ${GSTREAMER_VIDEO_LIBRARIES}
        ${GSTREAMER_GL_LIBRARIES}
        ${GSTREAMER_PBUTILS_LIBRARIES}
        ${GLIB2_LIBRARIES}
        ${GLIB2_GOBJECT_LIBRARIES}
)