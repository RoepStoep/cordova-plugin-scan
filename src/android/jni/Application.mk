APP_PLATFORM := android-19
APP_STL := c++_shared
APP_ABI := armeabi-v7a arm64-v8a x86 x86_64
APP_OPTIM := release
APP_CPPFLAGS += -O3 -std=c++14 -fexceptions -DHAVE_PTHREAD=1
APP_USE_CPP0X := true
