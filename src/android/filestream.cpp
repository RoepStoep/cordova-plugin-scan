
// includes

#include <android/asset_manager.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <streambuf>

#include "filestream.hpp"

// types

// added shared_ptr from https://stackoverflow.com/questions/13059091/creating-an-input-stream-from-constant-memory
struct membuf: std::streambuf {
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};
struct imemstream: virtual membuf, std::istream {
    std::shared_ptr<char> basePtr;
    imemstream(std::shared_ptr<char> base, size_t size)
        : membuf(base.get(), size)
        , basePtr(base)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};

// variables

AAssetManager* asset_manager;

// functions

void set_asset_manager(AAssetManager* manager) {
  asset_manager = manager;
}

std::unique_ptr<std::istream> get_stream(const std::string & file_name) {
    AAsset* asset = AAssetManager_open(asset_manager, file_name.c_str(), 0);
    if (!asset) return NULL;

    long size = AAsset_getLength(asset);
    std::shared_ptr<char> buffer((char*) malloc(sizeof(char)*size), free);
    AAsset_read(asset, buffer.get(), size);
    AAsset_close(asset);

    return std::unique_ptr<std::istream>(new imemstream(buffer, size));
}

std::unique_ptr<std::istream> get_stream_binary(const std::string & file_name) {
    return get_stream(file_name);
}