#ifndef FLAGS_H
#define FLAGS_H

#include <iostream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace {

std::string *appCommand = new std::string{};
std::string *appName = new std::string{};
std::string *appInfo = new std::string{};

struct arg {
    std::string argName, argInfo, alias;
    std::variant<std::string *, int *, double *, bool *> reference;
    bool required, updated;
};

std::vector<arg> *args = new std::vector<arg>;

void printHelp(bool usageOnly = false) {
    if (!(*appName).empty() && !usageOnly)
        std::cout << *appName << "\n" << *appInfo << std::endl;

    if (!args->empty()) {
        std::cout << "\nUsage: \n    " << *appCommand
                  << " [option] [argument]\n    " << *appCommand << " [flag]"
                  << std::endl;

        auto getType = [](int i) {
            if ((*args)[i].reference.index() == 0) {
                return "<string>";
            } else if ((*args)[i].reference.index() == 1) {
                return "<integer>";
            } else if ((*args)[i].reference.index() == 2) {
                return "<double>";
            } else if ((*args)[i].reference.index() == 3) {
                return "<bool>";
            }
            return "unknown type";
        };

        int longestCharCount = 0;
        std::string_view type;
        for (int i = 0; i < args->size(); i++) {
            type = getType(i);
            if (type == "<bool>" &&
                (*args)[i].argName.size() > longestCharCount) {
                longestCharCount = (*args)[i].argName.size();
            } else if ((*args)[i].argName.size() + type.size() >
                       longestCharCount)
                longestCharCount = (*args)[i].argName.size() + type.size();
        }

        std::cout << "\nOptions:" << std::endl;

        for (int i = 0; i < args->size(); i++) {
            type = getType(i);
            if (type != "<bool>") {
                std::string padding(
                    longestCharCount + 3 -
                        ((*args)[i].argName.size() + type.size()),
                    ' ');
                std::cout << "    " << (*args)[i].argName << " " << type
                          << padding << " " << (*args)[i].argInfo
                          << ((*args)[i].required ? " (required)" : "") << "\n";
            }
        }

        std::cout << "\nFlags:" << std::endl;

        for (int i = 0; i < args->size(); i++) {
            type = getType(i);
            if (type == "<bool>") {
                std::string padding(
                    longestCharCount + 3 - (*args)[i].argName.size(), ' ');
                std::cout << "    " << (*args)[i].argName << padding << "  "
                          << (*args)[i].argInfo
                          << ((*args)[i].required ? " (required)" : "") << "\n";
            }
        }

        std::cout << std::endl;
    }
}

void checkForRequired() {
    for (const auto &arg : *args) {
        if (arg.required == true && arg.updated == false) {
            std::cout << "Error: missing required parameter " << arg.argName
                      << std::endl;
            exit(1);
        }
    }
}

} // namespace

namespace flags {

void init(std::string command, std::string name, std::string info) {
    *appCommand = command;
    *appName = name;
    *appInfo = info;
}

void add(std::string name, std::string info,
         std::variant<std::string *, int *, double *, bool *> var,
         bool required = false, std::string alias = "") {
    arg newArg;
    newArg.argName = name;
    newArg.argInfo = info;
    newArg.reference = var;
    newArg.required = required;
    newArg.updated = false;
    alias.empty() ? newArg.alias = name.substr(1, 2) : newArg.alias = alias;
    (*args).push_back(newArg);
}

void parse(int argc, char **argv) {
    if (argc == 1) {
        std::cout
            << "No parameter given. Please enter --help for more information"
            << std::endl;
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        std::string_view currentArg = argv[i];
        if (currentArg == "--help" || currentArg == "-h") {
            printHelp();
            exit(1);
        }

        for (int j = 0; j <= (*args).size(); j++) {
            if (currentArg == (*args)[j].argName ||
                currentArg == (*args)[j].alias) {

                auto var = (*args)[j].reference;

                if (var.index() == 3) {
                    bool *reference = std::get<bool *>(var);
                    *reference = true;
                    break;
                } else if (i == argc - 1) {
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
                    double *reference = std::get<double *>(var);
                    try {
                        *reference = std::stod(argv[i + 1]);
                    } catch (std::invalid_argument) {
                        std::cout << "Error: " << currentArg
                                  << " takes a double parameter" << std::endl;
                        exit(1);
                    }
                }
                (*args)[j].updated = true;
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

    checkForRequired();

    delete appCommand;
    delete appName;
    delete appInfo;

    delete args;
}
} // namespace flags

#endif
