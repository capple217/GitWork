#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// FOR NOW, we won't require name or email for the messages
// Rather, we'll just focus on timestamps, message, tree hash
// for the commit objects
class Internals {
public:
  Internals(fs::path &path); // This is a git init solution, we can still
                             // work on a git clone type of constructor
  void objectify(std::string message); // Here, we store all our files into
                                       // objects after hashing

  // Recursive Post order DFS approach to go from most nested to least for
  // directories
  std::string fileOrdering(const fs::path &path);

  // Doesn't simply work, have some allocation issues to settle eventually
  ~Internals();

  // Going to re-make staging area with the index as recording all the names of
  // the files that are in objects. Worst case, there is something already
  // staged and we have to manually delete all of that before rewriting the new
  // stage
  void stage();
  bool isStageFull() { return _stageFullness; }

private:
  fs::path _dir;
  fs::path _git; // path of .gitmake directory within existing file

  fs::path _objects; //
  fs::path _refs; // Master branch containing {heads: tips/ leafs of branches,
                  // tags: important snapshots to remember}
  fs::path _headFile;  // Pointing to the current branch / cmmit
  fs::path _indexFile; // Staging area binary

  bool _stageFullness = false;
};
