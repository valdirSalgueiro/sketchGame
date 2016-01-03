LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_STATIC_LIBRARIES := libavformat libavcodec libavutil libpostproc libswscale
LOCAL_MODULE := libffmpeg
LOCAL_SRC_FILES := ../VideoGLSurfaceView.c
LOCAL_LDLIBS := -lz -llog -lGLESv1_CM
include $(BUILD_SHARED_LIBRARY)
include $(call all-makefiles-under,$(LOCAL_PATH))