#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdarg>

#include <random>
#include <sodium.h>

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

std::vector<uint8_t> generate_challenge(size_t size = 32) {
    std::vector<uint8_t> challenge(size);

    std::random_device rd;
    for (auto& b : challenge) {
        b = static_cast<uint8_t>(rd());
    }

    return challenge;
}

bool verify_signature(
    const std::vector<uint8_t>& pubkey,
    const std::vector<uint8_t>& challenge,
    const std::vector<uint8_t>& signature)
{
    if (pubkey.size() != crypto_sign_PUBLICKEYBYTES)  return false; // 32
    if (signature.size() != crypto_sign_BYTES)        return false; // 64

    return crypto_sign_verify_detached(
        signature.data(),
        challenge.data(), challenge.size(),
        pubkey.data()) == 0;
}

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
	std::vector<uint8_t> pubkey(ED25519_LEN, 0);
	device.initialize_ed25519_key(slot_write_to, pubkey);
	device.read_ed25519_key(slot_write_to, pubkey);

	auto challenge = generate_challenge();

	std::vector<uint8_t> signature;

	device.sign_ed25519_challenge(slot_write_to, challenge, signature);

	if (verify_signature(pubkey, challenge, signature)) {
		std::cout << "Signature VALID\n";
	} else {
		std::cout << "Signature INVALID\n";
	}

	if(!device.close()){
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}
