#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdarg>

#include "device.hpp"

#define CK_PTR *
#define CK_DECLARE_FUNCTION(returnType, name) returnType name
#define CK_DECLARE_FUNCTION_POINTER(returnType, name) returnType (*name)
#define CK_CALLBACK_FUNCTION(returnType, name) returnType (*name)

#ifndef NULL_PTR
#define NULL_PTR nullptr
#endif

extern "C" {
#include <pkcs11.h>
}

CK_RV C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);

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

	if(!device.start_secure_session()){
		return EXIT_FAILURE;
	}

	lt_ecc_slot_t slot_write_to = TR01_ECC_SLOT_0;
	std::vector<uint8_t> pubkey(32, 0);
	device.initialize_ed25519_key(slot_write_to, &pubkey);

	if(!device.close()){
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}
