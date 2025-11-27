#include "fileSystem.hpp"
#include "git-maker.hpp"
#include "hash.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

void gitWorker(fs::path &path);

// Need to update main to test blob and SHA
void clearScreen() { std::cout << "\033[2J\033[1;1H" << std::flush; }

// Cleaning up strings when accepting input
std::string trim(std::string s) {
  const char *ws = " \t\r\n";
  auto l = s.find_first_not_of(ws);
  if (l == std::string::npos)
    return "";
  auto r = s.find_last_not_of(ws);
  return s.substr(l, r - l + 1);
}

void script(FileBrowser &browser) {
  std::cout << "---- GITWORK ----\n";
  std::cout
      << "[Q] to quit selection.\n[U] to go up tree.\n"
         "[#] to select file/directory.\n[M] to "
         "save our staged files\n[X] to delete repository.\n[S] to stage "
         "changes.\n[B] to switch branches.\n[N] to create a new branch.\n\n\n";
  browser.printPath();
  std::cout << "\n\n\n";
}

void og_script(FileBrowser &browser) {
  std::cout << "---- File Explorer ----\n";
  std::cout
      << "Choose path to initialize or return to.\n[Q] to quit selection.\n[U] "
         "to go up tree.\n[#] to select directory.\n[G] to solidify "
         "location.\n";
  browser.printPath();
}

fs::path fileExplorer(const fs::path &path) {
  FileBrowser browser(path);
  std::string og_input;
  clearScreen();
  og_script(browser);
  while (std::getline(std::cin, og_input)) {
    clearScreen();
    og_script(browser);
    std::string t = trim(og_input);

    std::transform(t.begin(), t.end(), t.begin(), ::tolower);

    if (browser.containsGit()) {
      auto p = browser.current();
      clearScreen();
      gitWorker(p);
      continue;
    }

    if (t == "q") {
      return browser.current();
    } else if (t == "u") {
      browser.upTree();
      clearScreen();
      og_script(browser);
      continue;
    } else if (t == "g") {
      if (!fs::is_directory(browser.current()) ||
          !fs::exists(browser.current())) {
        std::cout << "[!] Invalid path for init location.\n";
      }
      auto p = browser.current();
      gitWorker(p);
    } else {
      std::stringstream tt(t);
      int t_val;
      char t_leftover;
      if (tt >> t_val && !(tt >> t_leftover)) {
        if (t_val < 0 || t_val >= browser.numPaths()) {
          std::cout << "[!] Out of bounds for init.\n";
        }

        browser.selectChild(t_val);
        clearScreen();
        og_script(browser);
      } else {
        std::cout << "[!] Invalid input for file explorer.\n";
      }
    }
  }

  return path;
}

void gitWorker(fs::path &path) {
  FileBrowser setup(path);
  Internals ints(path);

  clearScreen();
  script(setup);

  std::string input;
  while (std::getline(std::cin, input)) {
    clearScreen();
    script(setup);
    std::string s = trim(input);

    // normalize to lower-case
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if (!setup.containsGit()) {
      auto p = setup.current();
      fileExplorer(p);
      continue;
    }

    if (s == "u") {
      setup.upTree();

      if (!setup.containsGit()) {
        auto p = setup.current();
        fileExplorer(p);
      }
    }
    // Currently broken
    else if (s == "x") {
      ints.~Internals();
      break;
    }

    // Want to add functionality to exit out of this and other option if we had
    // a misclick
    else if (s == "b") {
      // switch to existing branch
      ints.printBranches();
      int b;
      std::cout << "Pick the index for the branch you want: ";
      std::cin >> b;
      ints.chooseBranch(b);

    } else if (s == "n") {
      // create new branch
      std::string name;
      std::cout << "New Branch: ";
      std::cin >> name;
      ints.createBranch(name);

    }

    else if (s == "s") {
      // For now, if we stage, we will get rid of any stage that
      // there already exists
      //
      // Want to add some more interface dynamics/visuals to show this action
      // was successful
      ints.stage();
      std::cout << "Staged successful!\n";
    }

    else if (s == "m") {
      if (!ints.isStageFull()) {
        continue;
        std::cout << "No files staged to be commited.\n";
      }
      std::string message;
      std::cout << "Write your message: \n";
      std::cin >> message;
      ints.objectify(message);
    } else if (s == "q") {
      return;
    }

    else {
      std::stringstream ss(s);
      int val;
      char leftover;
      if (ss >> val && !(ss >> leftover)) {
        if (val < 0 || val >= setup.numPaths()) {
          std::cout << "[!] Out of bounds.\n";
        }

        // pick path or file work
        setup.selectChild(val);
        clearScreen();
        script(setup);

      } else {
        std::cout << "[!] Invalid input for gitwork-system.\n";
      }
    }
  }

  fileExplorer(setup.current());
}

int main() {
  fs::path start =
      fs::current_path(); // Want to eventually update starting location
  fs::path dir = fileExplorer(start);

  return 0;
}
