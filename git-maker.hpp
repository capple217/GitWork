#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class Internals {
public:
  Internals(const fs::path &path);

private:
  fs::path _dir;
  fs::path _objects;
  fs::path _refs;
  fs::path _headFile;
  fs::path _indexFile;
};
