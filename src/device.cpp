#include "device.hpp"
#include "libtropic.h"
#include "libtropic_common.h"

#include <cstdarg>
#include <cstdio>
#include <vector>

// global bridge for C callback
static std::ostream* g_out = nullptr;

static int print_cb(const char* fmt, ...) {
	char buffer[1024];

	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	if (g_out) {
		(*g_out) << buffer;
	}

	return n;
}

Device::Device() {}

Device::~Device() {

}

bool Device::init() {
	lt_handle = {};
	dongle = {};

	psa_status_t status = psa_crypto_init();
	if (status != PSA_SUCCESS) {
		std::cerr << "PSA Crypto initialization failed, status=" << status << "\n";
		return false;
	}

	int len = snprintf(dongle.dev_path, sizeof(dongle.dev_path), "%s", LT_USB_DEVKIT_PATH);
	if (len < 0 || static_cast<size_t>(len) >= sizeof(dongle.dev_path)) {
		std::cerr << "Error: LT_USB_DEVKIT_PATH too long\n";
		return false;
	}

	dongle.baud_rate = 115200;
	lt_handle.l2.device = &dongle;
	lt_handle.l3.crypto_ctx = &crypto_ctx;

	std::cout << "Initializing handle...";
	lt_ret_t ret = lt_init(&lt_handle);
	if (ret != LT_OK) {
		std::cerr << "\nFailed to initialize handle, ret=" << lt_ret_verbose(ret) << "\n";
		return false;
	}
	std::cout << "OK\n";

	return true;
}

bool Device::close(){
	lt_ret_t ret = LT_OK;

	std::cout << "Aborting secure session ... ";
    ret = lt_session_abort(&lt_handle);
    if (LT_OK != ret) {
		std::cerr << "\nFailed to abort Secure Session, ret=" << lt_ret_verbose(ret) << "\n";
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
		return false;
    }
	std::cout << "OK\n";

	ret = lt_deinit(&lt_handle);
	if (LT_OK != ret){
		std::cerr << "Could not deinitialize handle, ret=" << lt_ret_verbose(ret) << "\n";
		mbedtls_psa_crypto_free();
		return false;
	}
	mbedtls_psa_crypto_free();

	std::cout << "Deinitialization successful!\n";

	return true;
}

bool Device::start_secure_session(){
	std::cout << "Starting Secure Session with key slot " << (int)TR01_PAIRING_KEY_SLOT_INDEX_0 << " ... ";
    // Keys are chosen based on the CMake option LT_SH0_KEYS.
    lt_ret_t ret = lt_verify_chip_and_start_secure_session(&lt_handle, LT_EX_SH0_PRIV, LT_EX_SH0_PUB, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
		std::cerr << "\nFailed to start Secure Session with key " << (int)TR01_PAIRING_KEY_SLOT_INDEX_0 << " ret=" << lt_ret_verbose(ret) << "\n";
		std::cerr << "Check if you use correct SH0 keys! Hint: if you use an engineering sample chip, compile with\n"
                  << "-DLT_SH0_KEYS=eng_sample\n";
        return false;
    }
	std::cout << "OK\n";

	std::cout << "Testing communication with a PING message ... ";
	std::string ping_msg = "Hello world!";
	
	std::vector<uint8_t> recv_buf(ping_msg.size());
	
	ret = lt_ping(&lt_handle, reinterpret_cast<const uint8_t*>(ping_msg.data()), recv_buf.data(), ping_msg.size());
    if (LT_OK != ret) {
		std::cerr << "Ping command failed, ret=" << lt_ret_verbose(ret) << "\n";
        return false;
    }

	std::string recv_str(reinterpret_cast<char*>(recv_buf.data()), recv_buf.size());
	if (ping_msg != recv_str){
		std::cerr << "Ping command did not return the sent string, aborting\n";
	}

	std::cout << "OK\n";

	return true;
}

bool Device::print_info(std::ostream& out) {
	g_out = &out;

	auto print = [&](const char* fmt, ...) {
		char buffer[1024];
		va_list ap;
		va_start(ap, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, ap);
		va_end(ap);
		out << buffer;
	};

	lt_ret_t ret;

	out << "Sending reboot request...";
	ret = lt_reboot(&lt_handle, TR01_REBOOT);
	if (ret != LT_OK) return fail("lt_reboot", ret);
	out << "OK\n";

	out << "Reading data from chip...\n";

	uint8_t fw_ver[4];

	ret = lt_get_info_riscv_fw_ver(&lt_handle, fw_ver);
	if (ret != LT_OK) return fail("RISC-V FW", ret);
	print("  RISC-V FW version: %02X.%02X.%02X (.%02X)\n",
			fw_ver[3], fw_ver[2], fw_ver[1], fw_ver[0]);

	ret = lt_get_info_spect_fw_ver(&lt_handle, fw_ver);
	if (ret != LT_OK) return fail("SPECT FW", ret);
	print("  SPECT FW version: %02X.%02X.%02X (.%02X)\n",
			fw_ver[3], fw_ver[2], fw_ver[1], fw_ver[0]);

	out << "Sending maintenance reboot request...";
	ret = lt_reboot(&lt_handle, TR01_MAINTENANCE_REBOOT);
	if (ret != LT_OK) return fail("lt_reboot", ret);
	out << "OK\n";

	out << "Reading data from chip...\n";
	out << "Firmware bank headers:\n";

	ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_FW1, print_cb);
	if (ret != LT_OK) return fail("FW1 header", ret);

	ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_FW2, print_cb);
	if (ret != LT_OK) return fail("FW2 header", ret);

	ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_SPECT1, print_cb);
	if (ret != LT_OK) return fail("SPECT1 header", ret);

	ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_SPECT2, print_cb);
	if (ret != LT_OK) return fail("SPECT2 header", ret);

	lt_chip_id_t chip_id{};

	out << "Chip ID data:\n";
	ret = lt_get_info_chip_id(&lt_handle, &chip_id);
	if (ret != LT_OK) return fail("chip id", ret);

	out << "---------------------------------------------------------\n";

	ret = lt_print_chip_id(&chip_id, print_cb);
	if (ret != LT_OK) return fail("print chip id", ret);

	out << "---------------------------------------------------------\n";

	out << "Sending reboot request...";
	ret = lt_reboot(&lt_handle, TR01_REBOOT);
	if (ret != LT_OK) return fail("lt_reboot", ret);
	out << "OK!\n";

	return true;
}

bool Device::fail(const char* msg, lt_ret_t ret) {
	std::cerr << "Error: " << msg << " failed, ret=" << lt_ret_verbose(ret) << "\n";
	return false;
}
