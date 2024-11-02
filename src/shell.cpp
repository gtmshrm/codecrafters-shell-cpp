#include "shell.hpp"

Shell::Shell(const std::string prompt) : m_Prompt(prompt) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const char* pathEnv = std::getenv("PATH");
    if (pathEnv) {
        std::string paths(pathEnv);
        TokenizeString(paths, ':');
    } else {
        std::cerr << "Environment variable PATH not found.\n";
    }

    while (true) {
        std::cout << m_Prompt;

        std::string input;
        std::getline(std::cin, input);
        if (std::cin.eof())
            std::exit(0);

        m_ISStream = std::istringstream(input);
        m_ISStream >> input;

        auto it = m_Cmds.find(input);
        if (it != m_Cmds.end())
            it->second();
        else if (!ExecCmd(input, m_ISStream.str()))
            std::cerr << input << ": command not found\n";
    }
}

void Shell::CmdType() {
    std::string input;

    while (m_ISStream >> input) {
        if (m_Cmds.find(input) != m_Cmds.end())
            std::cout << input << " is a shell builtin\n";
        else if (!CheckCmd(input))
            std::cerr << input << ": not found\n";
    }
}

void Shell::CmdExit() {
    std::string input;
    m_ISStream >> input;

    try {
        std::exit(std::stoi(input));
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << '\n';
    }
}

void Shell::CmdEcho() {
    std::string output;

    std::getline(m_ISStream, output);
    output.erase(0, output.find_first_not_of(" \t\n"));
    std::cout << output << '\n';
}

void Shell::TokenizeString(const std::string& str, const char delim) {
    std::string token;
    std::istringstream iss(str);

    while (std::getline(iss, token, delim))
        m_PathEnv.push_back(token);
}

bool Shell::CheckCmd(const std::string& cmd) {
    for (const auto& dir : m_PathEnv) {
        const fs::path path = dir / fs::path(cmd);
        if (fs::exists(path)) {
            std::cout << cmd << " is " << path.string() << '\n';
            return true;
        }
    }
    
    return false;
}

bool Shell::ExecCmd(const std::string& cmd, const std::string cmdWithArgs) {
    for (const auto& dir : m_PathEnv) {
        const fs::path path = dir / fs::path(cmd);
        if (fs::exists(path)) {
            system(cmdWithArgs.c_str());
            return true;
        }
    }
    
    return false;
}
