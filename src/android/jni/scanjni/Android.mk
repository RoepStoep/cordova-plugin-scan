LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := scanjni
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../scan/src/
LOCAL_SHARED_LIBRARIES := scan
LOCAL_SRC_FILES := scanjni.cpp
LOCAL_LDLIBS := -llog -landroid

include $(BUILD_SHARED_LIBRARY)
