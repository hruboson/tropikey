#include <fstream>
#include <iostream>

#include "device.hpp"

class TuiApp {
public:
	TuiApp(Device &device, std::ostream *out);
	~TuiApp();
	void run();

private:
	std::ostream *out = &std::cout;
	std::ofstream file;

	Device& device;
	bool init_device();

	void handle_sign_challenge();
	std::string bytes_to_hex(const std::vector<uint8_t> &bytes);

	// UI State
	std::vector<std::string> log_entries;
	std::string current_challenge_hex;
	std::string current_signature_hex;
	std::string signature_status;
	bool device_initialized = false;
	bool signing = false;
};
