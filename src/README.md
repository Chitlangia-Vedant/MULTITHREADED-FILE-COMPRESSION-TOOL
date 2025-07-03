# Multithreaded File Compression Tool (C++)

This is a simple multithreaded file compression and decompression tool using `zlib`. It divides the file into chunks and processes them in parallel using C++ threads.

## Features

- Compress/decompress any file
- Multithreaded chunk-wise compression
- Fast and efficient

## Requirements

- C++17
- zlib
- CMake 3.10+

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./MultithreadedCompressor compress <input_file> <output_file> <num_threads>
./MultithreadedCompressor decompress <input_file> <output_file> <num_threads>
```

### Example:

```bash
./MultithreadedCompressor compress data.txt data.z 4
./MultithreadedCompressor decompress data.z data_dec.txt 4
```
## Performance

Chunking and multithreading can speed up compression on large files. Performance gains vary depending on hardware and file size.



