#include "fileSystem.hpp"
#include "hash.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

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
  std::cout << "[Q] to quit selection.\n[U] to go up tree.\n[D#] to remove "
               "from selected files.\n[#] to select file/directory.\n\n\n";
  browser.printPath();
  std::cout << "\n\n\n";
  browser.printSelected();
}

int main() {
  fs::path rootDir = fs::current_path();
  FileBrowser browser(rootDir);

  // Initial run of file-system
  script(browser);

  std::string input;
  while (std::getline(std::cin, input)) {
    clearScreen();
    script(browser);
    std::string s = trim(input);

    // normalize to lower-case
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if (s == "u") {
      browser.upTree();
      clearScreen();
      script(browser);
      continue;
    } else if (s == "q") {
      break;
    } else if (s == "d") {
      if (browser.numSel() == 0) {
        std::cout << "Nothing to delete...\n";
      }
      std::cout << "Choose the row to delete: ";
      std::string input2;
      std::cin >> input2;
      s = trim(input2);

      std::stringstream ss(s);
      int val;
      char leftover;
      if (ss >> val && !(ss >> leftover)) {
        if (val < 1 || val > browser.numSel()) {
          std::cout << "[!] Out of bounds selected.\n";
        }

        browser.deleteSelected(val);
        // The 1-indexedness is fixed in the function

        clearScreen();
        script(browser);
        continue;
      } else {
        std::cout << "[!] Invalid input2.\n";
        continue;
      }
    }

    else {
      std::stringstream ss(s);
      int val;
      char leftover;
      if (ss >> val && !(ss >> leftover)) {
        if (val < 0 || val >= browser.numPaths()) {
          std::cout << "[!] Out of bounds.\n";
        }

        // pick path or file work
        browser.selectChild(val);
        clearScreen();
        script(browser);

      } else {
        std::cout << "[!] Invalid input1.\n";
      }
    }
  }

  return 0;
}
