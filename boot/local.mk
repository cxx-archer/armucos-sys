LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := boot
LOCAL_SRC_FILES := boot.c main.c
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := startup
LOCAL_SRC_FILES := start.S
include $(BUILD_SINGLE_OBJECTS)

