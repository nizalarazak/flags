#include "flags.hpp"
#include <iostream>

int main(int argc, char **argv) {

    flags::init("Greeting app v0.1",
                "Introduce yourself and I will greet you!");

    std::string name = "default_name";
    flags::flag("--name", "This is where you give your name", &name, true,
                "-n");

    std::string work = "default_work";
    flags::flag("--work", "This is where you enter what you do", &work);

    int age{};
    flags::flag("--age", "This is where you give your age", &age);

    bool willingToWork = false;
    flags::flag("--willingToWork",
                "Entering this will flag you as willing to work",
                &willingToWork, false, "-W");

    flags::parse(argc, argv);

    std::cout << "hi " << name << ", you are " << age
              << " years old and your occupation is " << work << std::endl;

    std::cout << "You are " << (willingToWork ? "" : "not ")
              << "willing to work" << std::endl;

    return 0;
}
