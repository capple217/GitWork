#pragma once

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <openssl/sha.h>
#include <sstream>
#include <vector>
#include <zlib.h>

namespace fs = std::filesystem;

std::string sha1_hex(const std::string &data);

std::vector<unsigned char>
compress_zlib(const std::vector<unsigned char> &input);

std::string write_blob_and_get_sha(const fs::path &repo_dir,
                                   const fs::path &filepath);
