#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdarg>

#include "device.hpp"

int main(int argc, char* argv[]) {
    std::ostream* out = &std::cout;
    std::ofstream file;

    if (argc > 1) {
        file.open(argv[1]);
        if (!file) {
            std::cerr << "Error: could not open file '" << argv[1] << "'\n";
            return EXIT_FAILURE;
        }
        out = &file;
    }

    Device device;

    if (!device.init()) {
        return EXIT_FAILURE;
    }

    if (!device.print_info(*out)) {
        return EXIT_FAILURE;
    }

    std::cout << "Successfully read device information";
    if (argc > 1) {
        std::cout << ". Saved in " << argv[1];
    }
    std::cout << "\n";

    return EXIT_SUCCESS;
}
