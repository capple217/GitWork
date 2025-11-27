#include "git-maker.hpp"
#include "hash.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

/*
 * IF there exists more than 1 .gitwork, we will simply delete all
 * after the first one
 *
 *
 * This is for git init SO we we just use the name of the parent directory
 * */
Internals::Internals(fs::path &path) : _dir{path} {

  // Checks if we already are inside of a gitwork directory
  // If so, we do fill our class information using the provided
  // paths and filling in missing areas with new directories

  // Should add more error checks
  fs::create_directory(path / ".gitwork");
  _git = path / ".gitwork";

  // Everyime we add a new file within our .gitwork, we have to make sure
  // it doesn't already exist. If it does, then we do nothing to it.
  // If, for some reason, there are multiple of the same files inside the
  // gitwork, we won't touch them and it would simply make the
  // program less reliable.
  // The onus is on the user to not have messed up files assuming our
  // product is fine.

  // OBJECTS
  // Again, potential error checks, etc. Will keep adding these
  fs::create_directory(_git / "objects");
  _objects = _git / "objects";

  // REFS
  // For this case, if we only have an existing refs directory but no
  // proper files inside, we will NOT replace them. Potential improvement
  // to be made later on but sub-files are important here

  // May add more sub-directories here
  _refs = _git / "refs";
  _refsHeads = _refs / "heads";
  fs::create_directory(_refs);
  fs::create_directory(_refs / "heads");

  // HEAD
  // TEXT FILE

  _headFile = _git / "HEAD";
  std::ofstream output_file(_headFile);

  // No need to write in it for now
  if (output_file.is_open())
    output_file.close();

  // INDEX
  // TEXT FILE

  _indexFile = _git / "index";
  std::ofstream out_file(_indexFile);

  // No need to write in it for now
  if (out_file.is_open())
    out_file.close();
}

// Trying without error checks to first test functionality
// need to add safety nets soon after
//
// This methodology is one I interpreted from The Git Parable.
// Future versions of this program may change this function
//
// Can create a vector later that contains all the gitwork ignores and we can
// just run that to be ignored
std::string Internals::fileOrdering(fs::path &path, int idx) {
  // Create temp file
  // And ensure it's temp name is unique
  // We can develop more comprehensive and unique names later
  std::string num = std::to_string(idx);
  fs::path file_path = path / num;
  std::ofstream temp_file(file_path);
  if (temp_file.is_open()) {
    for (const auto &entry : fs::directory_iterator(path)) {
      // Don't want to save our gitwork files
      if (entry.path().filename() == ".gitwork")
        continue;
      if (entry.is_directory()) {
        auto p = entry.path();
        auto sha = fileOrdering(p, ++idx);
        temp_file << "tree " << sha << " " << entry.path().filename()
                  << std::endl;
      } else if (entry.is_regular_file()) {
        // need to read all the contents of the files
        // for now, we only accept "regular files"
        std::string source = entry.path().filename();
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

          std::string new_file = hash;

          fs::path new_dest(_objects / new_file);
          fs::copy_file(entry.path(), new_dest,
                        fs::copy_options::skip_existing);

          file.close();
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

  std::string new_file = hash;

  fs::path new_dest(_objects / new_file);
  fs::copy_file(file_path, new_dest, fs::copy_options::skip_existing);

  file.close();
  fs::remove(file_path);
  // No longer need the temp file we made

  return hash;
}

// First have all these files stored into objects (whichever ones are updated)
// THen take the commit message and create the unique commit object and also
// store in objects
//
// objectify is simply commit
void Internals::objectify(std::string message) {

  std::ifstream index(_indexFile);
  std::string hash((std::istreambuf_iterator<char>(index)),
                   std::istreambuf_iterator<char>());

  // time
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);

  // final file
  std::string file_path = _dir / "message";
  std::ofstream temp_file(file_path);
  if (!temp_file.is_open()) {
    std::cout << "Error in opening commit message file.\n";
  }

  if (!_rootCommit) {
    std::ifstream tree(_branchCurr);
    if (!tree.is_open()) {
      std::cout << "Error opening file for commit parents.\n";
    }
    std::string treeHash(std::istreambuf_iterator<char>(tree), {});
    temp_file << "Parent: " << treeHash << std::endl;
  }
  temp_file << "Time: " << std::ctime(&now_c) << std::endl;
  temp_file << "Hash: " << hash << std::endl;
  temp_file << "Message: " << message << std::endl;

  // We dont have others like tree hash set up but will also update that soon
  // enough

  temp_file.close();

  // Get hash of this
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cout << "Failure to open file to get hash for commit message.\n";
  }
  std::string file_contents(std::istreambuf_iterator<char>(file), {});
  auto new_name = sha1_hex(file_contents);

  fs::path new_dest(_objects / new_name);
  fs::copy_file(file_path, new_dest, fs::copy_options::skip_existing);

  file.close();
  fs::remove(file_path);
  newTip(hash);
  _rootCommit = false;
}

// For now, this destructor is simple
// When we start incorporating network stuff,
// it may get more interseting
Internals::~Internals() { fs::remove_all(_git); }

// Have a hash that we can follow the path of
// Any existing stages are currently forcefully overwritten
void Internals::stage() {

  auto hash = fileOrdering(_dir, 0);
  std::ofstream file(_indexFile, std::ofstream::out | std::ofstream::trunc);
  if (!file.is_open()) {
    std::cerr << "Unable to open file to stage.\n";
  }

  file << hash << std::endl;
  _stageFullness = true;
  file.close();
}

void Internals::createBranch(std::string name = "main") {

  // if name already exists, then we do nothing
  for (const auto &entry : fs::directory_iterator{_refsHeads}) {
    if (entry.path().filename() == name) {
      return;
    }
  }

  fs::path path = _refsHeads / name;
  std::ofstream file(path);
  if (file.is_open())
    file.close();
  _branchNames.push_back(path);
  chooseBranch(path);
}

// Error checks will be done outside
// like in main
void Internals::chooseBranch(fs::path path) {

  std::ofstream file(_headFile, std::ofstream::out | std::ofstream::trunc);
  if (!file.is_open()) {
    std::cout << "Error in choosing branch.\n";
  }
  file << path.string() << std::endl;
  _branchCurr = path;
  file.close();
}

// Overload to use the branchNames vector
void Internals::chooseBranch(int idx) {
  if (idx < 0 || idx >= _branchNames.size()) {
    return;
  }

  chooseBranch(_branchNames[idx]);
}

// Whenever we commit something, it is always gonna be at the tip of a branch
void Internals::newTip(std::string hash) {
  std::ofstream file(_branchCurr, std::ofstream::out | std::ofstream::trunc);
  if (!file.is_open()) {
    std::cout << "Unable to open file to create new branch tip.\n";
  }

  file << hash << std::endl;
  file.close();
}

void Internals::printBranches() {
  int idx = 0;
  for (const auto &branch : _branchNames) {
    std::cout << idx++ << ": " << branch.filename() << std::endl;
  }
}
