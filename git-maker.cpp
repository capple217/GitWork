#include "git-maker.hpp"
#include "hash.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

/*
 * IF there exists more than 1 .gitwork, we will simply delete all
 * after the first one
 *
 *
 * This is for git init SO we we just use the name of the parent directory
 * */
Internals::Internals(const fs::path &path) : _dir{path} {

  // Checks if we already are inside of a gitwork directory
  // If so, we do fill our class information using the provided
  // paths and filling in missing areas with new directories

  bool exists = false;
  for (const auto &entry : fs::directory_iterator{path.parent_path()}) {
    if (entry.path().filename() == ".gitwork") {
      _git = entry.path();
      exists = true;
    }
  }

  // Should add more error checks
  if (!exists) {
    _git = path / ".gitwork";
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

  // May add more sub-directories here
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

// Trying without error checks to first test functionality
// need to add safety nets soon after
//
// This methodology is one I interpreted from The Git Parable.
// Future versions of this program may change this function
std::string Internals::fileOrdering(const fs::path &path) {
  // Create temp file
  std::string file_path = path.string() + "data.text";
  std::ofstream temp_file(file_path);
  if (temp_file.is_open()) {
    for (const auto &entry : fs::directory_iterator(path)) {
      if (entry.is_directory()) {
        auto sha = fileOrdering(entry.path());
        temp_file << "tree " << sha << " " << entry.path().filename()
                  << std::endl;
      } else {
        // need to make sure this is path for the file
        std::string source = entry.path().filename().string();
        std::ifstream file(source);
        if (file.is_open()) { // Kinda need this error check
          std::string file_contents(std::istreambuf_iterator<char>(file),
                                    {}); // Absorb all of the file and hash it
          auto hash = sha1_hex(file_contents);
          temp_file << "blob " << hash << " " << entry.path().filename()
                    << std::endl;

          // rename the source file into the hash and try to move it to the
          // objects folder if there already exists this file, we can skip this
          // step

          bool exists = false;
          std::string new_file = hash + ".txt";
          for (const auto &entry : fs::directory_iterator(_objects)) {
            if (entry.path().filename() == new_file) {
              exists = true;
              break;
            }
          }

          if (!exists) {
            fs::path new_dest(_objects / new_file);
            fs::copy_file(entry.path(), new_dest,
                          fs::copy_options::skip_existing);
          }
        }
      }
    }

    temp_file.close();
  }

  // Same as above, try to paste this temp file into objects with the hash as
  // it's name only if it doesn't exist already

  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cout << "error at the end of analyzing and deleting temp file.\n";
  }
  std::string file_contents(std::istreambuf_iterator<char>(file), {});
  auto hash = sha1_hex(file_contents);

  bool exists = false;
  std::string new_file = hash + ".txt";
  for (const auto &entry : fs::directory_iterator(_objects)) {
    if (entry.path().filename() == new_file) {
      exists = true;
      break;
    }
  }

  if (!exists) {
    fs::path new_dest(_objects / new_file);
    fs::copy_file(file_path, new_dest, fs::copy_options::skip_existing);
  }

  file.close();
  fs::remove(file_path);
  // No longer need the temp file we made

  return hash;
}

void Internals::objectify(std::string message) {
  //
}
