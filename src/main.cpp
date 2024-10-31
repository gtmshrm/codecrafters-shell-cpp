#include <sstream>
#include <iostream>
#include <optional>


std::optional<int> cmdExit(std::istringstream& iss);

std::string cmdEcho(std::istringstream& iss);


int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        std::cout << "$ ";

        std::string input;
        std::getline(std::cin, input);

        std::istringstream iss(input);
        iss >> input;

        if (input == "exit") {
            if (auto ret = cmdExit(iss))
                return *ret;
        } else if (input == "echo") {
            std::cout << cmdEcho(iss) << std::endl;
        } else {
            std::cerr << input << ": command not found\n";
        }
    }

    return 0;
}

std::optional<int> cmdExit(std::istringstream& iss) {
    std::string input;

    try {
        iss >> input;
        return stoi(input);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << '\n';
    }

    return std::nullopt;
}

std::string cmdEcho(std::istringstream& iss) {
    std::string output;
    std::getline(iss, output);
    output.erase(0, output.find_first_not_of(" \t\n"));
    return output;
}
