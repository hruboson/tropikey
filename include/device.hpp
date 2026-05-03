#pragma once

#include <iostream>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <array>

#include "key.hpp"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_posix_usb_dongle.h"

struct lt_handle_t;
struct lt_dev_posix_usb_dongle_t;
struct lt_ctx_mbedtls_v4_t;

// Choose pairing keypair for slot 0.
#if LT_USE_SH0_ENG_SAMPLE
#define LT_EX_SH0_PRIV sh0priv_eng_sample
#define LT_EX_SH0_PUB sh0pub_eng_sample
#elif LT_USE_SH0_PROD0
#define LT_EX_SH0_PRIV sh0priv_prod0
#define LT_EX_SH0_PUB sh0pub_prod0
#endif

/**
 * TODO
 *	- query for empty slots (maybe interactive mode where user is prompted to allow initializing new key in empty slot or rewriting any of the 32 slots)
 *	- proper check (and error message) for devkit connected to machine
 *	- proper chain of trust verification (https://tropicsquare.github.io/libtropic/latest/tutorials/linux/usb_devkit/full_chain_verification/)
 *	- relegate signing challenge
 *	- class for representation of key pair (pubkey, slot for privkey, operations, representation, ...)
 */

struct Version {
	uint8_t major = 0;
	uint8_t minor = 0;
	uint8_t patch = 0;
	uint8_t build = 0; // optional
};

class Device {
public:
    Device();
    ~Device();

    bool init();
	bool close();
	bool start_secure_session();

	bool initialize_ed25519_key(lt_ecc_slot_t slot, std::array<uint8_t, ED25519_KEY_LEN>& pubkey);
	bool initialize_ed25519_key(Ed25519Key& key);

	bool read_ed25519_key(lt_ecc_slot_t slot, std::array<uint8_t, ED25519_KEY_LEN>& pubkey);
	bool read_ed25519_key(Ed25519Key& key);

	bool erase_ed25519_key(lt_ecc_slot_t slot);
	bool erase_ed25519_key(Ed25519Key& key);

	bool sign_ed25519_challenge(lt_ecc_slot_t slot, std::vector<uint8_t>& challenge, std::vector<uint8_t>& signature);
	bool sign_ed25519_challenge(Ed25519Key key, std::vector<uint8_t>& challenge, std::vector<uint8_t>& signature);

    bool print_info(std::ostream& out);
	const Version& get_hw_version() const { return hw_version; }
    const Version& get_fw_version() const { return fw_version; }

private:
	lt_handle_t* lt_handle_ptr();

    lt_handle_t lt_handle{};
    lt_dev_posix_usb_dongle_t dongle{};
    lt_ctx_mbedtls_v4_t crypto_ctx{};

    bool fail(const char* msg, lt_ret_t ret);

    Version hw_version{};
    Version fw_version{};
};
