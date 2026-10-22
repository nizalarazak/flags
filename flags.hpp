#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

std::string appName;
std::string appInfo;

std::vector<std::string> argName;
std::vector<std::string> argInfo;

void printHelp() {
    std::cout << appName << "\n" << appInfo << std::endl;
    if (!argName.empty()) {
        std::cout << "\nUsage: " << std::endl;
        int longestCharCount = 0;
        for (auto arg : argName) {
            if (arg.size() > longestCharCount)
                longestCharCount = arg.size() + 1;
        }

        for (int i = 0; i < argName.size(); i++) {
            std::string padding(longestCharCount - argName[i].size(), ' ');
            std::cout << "\t" << argName[i] << padding << ": " << argInfo[i]
                      << std::endl;
        }
    }
}

} // namespace

namespace flags {

void init(std::string name, std::string info) {
    appName = name;
    appInfo = info;
}

void flag(std::string name, std::string info, std::string &var) {
    argName.push_back(name);
    argInfo.push_back(info);
}

void flag(std::string name, std::string info, int &var) {
    argName.push_back(name);
    argInfo.push_back(info);
}

void parse(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string_view arg = argv[i];
        if (arg == "--help") {
            printHelp();
        }
    }
}
} // namespace flags
