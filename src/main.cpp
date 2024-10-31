#include <sstream>
#include <iostream>
#include <optional>
#include <functional>
#include <unordered_map>

void cmdEcho(std::istringstream& iss);

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::unordered_map<std::string, std::function<void(std::istringstream&)>> cmds = {
        {"type", nullptr},
        {"exit", nullptr},
        {"echo", cmdEcho},
    };

    while (true) {
        std::cout << "$ ";

        std::string input;
        std::getline(std::cin, input);

        std::istringstream iss(input);
        iss >> input;

        if (input == "exit") {
            iss >> input;
            try {
                return stoi(input);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << '\n';
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range: " << e.what() << '\n';
            }
        } else if (input == "type") {
            while (iss >> input) {
                if (cmds.find(input) != cmds.end())
                    std::cout << input << " is a shell builtin\n";
                else
                    std::cerr << input << ": not found\n";
            }
        } else {
            auto it = cmds.find(input);

            if (it != cmds.end())
                it->second(iss);
            else
                std::cerr << input << ": command not found\n";
        }
    }

    return 0;
}

void cmdEcho(std::istringstream& iss) {
    std::string output;

    std::getline(iss, output);
    output.erase(0, output.find_first_not_of(" \t\n"));
    std::cout << output << '\n';
}
