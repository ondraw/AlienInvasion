# The ARMv7 is significanly faster due to the use of the hardware FPU
# 너무 많이 컴파일 해서 배포전까지만 코멘트
APP_ABI := armeabi-v7a arm64-v8a
#APP_ABI := armeabi-v7a
APP_PLATFORM := android-14
#APP_STL := stlport_static
APP_STL := c++_shared
APP_OPTIM := debug
APP_CFLAGS += -Wno-error=format-security