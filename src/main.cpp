#include <cstdlib>
#include <fstream>
#include <iostream>

#include <sodium.h>

#include "device.hpp"
#include "tui.hpp"

int main(int argc, char *argv[]) {
	std::ostream *out = &std::cout;
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
	TuiApp app(device, out);
	app.run();

	return EXIT_SUCCESS;
}
