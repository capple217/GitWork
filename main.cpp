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
  std::cout << "[Q] to quit selection.\n[U] to go up tree.\n"
               "[#] to select file/directory.\n[M] to "
               "save files\n[X] to delete repository.\n\n\n";
  browser.printPath();
  std::cout << "\n\n\n";
}

void og_script(FileBrowser &browser) {
  std::cout
      << "Choose path to initialize or return to.\n[Q] to quit selection.\n[U] "
         "to go up tree.\n[#] to select directory.\n[G] to solidify "
         "location.\n";
  browser.printPath();
}

int main() {
  clearScreen();
  fs::path rootDir = fs::current_path(); // Can add functioality later to have a
                                         // better start position
  FileBrowser browser(rootDir);

  fs::path dir = rootDir;

  // Choose init location
  og_script(browser);
  std::string og_input;
  while (std::getline(std::cin, og_input)) {
    clearScreen();
    og_script(browser);
    std::string t = trim(og_input);

    std::transform(t.begin(), t.end(), t.begin(), ::tolower);

    if (t == "q") {
      // Breaks code for now
      return 0;
      break;
    } else if (t == "u") {
      browser.upTree();
      clearScreen();
      og_script(browser);
      continue;
    } else if (t == "g") {
      if (!fs::is_directory(browser.current()) ||
          !fs::exists(browser.current())) {
        std::cout << "[!] Invalid path for init.\n";
      }
      dir = browser.current();
      break;
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
        std::cout << "[!] Invalid input init.\n";
      }
    }
  }

  // intentional gap
  // Temporary
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //

  // NEW GIT ENVIRONMENT SET UP
  FileBrowser setup(dir);
  Internals ints(dir);

  // Initial run of file-system
  clearScreen();
  script(setup);

  std::string input;
  while (std::getline(std::cin, input)) {
    clearScreen();
    script(setup);
    std::string s = trim(input);

    // normalize to lower-case
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if (s == "u") {
      setup.upTree();
      clearScreen();
      script(setup);
      continue;
    } else if (s == "x") {
      ints.~Internals();
      clearScreen();
      break;
    }

    else if (s == "m") {
      std::string message;
      std::cout << "Write your message: \n";
      ints.objectify(message);
      clearScreen();
      script(setup);
    } else if (s == "q") {

      break;
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
        std::cout << "[!] Invalid input1.\n";
      }
    }
  }

  return 0;
}
