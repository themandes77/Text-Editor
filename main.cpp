#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>


bool editing = true;

void exit_editor(std::ifstream &file) {
    editing = false;
    file.close();
}

const char *builtin[] = {
    "q",
    "c",
    "w",
    "p",
    "h"
};


int num_builtins() {
    return sizeof(builtin) / sizeof (char *);
}

void print_commands() {
    int i;
    for (i = 0; i < num_builtins(); i++) {
        std::printf("%s\n", builtin[i]);
    }
}

void prompt(std::vector<std::string> &lines) {
    int line_number = 0;
    for (const auto &line : lines) {
        std::cout << line_number++ << "  " << line << '\n';
    }
}

void clear_screen() { std::cout << "\033[2J\033[1;1H"; }

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
            std::cout << "bigger" << current_line << ", " << line_number << '\n';
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
        int line_number;

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
            if (command == "p") {
                prompt(lines);
                continue;
            }
            if (command == "w") {
                save_changes(file_name, lines);
                continue;
            }
            if (command == "c") {
                if (line_number >= 0) {
                    std::string new_content;
                    std::getline(std::cin, new_content);

                    edit_line(file, line_number, lines, new_content);
                    continue;
                }
            if (command == "h") {
                std::cout << "list of commands: \n";
                print_commands();
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
