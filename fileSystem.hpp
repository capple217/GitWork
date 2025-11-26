#pragma once
// fs (filesystem)

#include <filesystem>
#include <iostream>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

class FileBrowser {
public:
  FileBrowser(fs::path root);
  void upTree();
  void printPath();
  void selectChild(size_t idx);
  void reset();
  fs::path current() const { return _current; }

  void printSelected();
  size_t numPaths();
  void deleteSelected(size_t idx);
  size_t numSel() const { return _selected.size(); }

  // We keep track of current here to know whether we're
  // still in the git file
  bool containsGit() const;

private:
  std::vector<fs::path> _selected; // List of all selected paths
  fs::path _root;                  // Starting path
  fs::path _current;               // Current path
  std::vector<fs::path> _children; // List of current files/directories of path
};
