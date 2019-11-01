
#ifndef FILESTREAM_HPP
#define FILESTREAM_HPP

// includes

#include <iostream>

// functions

std::unique_ptr<std::istream> get_stream(const std::string & filename);
std::unique_ptr<std::istream> get_stream_binary(const std::string & filename);

#endif // !defined FILESTREAM_HPP

