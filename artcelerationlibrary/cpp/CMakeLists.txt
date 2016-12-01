cmake_minimum_required(VERSION 3.4.1)

add_library(ImageTransforms SHARED
            generic.cpp
            gaussian.cpp
            unsharp.cpp
            sobel_edge.cpp
            neon_edge.cpp
            motion_blur.cpp
            ImageTransforms.cpp)
include_directories(cpp/)
target_link_libraries(ImageTransforms
                      -ljnigraphics
                      android
                      log
                      )