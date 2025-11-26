#include "fileSystem.hpp"
#include <filesystem>

FileBrowser::FileBrowser(fs::path root) : _root{root}, _current{root} {}

void FileBrowser::upTree() { _current = _current.parent_path(); }

void FileBrowser::printPath() {
  if (_current.empty()) {
    std::cout << "[!] Path is empty...\n";
    return;
  }

  if (!fs::exists(_current)) {
    std::cout << "[!] Path doesn't exist...\n";
    return;
  }

  if (!fs::is_directory(_current)) {
    std::cout << "[!] Not a directory, please go Up (U) or Reset (R) before "
                 "going down further.\n\n";
    return;
  }

  _children.clear();
  auto idx = 0;

  std::cout << "Current: " << _current << "\n\n";
  for (const auto &entry : fs::directory_iterator{_current}) {
    const auto &path = entry.path();

    if (fs::is_directory(path)) {
      _children.push_back(path);
      std::cout << idx++ << ": [DIR] " << path.filename() << "\n";
    }

    else if (fs::is_regular_file(path)) {
      auto ext = path.extension().string();
      if (ext == ".cpp" || ext == ".hpp" || ext == ".c" || ext == "h") {
        _children.push_back(path);
        std::cout << idx++ << "      " << path.filename() << "\n";
      }
    }
  }

  if (_children.empty()) {
    std::cout << "(No valid source files or folders found in this directory)\n";
  }
}

void FileBrowser::reset() { _current = _root; }

void FileBrowser::selectChild(size_t idx) {

  if (!fs::exists(_current) || !fs::is_directory(_current)) {
    std::cout << "Cannot list children-current is not a directory.\n";
    return;
  }

  if (idx < 0 || idx >= _children.size()) {
    std::cout << "This value doesn't exist within range\n";
    return;
  }

  _current = _children[idx];
}

// Currently unused function, may be deleted
void FileBrowser::printSelected() {
  std::cout << "---- SELECTED ----\n";
  for (size_t i = 0; i < _selected.size(); ++i) {
    std::cout << i + 1 << "   " << _selected[i].filename() << std::endl;
  }
}

size_t FileBrowser::numPaths() {
  size_t size = 0;
  for (const auto &entry : fs::directory_iterator{_current}) {
    size++;
  }

  return size;
}

void FileBrowser::deleteSelected(size_t idx) {
  if (_selected.empty() || idx < 1 || idx > _selected.size())
    return;
  _selected.erase(_selected.begin() + idx - 2);
  // We add 1 since our list is 1-indexed not 0
}
