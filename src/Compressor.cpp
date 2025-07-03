#include "Compressor.hpp"
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <zlib.h>
#include <iostream>

constexpr size_t CHUNK_SIZE = 1024 * 1024; // 1MB per chunk
std::mutex io_mutex;

void compressChunk(const std::vector<char>& inData, std::vector<char>& outData) {
    uLong srcLen = inData.size();
    uLong destLen = compressBound(srcLen);
    outData.resize(destLen);

    if (compress(reinterpret_cast<Bytef*>(outData.data()), &destLen,
                 reinterpret_cast<const Bytef*>(inData.data()), srcLen) != Z_OK) {
        throw std::runtime_error("Compression failed");
    }

    outData.resize(destLen);
}

void decompressChunk(const std::vector<char>& inData, std::vector<char>& outData, size_t originalSize) {
    outData.resize(originalSize);
    uLongf destLen = originalSize;

    if (uncompress(reinterpret_cast<Bytef*>(outData.data()), &destLen,
                   reinterpret_cast<const Bytef*>(inData.data()), inData.size()) != Z_OK) {
        throw std::runtime_error("Decompression failed");
    }

    outData.resize(destLen);
}

void Compressor::compressFile(const std::string& input, const std::string& output, int threads) {
    std::ifstream inFile(input, std::ios::binary);
    std::ofstream outFile(output, std::ios::binary);
    std::vector<std::thread> workers;

    if (!inFile || !outFile) throw std::runtime_error("File error");

    std::vector<std::vector<char>> chunks;
    std::vector<std::vector<char>> compressedChunks(threads);

    while (!inFile.eof()) {
        chunks.clear();
        for (int i = 0; i < threads && !inFile.eof(); ++i) {
            std::vector<char> buffer(CHUNK_SIZE);
            inFile.read(buffer.data(), CHUNK_SIZE);
            buffer.resize(inFile.gcount());
            if (!buffer.empty()) chunks.push_back(std::move(buffer));
        }

        for (int i = 0; i < chunks.size(); ++i) {
            workers.emplace_back([&, i]() {
                compressChunk(chunks[i], compressedChunks[i]);
            });
        }

        for (auto& t : workers) t.join();
        workers.clear();

        for (const auto& comp : compressedChunks) {
            uint32_t size = comp.size();
            outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
            outFile.write(comp.data(), size);
        }
    }
}

void Compressor::decompressFile(const std::string& input, const std::string& output, int threads) {
    std::ifstream inFile(input, std::ios::binary);
    std::ofstream outFile(output, std::ios::binary);
    std::vector<std::thread> workers;

    if (!inFile || !outFile) throw std::runtime_error("File error");

    while (!inFile.eof()) {
        std::vector<std::vector<char>> compChunks;
        std::vector<std::vector<char>> decompChunks(threads);
        std::vector<size_t> originalSizes;

        for (int i = 0; i < threads && !inFile.eof(); ++i) {
            uint32_t size;
            if (!inFile.read(reinterpret_cast<char*>(&size), sizeof(size))) break;

            std::vector<char> buffer(size);
            inFile.read(buffer.data(), size);
            compChunks.push_back(std::move(buffer));
            originalSizes.push_back(CHUNK_SIZE);
        }

        for (int i = 0; i < compChunks.size(); ++i) {
            workers.emplace_back([&, i]() {
                decompressChunk(compChunks[i], decompChunks[i], originalSizes[i]);
            });
        }

        for (auto& t : workers) t.join();
        workers.clear();

        for (const auto& dec : decompChunks) {
            outFile.write(dec.data(), dec.size());
        }
    }
}
