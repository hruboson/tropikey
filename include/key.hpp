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

		// raw pointer access for lt_ecc_key_read
		uint8_t* data() { return pubkey.data(); };
		const uint8_t* data() const { return pubkey.data(); };

		// pubkey access
		std::array<uint8_t, ED25519_KEY_LEN>& get_pubkey() { return pubkey; };
		const std::array<uint8_t, ED25519_KEY_LEN>& get_pubkey() const { return pubkey; };

		lt_ecc_slot_t get_slot() const { return slot; };

		lt_ecc_curve_type_t  get_curve()  const { return curve; }
		lt_ecc_key_origin_t  get_origin() const { return origin; }
		lt_ecc_curve_type_t* curve_ptr()        { return &curve; }
		lt_ecc_key_origin_t* origin_ptr()       { return &origin; }
	private:
		lt_ecc_slot_t slot;
		std::array<uint8_t, ED25519_KEY_LEN> pubkey{}; // initialized to zeros

		lt_ecc_curve_type_t                  curve  = TR01_CURVE_ED25519;
		lt_ecc_key_origin_t                  origin = TR01_CURVE_GENERATED;
};
