#include "git-maker.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

/*
 * IF there exists more than 1 .gitwork, we will simply delete all
 * after the first one
 *
 *
 * This is for git init SO we we just use the name of the parent directory
 * */
Internals::Internals(const fs::path &path) {

  // Checks if we already are inside of a gitwork directory
  // If so, we do fill our class information using the provided
  // paths and filling in missing areas with new directories

  bool exists = false;
  for (const auto &entry : fs::directory_iterator{path.parent_path()}) {
    if (entry.path().filename() == ".gitwork") {
      _dir = entry.path();
      exists = true;
    }
  }

  // Should add more error checks
  if (!exists) {
    _dir = path / ".gitwork";
    fs::create_directory(_dir);
  }

  // Everyime we add a new file within our .gitwork, we have to make sure
  // it doesn't already exist. If it does, then we do nothing to it.
  // If, for some reason, there are multiple of the same files inside the
  // gitwork, we won't touch them and it would simply make the
  // program less reliable.
  // The onus is on the user to not have messed up files assuming our
  // product is fine.

  // OBJECTS
  exists = false;
  for (const auto &entry : fs::directory_iterator{_dir}) {
    if (entry.path().filename() == "objects") {
      _objects = entry.path();
      exists = true;
    }
  }

  // Again, potential error checks, etc. Will keep adding these
  if (!exists) {
    _objects = _dir / "objects";
    fs::create_directory(_objects);
  }

  // REFS
  // For this case, if we only have an existing refs directory but no
  // proper files inside, we will NOT replace them. Potential improvement
  // to be made later on but sub-files are important here
  exists = false;
  for (const auto &entry : fs::directory_iterator{_dir}) {
    if (entry.path().filename() == "refs") {
      _refs = entry.path();
      exists = true;
    }
  }

  // Hopefully, the subdirectory(ies) are made properly
  if (!exists) {
    _refs = _dir / "refs";
    fs::create_directory(_refs);
    fs::create_directory(_refs / "heads");
  }

  // HEAD
  // TEXT FILE
  exists = false;
  for (const auto &entry : fs::directory_iterator{_dir}) {
    if (!entry.is_directory() && entry.path().filename() == "HEAD") {
      _headFile = entry.path();
      exists = true;
    }
  }

  if (!exists) {
    _headFile = _dir / "HEAD.txt";
    std::ofstream output_file(_headFile);

    // No need to write in it for now
    if (output_file.is_open())
      output_file.close();
  }

  // INDEX
  // TEXT FILE
  exists = false;
  for (const auto &entry : fs::directory_iterator{_dir}) {
    // need to be careful with this first if condition to know we're
    // checking if this PATH is a directory or if I'm accidentally
    // reviewing an iterator somehow
    if (!entry.is_directory() && entry.path().filename() == "index") {
      _indexFile = entry.path();
      exists = true;
    }
  }

  if (!exists) {
    _indexFile = _dir / "index.txt";
    std::ofstream out_file(_indexFile);

    // No need to write in it for now
    if (out_file.is_open())
      out_file.close();
  }
}

// WE will also create a constructor without name where we check existance
// in the main function

/* Will be throwing this into main so that
 * our code here can just worry about the name and internals being set up
 *
 *
  if (!fs::is_directory(path) || !fs::exists(path)) {
    throw std::invalid_argument("Invalid Path.");
  }
 *
 *
 */
