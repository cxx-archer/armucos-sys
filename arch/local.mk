LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := arch
LOCAL_SRC_FILES := uart.c board.c irq.c timer.c

include $(BUILD_STATIC_LIBRARY)

