#ifndef FLAGS_H
#define FLAGS_H

#include <any>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <variant>
#include <vector>

namespace {

std::string *appName = new std::string{};
std::string *appInfo = new std::string{};

struct arg {
    std::string argName;
    std::string argInfo;
    std::string alias;
    std::variant<std::string *, int *, float *, bool *> reference;
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
            if ((*args)[i].reference.index() == 0) {
                return "<string>";
            } else if ((*args)[i].reference.index() == 1) {
                return "<integer>";
            } else if ((*args)[i].reference.index() == 2) {
                return "<float>";
            } else if ((*args)[i].reference.index() == 3) {
                return "<bool>";
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

void flag(std::string name, std::string info,
          std::variant<std::string *, int *, float *, bool *> var,
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

                auto var = (*args)[j].reference;

                if (var.index() == 3) {
                    bool *reference = std::get<bool *>(var);
                    *reference = true;
                    break;
                }

                if (i == argc - 1) {
                    std::cout << "Error: Value not specified for parameter "
                              << currentArg << std::endl;
                    exit(1);
                }

                if (var.index() == 0) {
                    std::string *reference = std::get<std::string *>(var);
                    *reference = std::string_view(argv[i + 1]);
                } else if (var.index() == 1) {
                    int *reference = std::get<int *>(var);
                    try {
                        *reference = std::stoi(argv[i + 1]);
                    } catch (std::invalid_argument) {
                        std::cout << "Error: " << currentArg
                                  << " takes an integer parameter" << std::endl;
                        exit(1);
                    }
                } else if (var.index() == 2) {
                    float *reference = std::get<float *>(var);
                    try {
                        *reference = std::stof(argv[i + 1]);
                    } catch (std::invalid_argument) {
                        std::cout << "Error: " << currentArg
                                  << " takes a float parameter" << std::endl;
                        exit(1);
                    }
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
