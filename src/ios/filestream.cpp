
// includes

#include <fstream>
#include <iostream>

#include "filestream.hpp"
#include "var.hpp"

// functions

std::unique_ptr<std::istream> get_stream(const std::string & file_name) {
    return std::make_unique<std::ifstream>(std::ifstream(var::data_file(file_name).c_str()));
}

std::unique_ptr<std::istream> get_stream_binary(const std::string & file_name) {
    return std::make_unique<std::ifstream>(std::ifstream(var::data_file(file_name), std::ios::binary));
}