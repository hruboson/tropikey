/*
 *  Copyright 2011-2025 The Pkcs11Interop Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  Written for the Pkcs11Interop project by:
 *  Jaroslav IMRICH <jimrich@jimrich.sk>
 */

#include <stdio.h>
#include <string.h>

// Removes unused parameter warning
#define UNUSED(x) (void)(x)

#ifdef _WIN32

// PKCS#11 related stuff
#pragma pack(push, cryptoki, 1)

#define CK_IMPORT_SPEC __declspec(dllimport)

#ifdef CRYPTOKI_EXPORTS
#define CK_EXPORT_SPEC __declspec(dllexport)
#else
#define CK_EXPORT_SPEC CK_IMPORT_SPEC
#endif

#define CK_CALL_SPEC __cdecl

#define CK_PTR *
#define CK_DEFINE_FUNCTION(returnType, name) returnType CK_EXPORT_SPEC CK_CALL_SPEC name
#define CK_DECLARE_FUNCTION(returnType, name) returnType CK_EXPORT_SPEC CK_CALL_SPEC name
#define CK_DECLARE_FUNCTION_POINTER(returnType, name)                                              \
	returnType CK_IMPORT_SPEC(CK_CALL_SPEC CK_PTR name)
#define CK_CALLBACK_FUNCTION(returnType, name) returnType(CK_CALL_SPEC CK_PTR name)

#ifndef NULL_PTR
#define NULL_PTR 0
#endif

#include <cryptoki\pkcs11.h>

#pragma pack(pop, cryptoki)

#else // #ifdef _WIN32

// PKCS#11 related stuff
#define CK_PTR *
#define CK_DEFINE_FUNCTION(returnType, name) returnType name
#define CK_DECLARE_FUNCTION(returnType, name) returnType name
#define CK_DECLARE_FUNCTION_POINTER(returnType, name) returnType(*name)
#define CK_CALLBACK_FUNCTION(returnType, name) returnType(*name)

#ifndef NULL_PTR
#define NULL_PTR 0
#endif

#define TROPIKEYSLOTID 0
#define TROPIKEYTOKENID 0
#define TROPIKEYSESSIONID 1

#include "device.hpp"
#include "key.hpp"
#include "pkcs11.h"
#include <mutex>
#include <optional>

/**
 * @brief Module-level state shared across all C_* functions
 *
 * @todo implement the rest of the pkcs#11 functions
 * @todo key intiialization through the ssh-keygen
 * @todo make this work with git remote authentication
 */
struct Pkcs11Module {
	//! @brief Module-wide mutex
	std::mutex mtx;
	bool initialized = false;
	bool session_open = false;

	//! @brief key cache, cleared on Finalize
	std::optional<std::vector<Ed25519Key>> key_cache;

	// find state
	std::vector<CK_OBJECT_HANDLE> found_objects;
	size_t find_index = 0;
	bool find_active = false;

	/**
	 * @brief Slot on the TROPIC01 chip used for signing the challenges.
	 * @note Currently in single-slot mode - the library uses only one slot always.
	 */
	lt_ecc_slot_t sign_key_slot = TR01_ECC_SLOT_0;
	bool sign_active = false;

	/**
	 * @brief Device instance
	 * @note std::optional is safer than null or not null
	 */
	std::optional<Device> device;

	/**
	 * @brief Return the key cache or fill it if its empty
	 */
	const std::vector<Ed25519Key> &get_keys() {
		if (!key_cache.has_value()) {
			key_cache = device->list_ed25519_keys();
		}
		return *key_cache;
	}

	static Pkcs11Module &get() {
		static Pkcs11Module instance;
		return instance;
	}

private:
	Pkcs11Module() = default;
};

#define MODULE Pkcs11Module::get()

static CK_VERSION to_ck_version(const Version &v) {
	CK_VERSION out;
	out.major = v.major;
	out.minor = v.minor;
	return out;
}

// handles 1..32  = public keys, 33..64 = private key stubs
static CK_OBJECT_HANDLE pubkey_handle(lt_ecc_slot_t slot) { return (CK_OBJECT_HANDLE)slot + 1; }
static CK_OBJECT_HANDLE privkey_handle(lt_ecc_slot_t slot) { return (CK_OBJECT_HANDLE)slot + 33; }
static lt_ecc_slot_t handle_to_slot(CK_OBJECT_HANDLE h) { return (lt_ecc_slot_t)((h - 1) % 32); }
static bool is_privkey(CK_OBJECT_HANDLE h) { return h > 32; }

#endif // #ifdef _WIN32
