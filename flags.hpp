#ifndef FLAGS_H
#define FLAGS_H

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>

namespace {

inline std::string *appCommand = new std::string{};
inline std::string *appName = new std::string{};
inline std::string *appInfo = new std::string{};

struct arg {
    std::string argName, argInfo, alias;
    std::variant<std::string *, int *, double *, bool *> reference;
    bool required, updated;
};

inline std::vector<arg> *args = new std::vector<arg>;

inline void printHelp(bool usageOnly = false) {
    // usageOnly flag prints help without app info
    // for when printing help in case of an error
    if (!(*appName).empty() && !usageOnly)
        std::cout << *appName << "\n" << *appInfo << std::endl;

    if (!args->empty()) {
        std::cout << "\nUsage: \n    " << *appCommand
                  << " [option] [argument]\n    " << *appCommand << " [flag]"
                  << std::endl;

        auto getType = [](const arg &a) {
            if (std::holds_alternative<std::string *>(a.reference)) {
                return "<string>";
            } else if (std::holds_alternative<int *>(a.reference)) {
                return "<integer>";
            } else if (std::holds_alternative<double *>(a.reference)) {
                return "<double>";
            } else if (std::holds_alternative<bool *>(a.reference)) {
                return "<bool>";
            }
            return "unknown type";
        };

        bool options{}, flags{};
        // dynamic sizing of spaces by looking for longest command
        size_t longestCharCount = 0;
        std::string_view type;
        for (const arg &arg : *args) {
            type = getType(arg);
            if (!flags && type == "<bool>")
                flags = true;
            if (!options && type != "<bool>")
                options = true;
            if (type == "<bool>" && arg.argName.size() > longestCharCount) {
                longestCharCount = arg.argName.size();
            } else if (arg.argName.size() + type.size() > longestCharCount) {
                longestCharCount = arg.argName.size() + type.size();
            }
        }

        // same as above but for aliases
        size_t longestAliasCount = 0;
        auto it = std::max_element(args->begin(), args->end(),
                                   [](const arg &a, const arg &b) {
                                       return a.alias.size() < b.alias.size();
                                   });
        if (it != args->end())
            longestAliasCount = it->alias.size();

        if (options) {
            std::cout << "\nOptions:" << std::endl;

            for (const arg &arg : *args) {
                type = getType(arg);
                if (type != "<bool>") {
                    std::string aliasPadding(
                        longestAliasCount - arg.alias.size(), ' ');
                    std::string padding(longestCharCount + 5 -
                                            (arg.argName.size() + type.size()),
                                        ' ');
                    std::cout << "    " << arg.alias << ", " << aliasPadding
                              << arg.argName << " " << type << padding << " "
                              << arg.argInfo
                              << (arg.required ? " (required)" : "") << "\n";
                }
            }
        }

        if (flags) {
            std::cout << "\nFlags:" << std::endl;

            for (const arg &arg : *args) {
                type = getType(arg);
                if (type == "<bool>") {
                    std::string aliasPadding(
                        longestAliasCount - arg.alias.size(), ' ');
                    std::string padding(
                        longestCharCount + 5 - arg.argName.size(), ' ');
                    std::cout << "    " << arg.alias << ", " << aliasPadding
                              << arg.argName << padding << "  " << arg.argInfo
                              << (arg.required ? " (required)" : "") << "\n";
                }
            }
        }

        std::cout << std::endl;
    }
}

inline void checkForRequired() {
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

/**
 * @brief Initializes basic information about your application for help output
 *
 * @param command The name of the binanry executable
 * @param name The name of your applciation
 * @param info Brief info about your applciation
 *
 * @note name and info will be print out when user asks for --help
 */
inline void init(std::string command, std::string name, std::string info) {
    *appCommand = command;
    *appName = name;
    *appInfo = info;
}

/**
* @brief Add a new argument to parse
*
* @param name Name of the arguement
* @param info A short description shown in help output
* @param var A reference of the variable where the parsed value is to be
stored
* @param required If this arguement must be provided
* @param alias A shorter name for the argument name
*
* @note required and alias are optional. alias will be autogenerated as -[first
character of command name] unless specified
 */
inline void add(std::string name, std::string info,
                std::variant<std::string *, int *, double *, bool *> var,
                bool required = false, std::string alias = "") {

    auto getAlias = [name]() {
        auto it = std::find_if(name.begin(), name.end(),
                               [](char c) { return c != '-'; });
        if (it == name.end())
            throw std::invalid_argument(
                "\nName of a given argument has no characters in it");
        std::string alias = "-";
        alias += *it;
        // check for collisions with previous aliases
        for (const arg &arg : *args) {
            while (arg.alias == alias) {
                it++;
                if (it == name.end()) {
                    throw std::runtime_error(
                        "\nCannot generate an alias for " + name +
                        " due to conflict with " + arg.argName +
                        ". Please specify another alias for " + name + " or " +
                        arg.argName + ".");
                    break;
                }
                alias += *it;
            }
        }
        return alias;
    };

    auto it = std::find_if(args->begin(), args->end(),
                           [name](const arg &a) { return a.argName == name; });
    if (it != args->end())
        throw std::invalid_argument("\nArg name " + name +
                                    " is being added more than once");

    it = std::find_if(args->begin(), args->end(),
                      [name](const arg &a) { return a.alias == name; });
    if (it != args->end())
        throw std::invalid_argument("\n" + name + " is already an alias for " +
                                    it->argName);

    it = std::find_if(args->begin(), args->end(),
                      [alias](const arg &a) { return a.alias == alias; });
    if (it != args->end())
        throw std::invalid_argument("\nAlias" + alias + " is also used by " +
                                    it->argName);

    arg newArg;
    newArg.argName = name;
    newArg.argInfo = info;
    newArg.reference = var;
    newArg.required = required;
    newArg.updated = false;
    alias.empty() ? newArg.alias = getAlias() : newArg.alias = alias;
    (*args).push_back(newArg);
}

/**
 * @brief Load CLI args into added variables
 *
 * @params argc Argument count given to int main()
 * @params argv Argument values given to int main()
 */
inline void parse(int argc, char **argv) {
    if (argc == 1) {
        std::cout
            << "No arguments given. Please enter --help for more information"
            << std::endl;
        exit(1);
    }

    if (args->size() == 0) {
        std::cout << "No arguments registered for this application";
        exit(0);
    }

    for (int i = 1; i < argc; i++) {
        std::string_view currentArg = argv[i];
        if (currentArg == "--help" || currentArg == "-h") {
            printHelp();
            exit(0);
        }

        for (size_t j = 0; j <= args->size(); j++) {
            if (j == (*args).size()) {
                std::cout << "Error: Unrecognized parameter " << currentArg
                          << std::endl;
                printHelp(true);
                exit(1);
            }
            if (currentArg == (*args)[j].argName ||
                currentArg == (*args)[j].alias) {

                const auto &var = (*args)[j].reference;

                // handling flags
                if (std::holds_alternative<bool *>(var)) {
                    bool *reference = std::get<bool *>(var);
                    *reference = true;
                    (*args)[j].updated = true;
                    break;
                } else if (i == argc - 1) {
                    std::cout << "Error: Value not specified for parameter "
                              << currentArg << std::endl;
                    exit(1);
                }

                // handling options
                if (std::holds_alternative<std::string *>(var)) {
                    std::string *reference = std::get<std::string *>(var);
                    *reference = std::string_view(argv[i + 1]);
                } else if (std::holds_alternative<int *>(var)) {
                    int *reference = std::get<int *>(var);
                    try {
                        *reference = std::stoi(argv[i + 1]);
                    } catch (std::invalid_argument) {
                        std::cout << "Error: " << currentArg
                                  << " takes an integer parameter" << std::endl;
                        exit(1);
                    }
                } else if (std::holds_alternative<double *>(var)) {
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
