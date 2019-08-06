LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

scan_src_files := \
    src/bb_base.cpp                             \
    src/bb_comp.cpp                               \
    src/bb_index.cpp                              \
    src/bit.cpp                               \
    src/book.cpp                              \
    src/common.cpp                              \
    src/eval.cpp                                  \
    src/fen.cpp                               \
    src/game.cpp                              \
    src/gen.cpp                                 \
    src/hash.cpp                              \
    src/hub.cpp                                  \
    src/libmy.cpp                                \
    src/list.cpp                                \
    src/main.cpp                               \
    src/move.cpp                                    \
    src/pos.cpp                                   \
    src/score.cpp                              \
    src/search.cpp                                  \
    src/sort.cpp                                \
    src/thread.cpp                                \
    src/tt.cpp                               \
    src/util.cpp                                    \
    src/var.cpp                                   \

LOCAL_MODULE := scan
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := $(scan_src_files)

include $(BUILD_STATIC_LIBRARY)
