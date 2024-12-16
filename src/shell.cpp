#include "shell.hpp"

Shell::Shell(const std::string prompt)
    : m_Prompt(prompt)
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const char *pathEnv = std::getenv("PATH");
    if (pathEnv) {
        std::string paths(pathEnv);
        AddPaths(paths, ':');
    } else {
        std::cerr << "Environment variable PATH not found.\n";
    }
    Run();
}

void Shell::Run()
{
    while (true) {
        std::cout << m_Prompt;

        std::string input;
        std::getline(std::cin, input);
        if (std::cin.eof()) {
            std::exit(0);
        }
        m_TokenizedCommand = TokenizeStr(input);

        auto it = m_Commands.find(m_TokenizedCommand.front());
        if (it != m_Commands.end()) {
            it->second();
        } else if (!ExecCommand(input)) {
            std::cerr << m_TokenizedCommand.front() << ": command not found\n";
        }
    }
}

void Shell::CommandType()
{
    for (size_t i = 1; i < m_TokenizedCommand.size(); ++i) {
        const auto &command = m_TokenizedCommand[i];

        if (m_Commands.find(command) != m_Commands.end()) {
            std::cout << command << " is a shell builtin\n";
        } else if (!CheckCommand(m_TokenizedCommand[i])) {
            std::cerr << command << ": not found\n";
        }
    }
}

void Shell::CommandExit()
{
    if (m_TokenizedCommand.size() < 2) {
        std::exit(0);
    } else if (m_TokenizedCommand.size() > 2) {
        std::cerr << "exit: too many arguments";
        return;
    }

    try {
        std::exit(std::stoi(m_TokenizedCommand[1]));
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
    } catch (const std::out_of_range &e) {
        std::cerr << "Out of range: " << e.what() << '\n';
    }
}

void Shell::CommandEcho()
{
    if (m_TokenizedCommand.size() > 1)
        std::cout << m_TokenizedCommand[1];

    for (size_t i = 2; i < m_TokenizedCommand.size(); ++i)
        std::cout << ' ' << m_TokenizedCommand[i];

    std::cout << '\n';
}

void Shell::CommandPwd()
{
    try {
        std::cout << fs::current_path().string() << '\n';
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error getting current working directory: " << e.what()
                  << '\n';
    }
}

void Shell::CommandCd()
{
    if (m_TokenizedCommand.size() == 1) {
        fs::current_path(fs::path(getenv("HOME")));
        return;
    }

    for (size_t i = 1; i < m_TokenizedCommand.size(); ++i) {
        const auto &path = m_TokenizedCommand[i];

        if (fs::exists(path))
            if (fs::is_directory(path))
                fs::current_path(path);
            else
                std::cerr << "cd: not a directory: " << path << '\n';
        else if (path == "~")
            fs::current_path(fs::path(getenv("HOME")));
        else
            std::cerr << "cd: " << path << ": No such file or directory\n";
    }
}

void Shell::AddPaths(const std::string &str, const char delim)
{
    std::string token;
    std::istringstream iss(str);

    while (std::getline(iss, token, delim)) {
        m_PathEnv.push_back(token);
    }
}

bool Shell::CheckCommand(const std::string &command)
{
    for (const auto &dir : m_PathEnv) {
        const fs::path path = dir / fs::path(command);
        if (fs::exists(path)) {
            std::cout << command << " is " << path.string() << '\n';
            return true;
        }
    }
    return false;
}

bool Shell::ExecCommand(const std::string &command)
{
    for (const auto &dir : m_PathEnv) {
        const fs::path path = dir / fs::path(m_TokenizedCommand.front());
        if (fs::exists(path)) {
            system(command.c_str());
            return true;
        }
    }
    return false;
}

std::vector<std::string> Shell::TokenizeStr(const std::string &str)
{
    std::vector<std::string> tokenized;

    for (size_t i = 0; i < str.size();) {
        auto word = getNextWord(str, i);
        tokenized.push_back(word.first);
        i = word.second;
    }
    return tokenized;
}

std::pair<std::string, size_t> Shell::getNextWord(const std::string &str, const size_t start)
{
    size_t idx = start;

    if (str[idx] == '\'') {
        while ((str[++idx] != '\'' || str[idx - 1] == '\\') && idx < str.size());
        auto token = str.substr(start + 1, idx - start - 1);
        for (idx++; !std::isspace(str[idx]) && idx < str.size(); idx++);
        return std::make_pair(std::move(token), idx);
    }

    while (!std::isspace(str[++idx]) && idx < str.size());
    auto token = str.substr(start, idx - start);
    while (std::isspace(str[++idx]) && idx < str.size());
    return std::make_pair(std::move(token), idx);
}
