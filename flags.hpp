#ifndef FLAGS_H
#define FLAGS_H

#include <any>
#include <iostream>
#include <vector>

namespace {

std::string *appName = new std::string{};
std::string *appInfo = new std::string{};

struct arg {
    std::string argName;
    std::string argInfo;
    std::string alias;
    std::any reference;
    bool required;
};

std::vector<arg> *args = new std::vector<arg>;

void printHelp(bool usageOnly = false) {
    if (!(*appName).empty() && !usageOnly)
        std::cout << *appName << "\n" << *appInfo << std::endl;

    if (!args->empty()) {
        std::cout << "\nUsage: " << std::endl;
        int longestCharCount = 0;
        for (auto flag : *args) {
            if (flag.argName.size() > longestCharCount)
                longestCharCount = flag.argName.size() + 1;
        }

        auto getType = [](int i) {
            if ((*args)[i].reference.type() == typeid(std::string *)) {
                return "<string>";
            } else if ((*args)[i].reference.type() == typeid(int *)) {
                return "<integer>";
            } else if ((*args)[i].reference.type() == typeid(float *)) {
                return "<float>";
            }
            return "unknown type";
        };

        for (int i = 0; i < args->size(); i++) {
            std::string padding(longestCharCount - (*args)[i].argName.size(),
                                ' ');
            std::cout << "\t" << (*args)[i].argName << padding << ": "
                      << (*args)[i].argInfo << " " << getType(i) << "\n";
        }

        std::cout << std::endl;
    }
}

} // namespace

namespace flags {

void init(std::string name, std::string info) {
    *appName = name;
    *appInfo = info;
}

void flag(std::string name, std::string info, std::any var,
          bool required = false, std::string alias = "") {
    arg newArg;
    newArg.argName = name;
    newArg.argInfo = info;
    newArg.reference = var;
    newArg.required = required;
    alias.empty() ? newArg.alias = name.substr(1, 2) : newArg.alias = alias;
    (*args).push_back(newArg);
}

void parse(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string_view currentArg = argv[i];
        if (currentArg == "--help" || currentArg == "-h") {
            printHelp();
            continue;
        }

        for (int j = 0; j <= (*args).size(); j++) {
            if (currentArg == (*args)[j].argName ||
                currentArg == (*args)[j].alias) {
                if (i == argc - 1) {
                    std::cout << "Error: Value not specified for parameter "
                              << currentArg << std::endl;
                    exit(1);
                }
                auto var = (*args)[j].reference;
                if (var.type() == typeid(std::string *)) {
                    std::string *reference = std::any_cast<std::string *>(var);
                    *reference = std::string_view(argv[i + 1]);
                } else if (var.type() == typeid(int *)) {
                    int *reference = std::any_cast<int *>(var);
                    *reference = std::stoi(argv[i + 1]);
                } else if (var.type() == typeid(float *)) {
                    float *reference = std::any_cast<float *>(var);
                    *reference = std::stof(argv[i + 1]);
                }
                i++;
                break;
            } else if (j == (*args).size()) {
                std::cout << "Error: Unrecognized parameter " << currentArg
                          << std::endl;
                printHelp(true);
                exit(1);
            }
        }
    }

    delete appName;
    delete appInfo;

    delete args;
}
} // namespace flags

#endif
