#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdarg>
#include <vector>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_posix_usb_dongle.h"
#include "psa/crypto.h"

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

#define ED25519_LEN 32

/**
 * TODO
 *	- query for empty slots (maybe interactive mode where user is prompted to allow initializing new key in empty slot or rewriting any of the 32 slots)
 *	- proper check (and error message) for devkit connected to machine
 *	- proper chain of trust verification (https://tropicsquare.github.io/libtropic/latest/tutorials/linux/usb_devkit/full_chain_verification/)
 *	- relegate signing challenge
 *	- class for representation of key pair (pubkey, slot for privkey, operations, representation, ...)
 */

class Device {
public:
    Device();
    ~Device();

    bool init();
	bool close();
	bool start_secure_session();

	bool initialize_ed25519_key(lt_ecc_slot_t slot, std::vector<uint8_t>& pubkey);
	bool read_ed25519_key(lt_ecc_slot_t slot, std::vector<uint8_t>& pubkey);
	bool erase_ed25519_key(lt_ecc_slot_t slot);

	bool sign_ed25519_challenge(lt_ecc_slot_t slot, std::vector<uint8_t>& challenge, std::vector<uint8_t>& signature);

    bool print_info(std::ostream& out);

private:
    lt_handle_t* lt_handle_ptr();

    lt_handle_t lt_handle{};
    lt_dev_posix_usb_dongle_t dongle{};
    lt_ctx_mbedtls_v4_t crypto_ctx{};

    bool fail(const char* msg, lt_ret_t ret);
};
