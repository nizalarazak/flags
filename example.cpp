#include "flags.hpp"
#include <iostream>

int main(int argc, char **argv) {

    flags::init("Zyphor", "Backup management tool");

    std::string name;
    flags::flag("--name", "This is where you give your name", name);

    std::string work;
    flags::flag("--occupation", "This is where you enter what you do", work);

    int age;
    flags::flag("--age", "This is where you give your age", age);

    flags::parse(argc, argv);

    return 0;
}
