#include "flags.hpp"

int main(int argc, char **argv) {

    flags::init("greet", "Greeting App",
                "Enter you name and I will greet you!");

    std::string name;
    bool working;

    flags::add("--name", "This is where you enter your name", &name, true);
    flags::add("--working", "This denotes that you are working", &working);

    flags::parse(argc, argv);

    std::cout << "Hi, " << name << std::endl;
    if (working)
        std::cout << "Good luck with your work" << std::endl;

    return 0;
}
