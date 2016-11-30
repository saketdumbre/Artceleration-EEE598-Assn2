LOCAL_PATH := $(call my-dir)
ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a x86))
include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_MODULE    :=ImageTransforms
LOCAL_SRC_FILES :=generic.cpp \
                  gaussian.cpp \
                  unsharp.cpp \
                  sobel_edge.cpp\
                  ImageTransforms.cpp.arm.neon
LOCAL_ARM_NEON  := true
LOCAL_NEON_CFLAGS   := -mfpu=neon
include $(BUILD_STATIC_LIBRARY)
$(call import-module,android/cpufeatures)
endif # TARGET_ARCH_ABI == armeabi-v7a || x86



