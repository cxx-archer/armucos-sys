LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := memory
LOCAL_SRC_FILES := memory.c

include $(BUILD_STATIC_LIBRARY)

