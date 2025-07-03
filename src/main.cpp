#include "Compressor.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage:\n"
                  << "  compress   <input> <output> <threads>\n"
                  << "  decompress <input> <output> <threads>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];
    int threads = std::stoi(argv[4]);

    try {
        if (mode == "compress") {
            Compressor::compressFile(input, output, threads);
        } else if (mode == "decompress") {
            Compressor::decompressFile(input, output, threads);
        } else {
            std::cerr << "Invalid mode.\n";
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    std::cout << "Operation completed successfully.\n";
    return 0;
}
