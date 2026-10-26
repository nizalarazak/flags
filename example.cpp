#include "flags.hpp"
#include <iostream>

int main(int argc, char **argv) {

    flags::init("greet", "Greeting app v1.2",
                "Introduce yourself and I will greet you!");

    std::string name = "default_name";
    flags::add("--name", "This is where you give your name", &name, true);

    std::string work = "default_job";
    flags::add("--work", "This is where you enter what you do", &work);

    int age{};
    flags::add("--age", "This is where you give your age", &age);

    bool willingToWork = false;
    flags::add("--willingToWork",
               "Entering this will flag you as willing to work", &willingToWork,
               false, "-W");

    flags::parse(argc, argv);

    std::cout << "hi " << name << ", you are " << age
              << " years old and your occupation is " << work << std::endl;

    std::cout << "You are " << (willingToWork ? "" : "not ")
              << "willing to work" << std::endl;

    return 0;
}
