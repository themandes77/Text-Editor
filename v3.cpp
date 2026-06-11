#include "test.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

struct termios orig_termios;

void die(const char *s) {
  perror(s);
  _exit(1);
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
    die("raw mode -> tcgetattr");
  }

  struct termios raw = orig_termios;

  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);

  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0;

  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("raw mode -> tcsetattr");
  }
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
    die("disable raw mode -> tcsetattr");
  }
}

bool editing = true;

void exit_editor(std::ifstream &file) {
  editing = false;
  file.close();
}

void prompt(std::vector<std::string> &lines) {
  int line_number = 0;
  for (const auto &line : lines) {
    std::cout << line_number++ << "  " << line << '\n';
  }
}

void save_changes(const std::string &file_name,
                  std::vector<std::string> &lines) {
  std::ofstream file(file_name);
  std::string confirmation;
  std::cout << "confirm changes? [y/n] ";
  std::cin >> confirmation;

  if (confirmation == "y") {
    for (int i = 0; i < lines.size(); ++i) {
      std::cout << i << "   " << lines[i];
      file << lines[i];
      if (i != lines.size()) {
        file << '\n';
      }
    }
  }
  file.close();
}

void edit_line(std::ifstream &file, int current_line,
               std::vector<std::string> &lines, std::string new_content) {
  std::string line;

  file.clear();
  file.seekg(0, std::ios::beg);

  int line_number = 0;

  if (current_line >= lines.size()) {

    while (current_line > lines.size()) {
      lines.push_back("");
      line_number++;
    }

    lines.push_back(new_content);
  } else {
    lines[current_line] = new_content;
  }
}

int main(int argc, char *argv[]) {

  if (argv[1] != NULL) {
    const std::string file_name = argv[1];

    std::ifstream file(file_name);
    std::vector<std::string> lines;
    std::string line;
    int line_number = 0;

    if (file.is_open()) {
      while (std::getline(file, line)) {
        lines.push_back(line);
      }
    }

    while (editing) {
      clear_screen();
      prompt(lines);
      std::string command;
      std::getline(std::cin, command);

      if (command == "q") {
        exit_editor(file);
        continue;
      }
      if (command == "w") {
        save_changes(file_name, lines);
        continue;
      }
      if (command == "c") {
        if (line_number >= 0) {
          std::string new_content;
          std::cout << command[2];
          std::getline(std::cin, new_content);

          edit_line(file, line_number, lines, new_content);
          continue;
        }
      }
      try {
        line_number = std::stoi(command);
        std::cout << line_number << '\n';
        if (line_number >= 0 && line_number < lines.size()) {
          std::cout << lines[line_number] << '\n';
        }
      } catch (const std::invalid_argument &) {
        std::cout << command << " Is not a valid command" << '\n';
      }
    }
  }

  return 0;
}
