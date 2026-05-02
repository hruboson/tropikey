#pragma once

#include "libtropic_common.h"

#include <cstdint>
#include <array>
#include <string>

#define ED25519_KEY_LEN 32

class Ed25519Key {
	public:
		Ed25519Key(lt_ecc_slot_t slot);
		std::string to_ssh_ed25519();

		uint8_t* data();
		const uint8_t* data() const;
		std::array<uint8_t, ED25519_KEY_LEN>& get_pubkey();
		const std::array<uint8_t, ED25519_KEY_LEN>& get_pubkey() const;

		lt_ecc_slot_t get_slot() const;

	private:
		lt_ecc_slot_t slot;
		bool initialized;
		std::array<uint8_t, ED25519_KEY_LEN> pubkey{}; // initialized to zeros
};
