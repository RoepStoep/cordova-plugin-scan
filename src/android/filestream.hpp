
#ifndef FILESTREAM_HPP
#define FILESTREAM_HPP

// includes

#include <iostream>

#include <android/asset_manager.h>

// functions

void set_asset_manager(AAssetManager* manager);

std::unique_ptr<std::istream> get_stream(const std::string & filename);
std::unique_ptr<std::istream> get_stream_binary(const std::string & filename);

#endif // !defined FILESTREAM_HPP

