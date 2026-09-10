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

#include "pkcs11_module.hpp"
#include "key.hpp"
#include "pkcs11t.h"
#include <algorithm>
#include <cstring>
#include <mutex>

/**
 * @todo fix MODULE mutex locks for C_* functions
 */

/**
 * @defgroup pkcs11_api PKCS#11 API
 * @{
 */

// CK = Cryptoki
// CK_RV = CK return value

CK_FUNCTION_LIST empty_pkcs11_2_40_functions = {{0x02, 0x28},
                                                &C_Initialize,
                                                &C_Finalize,
                                                &C_GetInfo,
                                                &C_GetFunctionList,
                                                &C_GetSlotList,
                                                &C_GetSlotInfo,
                                                &C_GetTokenInfo,
                                                &C_GetMechanismList,
                                                &C_GetMechanismInfo,
                                                &C_InitToken,
                                                &C_InitPIN,
                                                &C_SetPIN,
                                                &C_OpenSession,
                                                &C_CloseSession,
                                                &C_CloseAllSessions,
                                                &C_GetSessionInfo,
                                                &C_GetOperationState,
                                                &C_SetOperationState,
                                                &C_Login,
                                                &C_Logout,
                                                &C_CreateObject,
                                                &C_CopyObject,
                                                &C_DestroyObject,
                                                &C_GetObjectSize,
                                                &C_GetAttributeValue,
                                                &C_SetAttributeValue,
                                                &C_FindObjectsInit,
                                                &C_FindObjects,
                                                &C_FindObjectsFinal,
                                                &C_EncryptInit,
                                                &C_Encrypt,
                                                &C_EncryptUpdate,
                                                &C_EncryptFinal,
                                                &C_DecryptInit,
                                                &C_Decrypt,
                                                &C_DecryptUpdate,
                                                &C_DecryptFinal,
                                                &C_DigestInit,
                                                &C_Digest,
                                                &C_DigestUpdate,
                                                &C_DigestKey,
                                                &C_DigestFinal,
                                                &C_SignInit,
                                                &C_Sign,
                                                &C_SignUpdate,
                                                &C_SignFinal,
                                                &C_SignRecoverInit,
                                                &C_SignRecover,
                                                &C_VerifyInit,
                                                &C_Verify,
                                                &C_VerifyUpdate,
                                                &C_VerifyFinal,
                                                &C_VerifyRecoverInit,
                                                &C_VerifyRecover,
                                                &C_DigestEncryptUpdate,
                                                &C_DecryptDigestUpdate,
                                                &C_SignEncryptUpdate,
                                                &C_DecryptVerifyUpdate,
                                                &C_GenerateKey,
                                                &C_GenerateKeyPair,
                                                &C_WrapKey,
                                                &C_UnwrapKey,
                                                &C_DeriveKey,
                                                &C_SeedRandom,
                                                &C_GenerateRandom,
                                                &C_GetFunctionStatus,
                                                &C_CancelFunction,
                                                &C_WaitForSlotEvent};

CK_INTERFACE empty_pkcs11_2_40_interface = {(CK_CHAR *)"PKCS 11", &empty_pkcs11_2_40_functions, 0};

CK_FUNCTION_LIST_3_0 empty_pkcs11_3_1_functions = {{0x03, 0x01},
                                                   &C_Initialize,
                                                   &C_Finalize,
                                                   &C_GetInfo,
                                                   &C_GetFunctionList,
                                                   &C_GetSlotList,
                                                   &C_GetSlotInfo,
                                                   &C_GetTokenInfo,
                                                   &C_GetMechanismList,
                                                   &C_GetMechanismInfo,
                                                   &C_InitToken,
                                                   &C_InitPIN,
                                                   &C_SetPIN,
                                                   &C_OpenSession,
                                                   &C_CloseSession,
                                                   &C_CloseAllSessions,
                                                   &C_GetSessionInfo,
                                                   &C_GetOperationState,
                                                   &C_SetOperationState,
                                                   &C_Login,
                                                   &C_Logout,
                                                   &C_CreateObject,
                                                   &C_CopyObject,
                                                   &C_DestroyObject,
                                                   &C_GetObjectSize,
                                                   &C_GetAttributeValue,
                                                   &C_SetAttributeValue,
                                                   &C_FindObjectsInit,
                                                   &C_FindObjects,
                                                   &C_FindObjectsFinal,
                                                   &C_EncryptInit,
                                                   &C_Encrypt,
                                                   &C_EncryptUpdate,
                                                   &C_EncryptFinal,
                                                   &C_DecryptInit,
                                                   &C_Decrypt,
                                                   &C_DecryptUpdate,
                                                   &C_DecryptFinal,
                                                   &C_DigestInit,
                                                   &C_Digest,
                                                   &C_DigestUpdate,
                                                   &C_DigestKey,
                                                   &C_DigestFinal,
                                                   &C_SignInit,
                                                   &C_Sign,
                                                   &C_SignUpdate,
                                                   &C_SignFinal,
                                                   &C_SignRecoverInit,
                                                   &C_SignRecover,
                                                   &C_VerifyInit,
                                                   &C_Verify,
                                                   &C_VerifyUpdate,
                                                   &C_VerifyFinal,
                                                   &C_VerifyRecoverInit,
                                                   &C_VerifyRecover,
                                                   &C_DigestEncryptUpdate,
                                                   &C_DecryptDigestUpdate,
                                                   &C_SignEncryptUpdate,
                                                   &C_DecryptVerifyUpdate,
                                                   &C_GenerateKey,
                                                   &C_GenerateKeyPair,
                                                   &C_WrapKey,
                                                   &C_UnwrapKey,
                                                   &C_DeriveKey,
                                                   &C_SeedRandom,
                                                   &C_GenerateRandom,
                                                   &C_GetFunctionStatus,
                                                   &C_CancelFunction,
                                                   &C_WaitForSlotEvent,
                                                   &C_GetInterfaceList,
                                                   &C_GetInterface,
                                                   &C_LoginUser,
                                                   &C_SessionCancel,
                                                   &C_MessageEncryptInit,
                                                   &C_EncryptMessage,
                                                   &C_EncryptMessageBegin,
                                                   &C_EncryptMessageNext,
                                                   &C_MessageEncryptFinal,
                                                   &C_MessageDecryptInit,
                                                   &C_DecryptMessage,
                                                   &C_DecryptMessageBegin,
                                                   &C_DecryptMessageNext,
                                                   &C_MessageDecryptFinal,
                                                   &C_MessageSignInit,
                                                   &C_SignMessage,
                                                   &C_SignMessageBegin,
                                                   &C_SignMessageNext,
                                                   &C_MessageSignFinal,
                                                   &C_MessageVerifyInit,
                                                   &C_VerifyMessage,
                                                   &C_VerifyMessageBegin,
                                                   &C_VerifyMessageNext,
                                                   &C_MessageVerifyFinal};

CK_INTERFACE empty_pkcs11_3_1_interface = {(CK_CHAR *)"PKCS 11", &empty_pkcs11_3_1_functions, 0};

/**
 * @fn CK_RV C_Initialize(CK_VOID_PTR pInitArgs)
 * @brief Initializes the hardware device with TROPIC01 chip on it and starts secure session with the default key slot.
 *
 * @param[in] pInitArgs Arguments for the pkcs library. Tells the pkcs library how to use multithreading. Currently being ignored.
 * @return CKR_OK if the connection with TROPIC01 chip was established properly
 * @return CKR_DEVICE_ERROR if the connection with the TROPIC01 chip could not be established or could not start secure session with the key slot on the chip
 * @return CKR_CRYPTOKI_ALREADY_INITIALIZED if the connection was already established
 *
 * @see Device::init()
 * @see Device::start_secure_session()
 */
CK_DEFINE_FUNCTION(CK_RV, C_Initialize)(CK_VOID_PTR pInitArgs) {
	UNUSED(pInitArgs);

	std::lock_guard<std::mutex> lock(MODULE.mtx);
	if (MODULE.initialized) return CKR_CRYPTOKI_ALREADY_INITIALIZED;

	MODULE.device.emplace(); // construct Device in-place inside the optional
	if (!MODULE.device->init()) {
		MODULE.device.reset();
		return CKR_DEVICE_ERROR;
	}
	if (!MODULE.device->start_secure_session()) {
		MODULE.device.reset();
		return CKR_DEVICE_ERROR;
	}

	MODULE.initialized = true;
	return CKR_OK;
}

/**
 * @brief Last function called, indicates the application is done with Cryptoki library.
 * Deinitializes the HW device and clears the state of the optional MODULE.device
 *
 * @param[in] pReserved reserved for future uses, currently has no use
 * @return CKR_OK after successfully deinitializing the device with the TROPIC01 chip
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is no longer initialized
 *
 * @see Device::close()
 * @see Device::reset()
 */
CK_DEFINE_FUNCTION(CK_RV, C_Finalize)(CK_VOID_PTR pReserved) {
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;

	MODULE.device->close();
	MODULE.device.reset();
	MODULE.key_cache.reset();
	MODULE.initialized = false;

	return CKR_OK;
}

/**
 * @brief Returns general info about the Cryptoki (PKCS_11)
 *
 * @param[out] pInfo pointer to the location that receives the info
 * @return CKR_OK
 * @return CKR_ARGUMENTS_BAD
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetInfo)(CK_INFO_PTR pInfo) {
	if (!pInfo) return CKR_ARGUMENTS_BAD;

	pInfo->cryptokiVersion = {0x02, 0x28}; // Cryptoki implemented version 2.40

	// PKCS#11 string are NOT null terminated
	memset(pInfo->manufacturerID, ' ', sizeof(pInfo->manufacturerID));
	memset(pInfo->libraryDescription, ' ', sizeof(pInfo->libraryDescription));

	const char *mfr = "Tropic Square";
	const char *libinfo = "Tropikey PKCS#11 Module";
	memcpy(pInfo->manufacturerID, mfr, strlen(mfr));
	memcpy(pInfo->libraryDescription, libinfo, strlen(libinfo));

	pInfo->flags = 0;
	pInfo->libraryVersion = {0x00, 0x01}; // Tropikey version

	return CKR_OK;
}

/**
 * @brief Sets the pointer to the library's list of functions.
 *
 * @param[out] ppFunctionList pointer to the list that will receive the functions pointer
 * @return CKR_ARGUMENTS_BAD
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetFunctionList)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList) {
	if (NULL == ppFunctionList) return CKR_ARGUMENTS_BAD;

	*ppFunctionList = &empty_pkcs11_2_40_functions;

	return CKR_OK;
}

/**
 * @brief Obtains available slots of the system.
 *
 * In the case of the Tropikey implementation this will always be one, regardless
 * if the physical device (board with TROPIC01 chip) is connected or not. Currently only the slot ID
 * 0 is used.
 *
 * @note A slot represents a physical device interface. This means the slot is the
 * key reader.
 * @see https://thalesdocs.com/gphsm/ptk/5.9/docs/Content/PTK-C_Program/intro_PKCS11.htm
 *
 * @param[in] tokenPresent indicates whether the list obtained includes only those slots with a
 * token present (CK_TRUE), or all slots (CK_FALSE)
 * @param[out] pSlotList if NULL_PTR then only return number of slots in pulCount, otherwise the list
 * of slots is set in the space pointed to by this parameter
 * @param[out] pulCount points to the location that receives the number of slots
 * @return CKR_OK
 * @return CKR_BUFFER_TOO_SMALL if the device is not initialized
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetSlotList)(CK_BBOOL tokenPresent,
                                         CK_SLOT_ID_PTR pSlotList,
                                         CK_ULONG_PTR pulCount) {
	if (tokenPresent && !MODULE.initialized) {
		*pulCount = 0;
		return CKR_OK;
	}

	if (!pSlotList) {
		*pulCount = 1;
		return CKR_OK;
	}

	if (*pulCount < 1) return CKR_BUFFER_TOO_SMALL;

	pSlotList[0] = TROPIKEYSLOTID; // the only slot ID
	*pulCount = 1;
	return CKR_OK;
}

/**
 * @brief Obtains information about a slot in the system
 *
 * Returns HW and FW information of the TROPIC01 USB Devkit alongside description and other useful
 * information.
 *
 * @note A slot represents a physical device interface. This means the slot is the
 * key reader.
 * @see https://thalesdocs.com/gphsm/ptk/5.9/docs/Content/PTK-C_Program/intro_PKCS11.htm
 *
 * @see Device::get_hw_version()
 * @see Device::get_fw_version()
 *
 * @param[in] slotID ID of the slot
 * @param[out] pInfo points to the location that receives the information
 * @return CKR_OK
 * @return CKR_SLOT_ID_INVALID if the slot ID is not TROPIKEYSLOTID
 * @return CKR_ARGUMENTS_BAD if pInfo is not a valid pointer
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetSlotInfo)(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo) {
	if (slotID != TROPIKEYSLOTID) return CKR_SLOT_ID_INVALID; // only one slot available - the USB devkit
	if (!pInfo) return CKR_ARGUMENTS_BAD;

	// PKCS#11 string are NOT null terminated
	memset(pInfo->slotDescription, ' ', sizeof(pInfo->slotDescription));
	memset(pInfo->manufacturerID, ' ', sizeof(pInfo->manufacturerID));

	const char *desc = "TROPIC01 USB Devkit";
	const char *mfr = "Tropic Square";
	memcpy(pInfo->slotDescription, desc, strlen(desc));
	memcpy(pInfo->manufacturerID, mfr, strlen(mfr));

	pInfo->flags = CKF_HW_SLOT | (MODULE.initialized ? CKF_TOKEN_PRESENT : 0);

	if (MODULE.initialized) {
		pInfo->hardwareVersion = to_ck_version(MODULE.device->get_hw_version());
		pInfo->firmwareVersion = to_ck_version(MODULE.device->get_fw_version());
	} else {
		pInfo->hardwareVersion = {0, 0};
		pInfo->firmwareVersion = {0, 0};
	}

	return CKR_OK;
}

/**
 * @brief Obtains information about a token in the system.
 *
 * Returns information about the token of the USB devkit.
 *
 * @note A token is a device that stores objects and can perform cryptographic functions
 * @note Objects can be one of the following:
 * 1. Data objects, which are defined by an application
 * 2. Certificate objects, which are digital certificates such as X.509
 * 3. Key objects, which can be public, private or secret cryptographic keys
 * 4. Vendor-defined objects
 * Generally they are public key and private key objects
 *
 * @param[in] slotID
 * @param[out] pInfo
 * @return CKR_SLOT_ID_INVALID if the slotID is not equal to TROPIKEYSLOTID
 * @return CKR_ARGUMENTS_BAD if pInfo is not a valid pointer
 * @return CKR_TOKEN_NOT_PRESENT if the device was not initialized
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetTokenInfo)(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo) {
	if (slotID != TROPIKEYSLOTID) return CKR_SLOT_ID_INVALID;
	if (!pInfo) return CKR_ARGUMENTS_BAD;
	if (!MODULE.initialized) return CKR_TOKEN_NOT_PRESENT;

	memset(pInfo->label, ' ', sizeof(pInfo->label));
	memset(pInfo->manufacturerID, ' ', sizeof(pInfo->manufacturerID));
	memset(pInfo->model, ' ', sizeof(pInfo->model));
	memset(pInfo->serialNumber, ' ', sizeof(pInfo->serialNumber));
	memset(pInfo->utcTime, ' ', sizeof(pInfo->utcTime));

	const char *label = "Tropikey";
	const char *mfr = "Tropic Square";
	const char *model = "TROPIC01";
	memcpy(pInfo->label, label, strlen(label));
	memcpy(pInfo->manufacturerID, mfr, strlen(mfr));
	memcpy(pInfo->model, model, strlen(model));

	pInfo->flags = CKF_TOKEN_INITIALIZED |             // token is ready to use
	               CKF_PROTECTED_AUTHENTICATION_PATH | // no PIN needed (hardware handles auth)
	               CKF_HW_SLOT;                        // it's a real hardware token

	// TROPIC01 session model
	pInfo->ulMaxSessionCount = 1;
	pInfo->ulSessionCount = MODULE.initialized ? 1 : 0;
	pInfo->ulMaxRwSessionCount = 1;
	pInfo->ulRwSessionCount = 0;

	// no PIN on this token
	pInfo->ulMaxPinLen = 0;
	pInfo->ulMinPinLen = 0;

	// memory info not available from the chip
	pInfo->ulTotalPublicMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePublicMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulTotalPrivateMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePrivateMemory = CK_UNAVAILABLE_INFORMATION;

	// TODO serial number from chip_id
	if (MODULE.initialized) {
		pInfo->hardwareVersion = to_ck_version(MODULE.device->get_hw_version());
		pInfo->firmwareVersion = to_ck_version(MODULE.device->get_fw_version());
	} else {
		pInfo->hardwareVersion = {0, 0};
		pInfo->firmwareVersion = {0, 0};
	}
	return CKR_OK;
}

/**
 * @brief Obtains the list of mechanisms the token supports.
 *
 * In the case of the Tropikey it only supports one for now: ED25519 mechanism.
 * @todo implement more mechanisms
 *
 * @param[in] slotID ID of the token's slot
 * @param[out] pMechanismList in case of NULL_PTR then only returns number of mechanisms in pulCount,
 * otherwise store the mechanism type at the location pointed to by pMechanismList.
 * @param[out] pulCount points to the location that receives the number of mechanisms
 * @return CKR_SLOT_ID_INVALID if slot ID is not TROPIKEYSLOTID
 * @return CKR_ARGUMENTS_BAD if the pulCount is not a valid pointer
 * @return CKR_BUFFER_TOO_SMALL if the mechanism list is not a valid pointer
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetMechanismList)(CK_SLOT_ID slotID,
                                              CK_MECHANISM_TYPE_PTR pMechanismList,
                                              CK_ULONG_PTR pulCount) {
	if (slotID != TROPIKEYSLOTID) return CKR_SLOT_ID_INVALID;
	if (!pulCount) return CKR_ARGUMENTS_BAD;

	// only Ed25519 signing for now
	if (!pMechanismList) {
		*pulCount = 1;
		return CKR_OK;
	}

	if (*pulCount < 1) return CKR_BUFFER_TOO_SMALL;

	pMechanismList[0] = CKM_EDDSA;
	*pulCount = 1;
	return CKR_OK;
}

/**
 * @brief Obtains information about a mechanism of the system.
 *
 * Notably sets the ulMinKeySize and ulMaxKeySize for Ed25519.
 *
 * @param[in] slotID ID of the slot the mechanism should be retrieved for
 * @param[in] type type of the mechanism
 * @param[out] pInfo pointer to space where the info will be stored
 * @return CKR_SLOT_ID_INVALID if slotID is not TROPIKEYSLOTID
 * @return CKR_ARGUMENTS_BAD if pInfo is not a valid pointer
 * @return CKR_MECHANISM_INVALID if mechanism is not Ed25519 (for now the only one used)
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetMechanismInfo)(CK_SLOT_ID slotID,
                                              CK_MECHANISM_TYPE type,
                                              CK_MECHANISM_INFO_PTR pInfo) {
	if (slotID != TROPIKEYSLOTID) return CKR_SLOT_ID_INVALID;
	if (!pInfo) return CKR_ARGUMENTS_BAD;

	if (type != CKM_EDDSA) return CKR_MECHANISM_INVALID;

	pInfo->ulMinKeySize = 255; // Ed25519 key size in bits
	pInfo->ulMaxKeySize = 255;
	pInfo->flags = CKF_SIGN | CKF_HW;

	return CKR_OK;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_InitToken)(CK_SLOT_ID slotID,
                                       CK_UTF8CHAR_PTR pPin,
                                       CK_ULONG ulPinLen,
                                       CK_UTF8CHAR_PTR pLabel) {
	UNUSED(slotID);
	UNUSED(pPin);
	UNUSED(ulPinLen);
	UNUSED(pLabel);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV,
                   C_InitPIN)(CK_SESSION_HANDLE hSession, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen) {
	UNUSED(hSession);
	UNUSED(pPin);
	UNUSED(ulPinLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SetPIN)(CK_SESSION_HANDLE hSession,
                                    CK_UTF8CHAR_PTR pOldPin,
                                    CK_ULONG ulOldLen,
                                    CK_UTF8CHAR_PTR pNewPin,
                                    CK_ULONG ulNewLen) {
	UNUSED(hSession);
	UNUSED(pOldPin);
	UNUSED(ulOldLen);
	UNUSED(pNewPin);
	UNUSED(ulNewLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @brief Opens session between application and a token in particular slot. The slot must contain
 * the key in it.
 *
 * @note only supporting one session at the time for now
 * @note session is currently only open for single token
 *
 * @param[in] slotID ID of the slot
 * @param[in] flags indicates type of session
 * @param[in] pApplication application-defined pointer to be passed to the notification callback
 * @param[in] Notify address of the notification callback function
 * @param[out] phSession points to the location that receives the handle for the new session
 *
 * @return CKR_SLOT_ID_INVALID if the slot ID is not the TROPIKEYSLOTID
 * @return CKR_ARGUMENTS_BAD if the phSession is not a valid pointer
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_SESSION_PARALLEL_NOT_SUPPORTED if flags do not contain CKF_SERIAL_SESSION
 * @return CKR_SESSION_COUNT if the session is already open
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_OpenSession)(CK_SLOT_ID slotID,
                                         CK_FLAGS flags,
                                         CK_VOID_PTR pApplication,
                                         CK_NOTIFY Notify,
                                         CK_SESSION_HANDLE_PTR phSession) {
	UNUSED(pApplication);
	UNUSED(Notify);

	if (slotID != TROPIKEYSLOTID) return CKR_SLOT_ID_INVALID;
	if (!phSession) return CKR_ARGUMENTS_BAD;
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;

	// PKCS#11 requires CKF_SERIAL_SESSION to always be set
	if (!(flags & CKF_SERIAL_SESSION)) return CKR_SESSION_PARALLEL_NOT_SUPPORTED;

	std::lock_guard<std::mutex> lock(MODULE.mtx);

	if (MODULE.session_open) return CKR_SESSION_COUNT; // only supporting one session

	MODULE.session_open = true;
	*phSession = TROPIKEYSESSIONID; // only one session - fixed value

	return CKR_OK;
}

/**
 * @brief Close an open session on a token, once the session is closed the app cannot pass any
 * cryptographic requests to the token.
 *
 * @note locks or waits at the lock(Module.mtx) function call until it is unlocked
 *
 * @param[in] hSession session's handle (set in C_OpenSession)
 * @return CKR_SESSION_HANDLE_INVALID if the session handle is not the TROPIKEYSESSIONID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device was not initialized
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_CloseSession)(CK_SESSION_HANDLE hSession) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;

	std::lock_guard<std::mutex> lock(MODULE.mtx);
	MODULE.session_open = false;
	return CKR_OK;
}

/**
 * @brief Close an open session on all tokens.
 *
 * Since Tropikey uses only one slot and token, close that one.
 *
 * @note locks or waits at the lock(Module.mtx) function call until it is unlocked
 *
 * @param[in] slotID ID of the slot to close the session on
 * @return CKR_SLOT_ID_INVALID if the slotID is not the TROPIKEYSLOTID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device was not initialized
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_CloseAllSessions)(CK_SLOT_ID slotID) {
	if (slotID != TROPIKEYSLOTID) return CKR_SLOT_ID_INVALID;
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;

	std::lock_guard<std::mutex> lock(MODULE.mtx);
	MODULE.session_open = false;
	return CKR_OK;
}

/**
 * @brief Obtains session information.
 *
 * @param[in] hSession session's handle (set in C_OpenSession)
 * @param[out] pInfo pointer to space where the information will be written to
 * @return CKR_SESSION_HANDLE_INVALID if the session handle is not the TROPIKEYSESSIONID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device was not initialized
 * @return CKR_ARGUMENTS_BAD if the pInfo is not a valid pointer
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetSessionInfo)(CK_SESSION_HANDLE hSession, CK_SESSION_INFO_PTR pInfo) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;
	if (!pInfo) return CKR_ARGUMENTS_BAD;

	pInfo->slotID = 0;
	pInfo->state = CKS_RO_PUBLIC_SESSION; // read-only, no PIN required
	pInfo->flags = CKF_SERIAL_SESSION;
	pInfo->ulDeviceError = 0;

	return CKR_OK;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetOperationState)(CK_SESSION_HANDLE hSession,
                                               CK_BYTE_PTR pOperationState,
                                               CK_ULONG_PTR pulOperationStateLen) {
	UNUSED(hSession);
	UNUSED(pOperationState);
	UNUSED(pulOperationStateLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SetOperationState)(CK_SESSION_HANDLE hSession,
                                               CK_BYTE_PTR pOperationState,
                                               CK_ULONG ulOperationStateLen,
                                               CK_OBJECT_HANDLE hEncryptionKey,
                                               CK_OBJECT_HANDLE hAuthenticationKey) {
	UNUSED(hSession);
	UNUSED(pOperationState);
	UNUSED(ulOperationStateLen);
	UNUSED(hEncryptionKey);
	UNUSED(hAuthenticationKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @brief Authenticates the user
 *
 * Tropikey authentication is handled in C_Initilaize in start_secure_session. No PIN-based login.
 * From PKCS#11's perspective we're always logged in.
 *
 * @param[in] hSession session's handle (set in C_OpenSession)
 * @param[in] userType user type (either CKU_USER, CKU_SO, or CKU_CONTEXT_SPECIFIC)
 * @param[in] pPin pointer to the user's PIN
 * @param[in] ulPinLen is the length of the user's PIN
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_USER_TYPE_INVALID if the userType is not CKU_USER
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_Login)(CK_SESSION_HANDLE hSession,
                                   CK_USER_TYPE userType,
                                   CK_UTF8CHAR_PTR pPin,
                                   CK_ULONG ulPinLen) {
	UNUSED(pPin);
	UNUSED(ulPinLen);

	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;

	// only CKU_USER, reject SO (security officer) login
	if (userType != CKU_USER) return CKR_USER_TYPE_INVALID;

	return CKR_OK;
}

/**
 * @brief Logs out the user from a token.
 *
 * @note basically an empty function since we don't really handle login anyway
 *
 * @param[in] hSession session's handle (set in C_OpenSession)
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_Logout)(CK_SESSION_HANDLE hSession) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;

	return CKR_OK;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_CreateObject)(CK_SESSION_HANDLE hSession,
                                          CK_ATTRIBUTE_PTR pTemplate,
                                          CK_ULONG ulCount,
                                          CK_OBJECT_HANDLE_PTR phObject) {
	UNUSED(hSession);
	UNUSED(pTemplate);
	UNUSED(ulCount);
	UNUSED(phObject);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_CopyObject)(CK_SESSION_HANDLE hSession,
                                        CK_OBJECT_HANDLE hObject,
                                        CK_ATTRIBUTE_PTR pTemplate,
                                        CK_ULONG ulCount,
                                        CK_OBJECT_HANDLE_PTR phNewObject) {
	UNUSED(hSession);
	UNUSED(hObject);
	UNUSED(pTemplate);
	UNUSED(ulCount);
	UNUSED(phNewObject);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DestroyObject)(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject) {
	UNUSED(hSession);
	UNUSED(hObject);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetObjectSize)(CK_SESSION_HANDLE hSession,
                                           CK_OBJECT_HANDLE hObject,
                                           CK_ULONG_PTR pulSize) {
	UNUSED(hSession);
	UNUSED(hObject);
	UNUSED(pulSize);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @brief Obtains the value of one or more attributes of an object.
 *
 * Attribute is a "member" or a "value" of an object. An object consists of a set of attributes,
 * each with its given value. pTemplate is filled with attributes, it describes what is required and
 * what to fill and then is filled with the information
 *
 * @see https://docs.oasis-open.org/pkcs11/pkcs11-spec/v3.1/csd01/pkcs11-spec-v3.1-csd01.html#_Toc98177049
 *
 * @param[in] hSession session's handle (set in C_OpenSession)
 * @param[in] hObject object's handle
 * @param[out] pTemplate points to a template that specifies which attribute values are to be
 * obtained and receives the attribute values
 * @param [in] ulCount number of attributes in the pTemplate
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_ARGUMENTS_BAD if pTemplate is not a valid pointer
 * @return CKR_OBJECT_HANDLE_INVALID if the requested object (in this case the key) was not found on
 * the device
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetAttributeValue)(CK_SESSION_HANDLE hSession,
                                               CK_OBJECT_HANDLE hObject,
                                               CK_ATTRIBUTE_PTR pTemplate,
                                               CK_ULONG ulCount) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;
	if (!pTemplate) return CKR_ARGUMENTS_BAD;

	lt_ecc_slot_t slot = handle_to_slot(hObject); // decode slot from handle
	bool priv = is_privkey(hObject);              // decode key type from handle

	auto key_it = std::find_if(MODULE.get_keys().begin(),
	                           MODULE.get_keys().end(),
	                           [slot](const Ed25519Key &k) { return k.get_slot() == slot; });

	if (key_it == MODULE.get_keys().end()) return CKR_OBJECT_HANDLE_INVALID; // key not found

	const Ed25519Key &key = *key_it; // reference, not optional - use key.data()

	// helper lambda function: fills memory space for given attribute and writes data
	auto fill = [](CK_ATTRIBUTE_PTR attr, const void *data, CK_ULONG len) -> CK_RV {
		if (!attr->pValue) {
			attr->ulValueLen = len;
			return CKR_OK;
		}
		if (attr->ulValueLen < len) {
			attr->ulValueLen = CK_UNAVAILABLE_INFORMATION;
			return CKR_BUFFER_TOO_SMALL;
		}
		memcpy(attr->pValue, data, len);
		attr->ulValueLen = len;
		return CKR_OK;
	};

	CK_RV result = CKR_OK;

	for (CK_ULONG i = 0; i < ulCount; i++) {
		CK_RV rv = CKR_OK;

		switch (pTemplate[i].type) {
		case CKA_CLASS: {
			CK_OBJECT_CLASS cls = priv ? CKO_PRIVATE_KEY : CKO_PUBLIC_KEY;
			rv = fill(&pTemplate[i], &cls, sizeof(cls));
			break;
		}
		case CKA_KEY_TYPE: {
			CK_KEY_TYPE kt = CKK_EC_EDWARDS;
			rv = fill(&pTemplate[i], &kt, sizeof(kt));
			break;
		}
		case CKA_ID: {
			uint8_t id = (uint8_t)slot;
			rv = fill(&pTemplate[i], &id, sizeof(id));
			break;
		}
		case CKA_SIGN: {
			CK_BBOOL val = priv ? CK_TRUE : CK_FALSE;
			rv = fill(&pTemplate[i], &val, sizeof(val));
			break;
		}
		case CKA_VERIFY: {
			CK_BBOOL val = priv ? CK_FALSE : CK_TRUE;
			rv = fill(&pTemplate[i], &val, sizeof(val));
			break;
		}
		case CKA_EC_POINT: {
			rv = fill(&pTemplate[i], key.data(), ED25519_KEY_LEN);
			break;
		}
		case CKA_EC_PARAMS: {
			static const uint8_t oid[] = {0x06, 0x03, 0x2B, 0x65, 0x70}; // 1.3.101.112 ... OID for Ed25519
			rv = fill(&pTemplate[i], oid, sizeof(oid));
			break;
		}
		case CKA_PRIVATE: {
			CK_BBOOL val = priv ? CK_TRUE : CK_FALSE;
			rv = fill(&pTemplate[i], &val, sizeof(val));
			break;
		}
		case CKA_TOKEN: {
			CK_BBOOL val = CK_TRUE;
			rv = fill(&pTemplate[i], &val, sizeof(val));
			break;
		}
		case CKA_LABEL: {
			std::string label = "TROPIC01 Ed25519 key slot " + std::to_string((int)slot);
			rv = fill(&pTemplate[i], label.c_str(), label.size());
			break;
		}
		case CKA_SENSITIVE: {
			CK_BBOOL val = priv ? CK_TRUE : CK_FALSE;
			rv = fill(&pTemplate[i], &val, sizeof(val));
			break;
		}
		case CKA_EXTRACTABLE: {
			CK_BBOOL val = CK_FALSE;
			rv = fill(&pTemplate[i], &val, sizeof(val));
			break;
		}
		case CKA_ALWAYS_AUTHENTICATE: {
			CK_BBOOL val = CK_FALSE;
			rv = fill(&pTemplate[i], &val, sizeof(val));
			break;
		}
		default:
			std::cerr << "C_GetAttributeValue: unknown attribute 0x" << std::hex
			          << pTemplate[i].type << "\n";
			pTemplate[i].ulValueLen = CK_UNAVAILABLE_INFORMATION;
			rv = CKR_ATTRIBUTE_TYPE_INVALID;
			break;
		}

		if (rv != CKR_OK) result = rv;
	}

	return result;
}

CK_DEFINE_FUNCTION(CK_RV, C_SetAttributeValue)(CK_SESSION_HANDLE hSession,
                                               CK_OBJECT_HANDLE hObject,
                                               CK_ATTRIBUTE_PTR pTemplate,
                                               CK_ULONG ulCount) {
	UNUSED(hSession);
	UNUSED(hObject);
	UNUSED(pTemplate);
	UNUSED(ulCount);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @brief Initializes a search for token and session objects that match a template.
 *
 * In the case of Tropikey application the keys are the objects.
 *
 * @param[in] hSession
 * @param[in] pTemplate
 * @param[in] ulCount
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_OPERATION_ACTIVE if the operation is already active on given hSession
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_FindObjectsInit)(CK_SESSION_HANDLE hSession,
                                             CK_ATTRIBUTE_PTR pTemplate,
                                             CK_ULONG ulCount) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;

	std::lock_guard<std::mutex> lock(MODULE.mtx);

	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;
	if (MODULE.find_active) return CKR_OPERATION_ACTIVE;

	CK_OBJECT_CLASS wanted_class = (CK_OBJECT_CLASS)-1;
	for (CK_ULONG i = 0; i < ulCount; i++) {
		if (pTemplate[i].type == CKA_CLASS && pTemplate[i].pValue)
			wanted_class = *(CK_OBJECT_CLASS *)pTemplate[i].pValue;
	}

	MODULE.found_objects.clear();

	for (const auto &key : MODULE.get_keys()) { // uses cache
		if (wanted_class == (CK_OBJECT_CLASS)-1 || wanted_class == CKO_PUBLIC_KEY)
			MODULE.found_objects.push_back(pubkey_handle(key.get_slot()));
		if (wanted_class == (CK_OBJECT_CLASS)-1 || wanted_class == CKO_PRIVATE_KEY)
			MODULE.found_objects.push_back(privkey_handle(key.get_slot()));
	}

	MODULE.find_index = 0;
	MODULE.find_active = true;
	return CKR_OK;
}

/**
 * @brief Continues search for token and session objects that match a template, obtaining additional
 * object handles.
 *
 * In case of Tropikey application the phObjects is filled with any found objects in cache during
 * the C_FindObjectsInit.
 *
 * @param[in] hSession session's handle
 * @param[out] phObject points to the location that receives the array of additional object (usually key) handles
 * @param[in] ulMaxObjectCount maximum number of object handles to be returned
 * @param[out] pulObjectCount points to the location that receives the actual number of object
 * handles returned
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_ARGUMENTS_BAD if either phObject or pulObjectCount is an invalid pointer
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_OPERATION_ACTIVE if the operation is already active on given hSession
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_FindObjects)(CK_SESSION_HANDLE hSession,
                                         CK_OBJECT_HANDLE_PTR phObject,
                                         CK_ULONG ulMaxObjectCount,
                                         CK_ULONG_PTR pulObjectCount) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!phObject || !pulObjectCount) return CKR_ARGUMENTS_BAD;

	std::lock_guard<std::mutex> lock(MODULE.mtx);

	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;
	if (!MODULE.find_active) return CKR_OPERATION_NOT_INITIALIZED;

	CK_ULONG count = 0;
	while (count < ulMaxObjectCount && MODULE.find_index < MODULE.found_objects.size()) {
		phObject[count++] = MODULE.found_objects[MODULE.find_index++];
	}

	*pulObjectCount = count;
	return CKR_OK;
}

/**
 * @brief Finalizes the search for objects.
 *
 * In case of Tropikey application the found_objects cache is cleared.
 *
 * @param[in] hSession session's handle
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_FindObjectsFinal)(CK_SESSION_HANDLE hSession) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;

	std::lock_guard<std::mutex> lock(MODULE.mtx);

	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;
	if (!MODULE.find_active) return CKR_OPERATION_NOT_INITIALIZED;

	MODULE.find_active = false;
	MODULE.found_objects.clear();
	return CKR_OK;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_EncryptInit)(CK_SESSION_HANDLE hSession,
                                         CK_MECHANISM_PTR pMechanism,
                                         CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_Encrypt)(CK_SESSION_HANDLE hSession,
                                     CK_BYTE_PTR pData,
                                     CK_ULONG ulDataLen,
                                     CK_BYTE_PTR pEncryptedData,
                                     CK_ULONG_PTR pulEncryptedDataLen) {
	UNUSED(hSession);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pEncryptedData);
	UNUSED(pulEncryptedDataLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_EncryptUpdate)(CK_SESSION_HANDLE hSession,
                                           CK_BYTE_PTR pPart,
                                           CK_ULONG ulPartLen,
                                           CK_BYTE_PTR pEncryptedPart,
                                           CK_ULONG_PTR pulEncryptedPartLen) {
	UNUSED(hSession);
	UNUSED(pPart);
	UNUSED(ulPartLen);
	UNUSED(pEncryptedPart);
	UNUSED(pulEncryptedPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_EncryptFinal)(CK_SESSION_HANDLE hSession,
                                          CK_BYTE_PTR pLastEncryptedPart,
                                          CK_ULONG_PTR pulLastEncryptedPartLen) {
	UNUSED(hSession);
	UNUSED(pLastEncryptedPart);
	UNUSED(pulLastEncryptedPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptInit)(CK_SESSION_HANDLE hSession,
                                         CK_MECHANISM_PTR pMechanism,
                                         CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_Decrypt)(CK_SESSION_HANDLE hSession,
                                     CK_BYTE_PTR pEncryptedData,
                                     CK_ULONG ulEncryptedDataLen,
                                     CK_BYTE_PTR pData,
                                     CK_ULONG_PTR pulDataLen) {
	UNUSED(hSession);
	UNUSED(pEncryptedData);
	UNUSED(ulEncryptedDataLen);
	UNUSED(pData);
	UNUSED(pulDataLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptUpdate)(CK_SESSION_HANDLE hSession,
                                           CK_BYTE_PTR pEncryptedPart,
                                           CK_ULONG ulEncryptedPartLen,
                                           CK_BYTE_PTR pPart,
                                           CK_ULONG_PTR pulPartLen) {
	UNUSED(hSession);
	UNUSED(pEncryptedPart);
	UNUSED(ulEncryptedPartLen);
	UNUSED(pPart);
	UNUSED(pulPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptFinal)(CK_SESSION_HANDLE hSession,
                                          CK_BYTE_PTR pLastPart,
                                          CK_ULONG_PTR pulLastPartLen) {
	UNUSED(hSession);
	UNUSED(pLastPart);
	UNUSED(pulLastPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DigestInit)(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism) {
	UNUSED(hSession);
	UNUSED(pMechanism);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_Digest)(CK_SESSION_HANDLE hSession,
                                    CK_BYTE_PTR pData,
                                    CK_ULONG ulDataLen,
                                    CK_BYTE_PTR pDigest,
                                    CK_ULONG_PTR pulDigestLen) {
	UNUSED(hSession);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pDigest);
	UNUSED(pulDigestLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DigestUpdate)(CK_SESSION_HANDLE hSession,
                                          CK_BYTE_PTR pPart,
                                          CK_ULONG ulPartLen) {
	UNUSED(hSession);
	UNUSED(pPart);
	UNUSED(ulPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DigestKey)(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DigestFinal)(CK_SESSION_HANDLE hSession,
                                         CK_BYTE_PTR pDigest,
                                         CK_ULONG_PTR pulDigestLen) {
	UNUSED(hSession);
	UNUSED(pDigest);
	UNUSED(pulDigestLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @brief Initializes the signing operation, where the signature is an appendix to the data.
 *
 * In the case of Tropikey application just mark signing state as active and assign sign_key_slot to
 * the slot translated from hKey.
 *
 * @todo map the CKM_EDDSA definition to libtropic TR01_CURVE_ED25519 (and other supported
 * algorithms)
 *
 * @param[in] hSession session's handle
 * @param[in] pMechanism points to the signature mechanism
 * @param[in] hKey handle of the signature key (object handle)
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_ARGUMENTS_BAD if either pMechanism is an invalid pointer
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_OPERATION_ACTIVE if the operation is already active on given hSession
 * @return CKR_MECHANISM_INVALID if the mechanism (signing algorithm) is not supported (currently
 * only EDDSA is supported)
 * @return CKR_KEY_TYPE_INCONSISTENT if the key handle is not a private key object handle
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignInit)(CK_SESSION_HANDLE hSession,
                                      CK_MECHANISM_PTR pMechanism,
                                      CK_OBJECT_HANDLE hKey) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!pMechanism) return CKR_ARGUMENTS_BAD;

	std::lock_guard<std::mutex> lock(MODULE.mtx);

	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;
	if (MODULE.sign_active) return CKR_OPERATION_ACTIVE;

	if (pMechanism->mechanism != CKM_EDDSA) return CKR_MECHANISM_INVALID;

	if (!is_privkey(hKey)) return CKR_KEY_TYPE_INCONSISTENT;

	MODULE.sign_key_slot = handle_to_slot(hKey);
	MODULE.sign_active = true;
	return CKR_OK;
}

/**
 * @brief Signs the challenge using the TROPIC01 USB Devkit.
 *
 * @param[in] hSession session's handle
 * @param[in] pData pointer to the data
 * @param[in] ulDataLen length of the data
 * @param[out] pSignature points to the location that receives the signature
 * @param[out] pulSignatureLen points to the location that will hold length of the generated
 * signature
 * @return CKR_SESSION_HANDLE_INVALID if the hSession is not TROPIKEYSESSIONID
 * @return CKR_ARGUMENTS_BAD if either pData or pulSignatureLen is an invalid pointer
 * @return CKR_CRYPTOKI_NOT_INITIALIZED if the device is not initialized
 * @return CKR_OPERATION_NOT_INITIALIZED if the operation is already active on given hSession
 * @return CKR_BUFFER_TOO_SMALL if the buffer to hold the signature was too small
 * @return CKR_DEVICE_ERROR if the signing on the TROPIC01 USB Devkit was not successful
 * @return CKR_OK
 */
CK_DEFINE_FUNCTION(CK_RV, C_Sign)(CK_SESSION_HANDLE hSession,
                                  CK_BYTE_PTR pData,
                                  CK_ULONG ulDataLen,
                                  CK_BYTE_PTR pSignature,
                                  CK_ULONG_PTR pulSignatureLen) {
	if (hSession != TROPIKEYSESSIONID) return CKR_SESSION_HANDLE_INVALID;
	if (!pData || !pulSignatureLen) return CKR_ARGUMENTS_BAD;

	std::lock_guard<std::mutex> lock(MODULE.mtx);

	if (!MODULE.initialized) return CKR_CRYPTOKI_NOT_INITIALIZED;
	if (!MODULE.sign_active) return CKR_OPERATION_NOT_INITIALIZED;

	// two-pass: first call with pSignature=NULL returns required length
	if (!pSignature) {
		*pulSignatureLen = 64;
		return CKR_OK;
	}

	if (*pulSignatureLen < 64) {
		*pulSignatureLen = 64;
		return CKR_BUFFER_TOO_SMALL;
	}

	std::vector<uint8_t> challenge(pData, pData + ulDataLen);
	std::vector<uint8_t> signature;

	if (!MODULE.device->sign_ed25519_challenge(MODULE.sign_key_slot, challenge, signature)) {
		MODULE.sign_active = false;
		return CKR_DEVICE_ERROR;
	}

	memcpy(pSignature, signature.data(), 64);
	*pulSignatureLen = 64;
	MODULE.sign_active = false; // operation complete, reset state
	return CKR_OK;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignUpdate)(CK_SESSION_HANDLE hSession,
                                        CK_BYTE_PTR pPart,
                                        CK_ULONG ulPartLen) {
	UNUSED(hSession);
	UNUSED(pPart);
	UNUSED(ulPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignFinal)(CK_SESSION_HANDLE hSession,
                                       CK_BYTE_PTR pSignature,
                                       CK_ULONG_PTR pulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pSignature);
	UNUSED(pulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignRecoverInit)(CK_SESSION_HANDLE hSession,
                                             CK_MECHANISM_PTR pMechanism,
                                             CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignRecover)(CK_SESSION_HANDLE hSession,
                                         CK_BYTE_PTR pData,
                                         CK_ULONG ulDataLen,
                                         CK_BYTE_PTR pSignature,
                                         CK_ULONG_PTR pulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pSignature);
	UNUSED(pulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyInit)(CK_SESSION_HANDLE hSession,
                                        CK_MECHANISM_PTR pMechanism,
                                        CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_Verify)(CK_SESSION_HANDLE hSession,
                                    CK_BYTE_PTR pData,
                                    CK_ULONG ulDataLen,
                                    CK_BYTE_PTR pSignature,
                                    CK_ULONG ulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pSignature);
	UNUSED(ulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyUpdate)(CK_SESSION_HANDLE hSession,
                                          CK_BYTE_PTR pPart,
                                          CK_ULONG ulPartLen) {
	UNUSED(hSession);
	UNUSED(pPart);
	UNUSED(ulPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyFinal)(CK_SESSION_HANDLE hSession,
                                         CK_BYTE_PTR pSignature,
                                         CK_ULONG ulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pSignature);
	UNUSED(ulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyRecoverInit)(CK_SESSION_HANDLE hSession,
                                               CK_MECHANISM_PTR pMechanism,
                                               CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyRecover)(CK_SESSION_HANDLE hSession,
                                           CK_BYTE_PTR pSignature,
                                           CK_ULONG ulSignatureLen,
                                           CK_BYTE_PTR pData,
                                           CK_ULONG_PTR pulDataLen) {
	UNUSED(hSession);
	UNUSED(pSignature);
	UNUSED(ulSignatureLen);
	UNUSED(pData);
	UNUSED(pulDataLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DigestEncryptUpdate)(CK_SESSION_HANDLE hSession,
                                                 CK_BYTE_PTR pPart,
                                                 CK_ULONG ulPartLen,
                                                 CK_BYTE_PTR pEncryptedPart,
                                                 CK_ULONG_PTR pulEncryptedPartLen) {
	UNUSED(hSession);
	UNUSED(pPart);
	UNUSED(ulPartLen);
	UNUSED(pEncryptedPart);
	UNUSED(pulEncryptedPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptDigestUpdate)(CK_SESSION_HANDLE hSession,
                                                 CK_BYTE_PTR pEncryptedPart,
                                                 CK_ULONG ulEncryptedPartLen,
                                                 CK_BYTE_PTR pPart,
                                                 CK_ULONG_PTR pulPartLen) {
	UNUSED(hSession);
	UNUSED(pEncryptedPart);
	UNUSED(ulEncryptedPartLen);
	UNUSED(pPart);
	UNUSED(pulPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignEncryptUpdate)(CK_SESSION_HANDLE hSession,
                                               CK_BYTE_PTR pPart,
                                               CK_ULONG ulPartLen,
                                               CK_BYTE_PTR pEncryptedPart,
                                               CK_ULONG_PTR pulEncryptedPartLen) {
	UNUSED(hSession);
	UNUSED(pPart);
	UNUSED(ulPartLen);
	UNUSED(pEncryptedPart);
	UNUSED(pulEncryptedPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptVerifyUpdate)(CK_SESSION_HANDLE hSession,
                                                 CK_BYTE_PTR pEncryptedPart,
                                                 CK_ULONG ulEncryptedPartLen,
                                                 CK_BYTE_PTR pPart,
                                                 CK_ULONG_PTR pulPartLen) {
	UNUSED(hSession);
	UNUSED(pEncryptedPart);
	UNUSED(ulEncryptedPartLen);
	UNUSED(pPart);
	UNUSED(pulPartLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_GenerateKey)(CK_SESSION_HANDLE hSession,
                                         CK_MECHANISM_PTR pMechanism,
                                         CK_ATTRIBUTE_PTR pTemplate,
                                         CK_ULONG ulCount,
                                         CK_OBJECT_HANDLE_PTR phKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(pTemplate);
	UNUSED(ulCount);
	UNUSED(phKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_GenerateKeyPair)(CK_SESSION_HANDLE hSession,
                                             CK_MECHANISM_PTR pMechanism,
                                             CK_ATTRIBUTE_PTR pPublicKeyTemplate,
                                             CK_ULONG ulPublicKeyAttributeCount,
                                             CK_ATTRIBUTE_PTR pPrivateKeyTemplate,
                                             CK_ULONG ulPrivateKeyAttributeCount,
                                             CK_OBJECT_HANDLE_PTR phPublicKey,
                                             CK_OBJECT_HANDLE_PTR phPrivateKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(pPublicKeyTemplate);
	UNUSED(ulPublicKeyAttributeCount);
	UNUSED(pPrivateKeyTemplate);
	UNUSED(ulPrivateKeyAttributeCount);
	UNUSED(phPublicKey);
	UNUSED(phPrivateKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_WrapKey)(CK_SESSION_HANDLE hSession,
                                     CK_MECHANISM_PTR pMechanism,
                                     CK_OBJECT_HANDLE hWrappingKey,
                                     CK_OBJECT_HANDLE hKey,
                                     CK_BYTE_PTR pWrappedKey,
                                     CK_ULONG_PTR pulWrappedKeyLen) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hWrappingKey);
	UNUSED(hKey);
	UNUSED(pWrappedKey);
	UNUSED(pulWrappedKeyLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_UnwrapKey)(CK_SESSION_HANDLE hSession,
                                       CK_MECHANISM_PTR pMechanism,
                                       CK_OBJECT_HANDLE hUnwrappingKey,
                                       CK_BYTE_PTR pWrappedKey,
                                       CK_ULONG ulWrappedKeyLen,
                                       CK_ATTRIBUTE_PTR pTemplate,
                                       CK_ULONG ulAttributeCount,
                                       CK_OBJECT_HANDLE_PTR phKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hUnwrappingKey);
	UNUSED(pWrappedKey);
	UNUSED(ulWrappedKeyLen);
	UNUSED(pTemplate);
	UNUSED(ulAttributeCount);
	UNUSED(phKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DeriveKey)(CK_SESSION_HANDLE hSession,
                                       CK_MECHANISM_PTR pMechanism,
                                       CK_OBJECT_HANDLE hBaseKey,
                                       CK_ATTRIBUTE_PTR pTemplate,
                                       CK_ULONG ulAttributeCount,
                                       CK_OBJECT_HANDLE_PTR phKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hBaseKey);
	UNUSED(pTemplate);
	UNUSED(ulAttributeCount);
	UNUSED(phKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SeedRandom)(CK_SESSION_HANDLE hSession,
                                        CK_BYTE_PTR pSeed,
                                        CK_ULONG ulSeedLen) {
	UNUSED(hSession);
	UNUSED(pSeed);
	UNUSED(ulSeedLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_GenerateRandom)(CK_SESSION_HANDLE hSession,
                                            CK_BYTE_PTR RandomData,
                                            CK_ULONG ulRandomLen) {
	UNUSED(hSession);
	UNUSED(RandomData);
	UNUSED(ulRandomLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetFunctionStatus)(CK_SESSION_HANDLE hSession) {
	UNUSED(hSession);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_CancelFunction)(CK_SESSION_HANDLE hSession) {
	UNUSED(hSession);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_WaitForSlotEvent)(CK_FLAGS flags,
                                              CK_SLOT_ID_PTR pSlot,
                                              CK_VOID_PTR pReserved) {
	UNUSED(flags);
	UNUSED(pSlot);
	UNUSED(pReserved);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @brief
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetInterfaceList)(CK_INTERFACE_PTR pInterfacesList,
                                              CK_ULONG_PTR pulCount) {
	if (NULL == pulCount) return CKR_ARGUMENTS_BAD;

	if (NULL == pInterfacesList) {
		*pulCount = 2;
	} else {
		if (*pulCount < 2) return CKR_BUFFER_TOO_SMALL;

		pInterfacesList[0].pInterfaceName = empty_pkcs11_2_40_interface.pInterfaceName;
		pInterfacesList[0].pFunctionList = empty_pkcs11_2_40_interface.pFunctionList;
		pInterfacesList[0].flags = empty_pkcs11_2_40_interface.flags;

		pInterfacesList[1].pInterfaceName = empty_pkcs11_3_1_interface.pInterfaceName;
		pInterfacesList[1].pFunctionList = empty_pkcs11_3_1_interface.pFunctionList;
		pInterfacesList[1].flags = empty_pkcs11_3_1_interface.flags;
	}

	return CKR_OK;
}

/**
 * @brief
 */
CK_DEFINE_FUNCTION(CK_RV, C_GetInterface)(CK_UTF8CHAR_PTR pInterfaceName,
                                          CK_VERSION_PTR pVersion,
                                          CK_INTERFACE_PTR_PTR ppInterface,
                                          CK_FLAGS flags) {
	if (NULL == ppInterface) return CKR_ARGUMENTS_BAD;

	if (flags != 0) {
		*ppInterface = NULL;
		return CKR_OK;
	}

	if (NULL != pInterfaceName) {
		const char *requested_interface_name = (const char *)pInterfaceName;
		const char *supported_interface_name = "PKCS 11";

		if (strlen(requested_interface_name) != strlen(supported_interface_name) ||
		    0 != strcmp(requested_interface_name, supported_interface_name)) {
			*ppInterface = NULL;
			return CKR_OK;
		}
	}

	if (NULL != pVersion) {
		if (pVersion->major == empty_pkcs11_2_40_functions.version.major &&
		    pVersion->minor == empty_pkcs11_2_40_functions.version.minor) {
			*ppInterface = &empty_pkcs11_2_40_interface;
			return CKR_OK;
		} else if (pVersion->major == empty_pkcs11_3_1_functions.version.major &&
		           pVersion->minor == empty_pkcs11_3_1_functions.version.minor) {
			*ppInterface = &empty_pkcs11_3_1_interface;
			return CKR_OK;
		} else {
			*ppInterface = NULL;
			return CKR_OK;
		}
	}

	*ppInterface = &empty_pkcs11_3_1_interface;
	return CKR_OK;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_LoginUser)(CK_SESSION_HANDLE hSession,
                                       CK_USER_TYPE userType,
                                       CK_UTF8CHAR_PTR pPin,
                                       CK_ULONG ulPinLen,
                                       CK_UTF8CHAR_PTR pUsername,
                                       CK_ULONG ulUsernameLen) {
	UNUSED(hSession);
	UNUSED(userType);
	UNUSED(pPin);
	UNUSED(ulPinLen);
	UNUSED(pUsername);
	UNUSED(ulUsernameLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SessionCancel)(CK_SESSION_HANDLE hSession, CK_FLAGS flags) {
	UNUSED(hSession);
	UNUSED(flags);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageEncryptInit)(CK_SESSION_HANDLE hSession,
                                                CK_MECHANISM_PTR pMechanism,
                                                CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_EncryptMessage)(CK_SESSION_HANDLE hSession,
                                            CK_VOID_PTR pParameter,
                                            CK_ULONG ulParameterLen,
                                            CK_BYTE_PTR pAssociatedData,
                                            CK_ULONG ulAssociatedDataLen,
                                            CK_BYTE_PTR pPlaintext,
                                            CK_ULONG ulPlaintextLen,
                                            CK_BYTE_PTR pCiphertext,
                                            CK_ULONG_PTR pulCiphertextLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pAssociatedData);
	UNUSED(ulAssociatedDataLen);
	UNUSED(pPlaintext);
	UNUSED(ulPlaintextLen);
	UNUSED(pCiphertext);
	UNUSED(pulCiphertextLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_EncryptMessageBegin)(CK_SESSION_HANDLE hSession,
                                                 CK_VOID_PTR pParameter,
                                                 CK_ULONG ulParameterLen,
                                                 CK_BYTE_PTR pAssociatedData,
                                                 CK_ULONG ulAssociatedDataLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pAssociatedData);
	UNUSED(ulAssociatedDataLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_EncryptMessageNext)(CK_SESSION_HANDLE hSession,
                                                CK_VOID_PTR pParameter,
                                                CK_ULONG ulParameterLen,
                                                CK_BYTE_PTR pPlaintextPart,
                                                CK_ULONG ulPlaintextPartLen,
                                                CK_BYTE_PTR pCiphertextPart,
                                                CK_ULONG_PTR pulCiphertextPartLen,
                                                CK_FLAGS flags) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pPlaintextPart);
	UNUSED(ulPlaintextPartLen);
	UNUSED(pCiphertextPart);
	UNUSED(pulCiphertextPartLen);
	UNUSED(flags);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageEncryptFinal)(CK_SESSION_HANDLE hSession) {
	UNUSED(hSession);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageDecryptInit)(CK_SESSION_HANDLE hSession,
                                                CK_MECHANISM_PTR pMechanism,
                                                CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptMessage)(CK_SESSION_HANDLE hSession,
                                            CK_VOID_PTR pParameter,
                                            CK_ULONG ulParameterLen,
                                            CK_BYTE_PTR pAssociatedData,
                                            CK_ULONG ulAssociatedDataLen,
                                            CK_BYTE_PTR pCiphertext,
                                            CK_ULONG ulCiphertextLen,
                                            CK_BYTE_PTR pPlaintext,
                                            CK_ULONG_PTR pulPlaintextLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pAssociatedData);
	UNUSED(ulAssociatedDataLen);
	UNUSED(pCiphertext);
	UNUSED(ulCiphertextLen);
	UNUSED(pPlaintext);
	UNUSED(pulPlaintextLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptMessageBegin)(CK_SESSION_HANDLE hSession,
                                                 CK_VOID_PTR pParameter,
                                                 CK_ULONG ulParameterLen,
                                                 CK_BYTE_PTR pAssociatedData,
                                                 CK_ULONG ulAssociatedDataLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pAssociatedData);
	UNUSED(ulAssociatedDataLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_DecryptMessageNext)(CK_SESSION_HANDLE hSession,
                                                CK_VOID_PTR pParameter,
                                                CK_ULONG ulParameterLen,
                                                CK_BYTE_PTR pCiphertextPart,
                                                CK_ULONG ulCiphertextPartLen,
                                                CK_BYTE_PTR pPlaintextPart,
                                                CK_ULONG_PTR pulPlaintextPartLen,
                                                CK_FLAGS flags) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pCiphertextPart);
	UNUSED(ulCiphertextPartLen);
	UNUSED(pPlaintextPart);
	UNUSED(pulPlaintextPartLen);
	UNUSED(flags);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageDecryptFinal)(CK_SESSION_HANDLE hSession) {
	UNUSED(hSession);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageSignInit)(CK_SESSION_HANDLE hSession,
                                             CK_MECHANISM_PTR pMechanism,
                                             CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignMessage)(CK_SESSION_HANDLE hSession,
                                         CK_VOID_PTR pParameter,
                                         CK_ULONG ulParameterLen,
                                         CK_BYTE_PTR pData,
                                         CK_ULONG ulDataLen,
                                         CK_BYTE_PTR pSignature,
                                         CK_ULONG_PTR pulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pSignature);
	UNUSED(pulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignMessageBegin)(CK_SESSION_HANDLE hSession,
                                              CK_VOID_PTR pParameter,
                                              CK_ULONG ulParameterLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_SignMessageNext)(CK_SESSION_HANDLE hSession,
                                             CK_VOID_PTR pParameter,
                                             CK_ULONG ulParameterLen,
                                             CK_BYTE_PTR pData,
                                             CK_ULONG ulDataLen,
                                             CK_BYTE_PTR pSignature,
                                             CK_ULONG_PTR pulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pSignature);
	UNUSED(pulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageSignFinal)(CK_SESSION_HANDLE hSession) {
	UNUSED(hSession);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageVerifyInit)(CK_SESSION_HANDLE hSession,
                                               CK_MECHANISM_PTR pMechanism,
                                               CK_OBJECT_HANDLE hKey) {
	UNUSED(hSession);
	UNUSED(pMechanism);
	UNUSED(hKey);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyMessage)(CK_SESSION_HANDLE hSession,
                                           CK_VOID_PTR pParameter,
                                           CK_ULONG ulParameterLen,
                                           CK_BYTE_PTR pData,
                                           CK_ULONG ulDataLen,
                                           CK_BYTE_PTR pSignature,
                                           CK_ULONG ulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pSignature);
	UNUSED(ulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyMessageBegin)(CK_SESSION_HANDLE hSession,
                                                CK_VOID_PTR pParameter,
                                                CK_ULONG ulParameterLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_VerifyMessageNext)(CK_SESSION_HANDLE hSession,
                                               CK_VOID_PTR pParameter,
                                               CK_ULONG ulParameterLen,
                                               CK_BYTE_PTR pData,
                                               CK_ULONG ulDataLen,
                                               CK_BYTE_PTR pSignature,
                                               CK_ULONG ulSignatureLen) {
	UNUSED(hSession);
	UNUSED(pParameter);
	UNUSED(ulParameterLen);
	UNUSED(pData);
	UNUSED(ulDataLen);
	UNUSED(pSignature);
	UNUSED(ulSignatureLen);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * @todo
 */
CK_DEFINE_FUNCTION(CK_RV, C_MessageVerifyFinal)(CK_SESSION_HANDLE hSession) {
	UNUSED(hSession);

	return CKR_FUNCTION_NOT_SUPPORTED;
}

/** @} */
