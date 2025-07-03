#pragma once
#include <string>

class Compressor {
public:
    static void compressFile(const std::string& input, const std::string& output, int threads = 4);
    static void decompressFile(const std::string& input, const std::string& output, int threads = 4);
};
