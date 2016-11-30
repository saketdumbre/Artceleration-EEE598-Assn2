LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    :=ImageTransforms
LOCAL_SRC_FILES :=generic.cpp gaussian.cpp unsharp.cpp sobel_edge.cpp ImageTransforms.cpp
LOCAL_C_INCLUDES:=$(LOCAL_PATH)
LOCAL_LDLIBS    := -lm -llog -ljnigraphics
include $(BUILD_SHARED_LIBRARY)



