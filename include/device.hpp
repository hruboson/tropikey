#pragma once

#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

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
 * @struct Version
 * @brief TROPIC01 version structure.
 *
 * Holds the chip's version in this minimal struct. Used mostly for PKCS#11 info output.
 */
struct Version {
	uint8_t major = 0;
	uint8_t minor = 0;
	uint8_t patch = 0;
	uint8_t build = 0; // optional
};

/**
 * @class Device
 * @brief High-level abstraction of the TROPIC01 chip handle.
 *
 * Provides convenient functions for working with the USB Devkit
 *
 * @todo proper check (and error message) for devkit connected to machine
 * @todo proper chain of trust verification
 */
class Device {
public:
	/**
	 * @brief Device's constructor
	 *
	 * @param[in] out general/info stream
	 * @param[in] err warning/error stream
	 *
	 * @note
	 * - no streams passed - both out and err are silenced (discarded).
	 * - one stream given - both out and err point at it.
	 * - two streams given - out and err point at their respective streams.
	 * - nullptr passed for either is also treated as "silence", so callers can't
	 *   accidentally crash on a null deref.
	 */
	Device();
	Device(std::ostream *out);
	Device(std::ostream *out, std::ostream *err);
	~Device();

	// change to false to scan all 32 available slots on the HW, right
	// right now I have no use for all 32 slots so I just use one - the first one
	bool single_key_mode = true;

	//! @brief Initilazes communication with the TROPIC01 chip on the USB devkit
	bool init();
	//! @brief Closes all communications with the TROPIC01 chip on the USB devkit
	bool close();
	//! @brief Starts secure session with the TROPIC01 chip on the USB devkit using the
	//! `lt_verify_chip_and_start_secure_session`
	bool start_secure_session();

	/**
	 * @brief Initializes/generates a key in given slot on the TROPIC01 chip.
	 *
	 * @param[in] slot Number of the slot on the TROPIC01 chip to generate the key in.
	 * @param[out] pubkey Reference to the field that will hold public key of the generated priv/pub
	 * keys.
	 */
	bool initialize_ed25519_key(lt_ecc_slot_t slot, std::array<uint8_t, ED25519_KEY_LEN> &pubkey);

	/**
	 * @brief Initializes/generates a key in given slot on the TROPIC01 chip.
	 *
	 * Uses the Ed25519Key high-level key abstraction instance.
	 *
	 * @param[in/out] key Initialized instance of the Ed25519Key abstraction structure. Public key
	 * is written into the instance.
	 */
	bool initialize_ed25519_key(Ed25519Key &key);

	/**
	 * @brief Reads the public key from a given slot on the TROPIC01 chip.
	 *
	 * @param[in] slot Slot to read the key from
	 * @param[out] pubkey Reference to the field that will hold the read public key
	 */
	bool read_ed25519_key(lt_ecc_slot_t slot, std::array<uint8_t, ED25519_KEY_LEN> &pubkey);

	/**
	 * @brief Reads the public key from a given slot on the TROPIC01 chip.
	 *
	 * @param[in/out] key Initialized instance of the Ed25519Key abstraction structure. Public key
	 * is written into the instance.
	 */
	bool read_ed25519_key(Ed25519Key &key);

	/**
	 * @brief Reads the public key from a give slot on the TROPIC01 chip.
	 * @note internal silent use
	 *
	 * @param[in] slot Number of the slot on the TROPIC01 chip to read the public key from
	 */
	std::optional<Ed25519Key> read_ed25519_key(lt_ecc_slot_t slot); // internal silent use

	/**
	 * @brief Erase the key in a give slot on the TROPIC01 chip.
	 *
	 * @param[in] slot Number of the slot on the TROPIC01 chip to erase the key pair from
	 */
	bool erase_ed25519_key(lt_ecc_slot_t slot);

	/**
	 * @brief Erase the key in a give slot on the TROPIC01 chip.
	 *
	 * @param[in] key Initalized instance of the Ed25519Key abstraction structure.
	 */
	bool erase_ed25519_key(Ed25519Key &key);

	/**
	 * @brief Performs the signing challenge using the keypair in given slot
	 *
	 * @param[in] slot Number of the slot to use the keypair from.
	 * @param[in] challenge The challenge that should be signed
	 * @param[out] signature The buffer where the signature will be stored
	 */
	bool sign_ed25519_challenge(lt_ecc_slot_t slot,
	                            std::vector<uint8_t> &challenge,
	                            std::vector<uint8_t> &signature);

	/**
	 * @brief Performs the signing challenge using the keypair in given slot
	 *
	 * @param[in] key Initialized instance of the Ed25519Key abstraction structure.
	 * @param[in] challenge The challenge that should be signed
	 * @param[out] signature The buffer where the signature will be stored
	 */
	bool sign_ed25519_challenge(Ed25519Key key,
	                            std::vector<uint8_t> &challenge,
	                            std::vector<uint8_t> &signature);

	/**
	 * @brief Scans all slots on the TROPIC01 chip and return a list of Ed25519Key instances for
	 * every found keypair.
	 *
	 * @see Device::read_ed25519_key()
	 */
	std::vector<Ed25519Key> list_ed25519_keys();

	bool print_info();
	const Version &get_hw_version() const { return hw_version; }
	const Version &get_fw_version() const { return fw_version; }

private:
	// debug out
	std::ostream *out;
	std::ostream *err;
	std::ofstream file;

	//! @brief Returns pointer to the lt_handle object
	lt_handle_t *lt_handle_ptr();

	//! @brief Handle for the TROPIC01 chip. Set during the Device::init() function
	lt_handle_t lt_handle{};

	//! @brief Handle for the dongle (USB devkit). Set during the Device::init() function
	lt_dev_posix_usb_dongle_t dongle{};

	//! @brief Buffer for the crypto context set during Device::init() function
	lt_ctx_mbedtls_v4_t crypto_ctx{};

	//! @brief Helper function for error message when communicating through the LIBTROPIC library.
	bool fail(const char *msg, lt_ret_t ret);

	Version hw_version{};
	Version fw_version{};
};
