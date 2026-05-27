#include "tui.hpp"
#include "device.hpp"

#include <cstdlib>
#include <ftxui/component/screen_interactive.hpp>
#include <iomanip>
#include <random>
#include <sodium.h>
#include <sstream>

#include "ftxui/component/app.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

/**
 * TODO:
 *  - some kind of MVC/MVVM structure
 *  - maybe common interface for tui and (future) gui app
 *  - separation of concerns!!! logging
 */

using namespace ftxui;

std::vector<uint8_t> generate_challenge(size_t size = 32) {
	std::vector<uint8_t> challenge(size);
	std::random_device rd;
	for (auto &b : challenge) {
		b = static_cast<uint8_t>(rd());
	}
	return challenge;
}

bool verify_signature(const std::array<uint8_t, ED25519_KEY_LEN> &pubkey,
                      const std::vector<uint8_t> &challenge,
                      const std::vector<uint8_t> &signature) {
	if (pubkey.size() != crypto_sign_PUBLICKEYBYTES) return false;
	if (signature.size() != crypto_sign_BYTES) return false;
	return crypto_sign_verify_detached(
	           signature.data(), challenge.data(), challenge.size(), pubkey.data()) == 0;
}

TuiApp::TuiApp(Device &device, std::ostream *out) : device(device), out(out) {
	if (out != &std::cout) {
		std::ofstream *file_out = dynamic_cast<std::ofstream *>(out);
		if (file_out) {
			// out is a file stream, already initialized
		}
	}
}

std::string TuiApp::bytes_to_hex(const std::vector<uint8_t> &bytes) {
	std::stringstream ss;
	for (auto byte : bytes) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
	}
	return ss.str();
}

TuiApp::~TuiApp() {
	if (!device.close()) {
		// this might be better as deinitialize function and not destructor
		exit(EXIT_FAILURE);
	}
}

bool TuiApp::init_device() {
	device_initialized = false;

	log_entries.clear();
	log_entries.push_back("Initializing device...");

	if (!device.init()) {
		// todo logs
		log_entries.push_back("Device not found");
		return EXIT_FAILURE;
	}

	if (!device.print_info(*out)) {
		log_entries.push_back("Could not print device info");
		return EXIT_FAILURE;
	}

	if (!device.start_secure_session()) {
		log_entries.push_back("Secure session could not be initialized");
		return EXIT_FAILURE;
	}

	device_initialized = true;

	return true;
}

void TuiApp::handle_sign_challenge() {
	if (!device_initialized) {
		signature_status = "Device not initialized";
		return;
	}

	signing = true;

	// Generate challenge
	auto challenge = generate_challenge();
	current_challenge_hex = bytes_to_hex(challenge);

	// Initialize and read key
	Ed25519Key key(TR01_ECC_SLOT_0);
	if (!device.initialize_ed25519_key(key)) {
		signature_status = "Failed to initialize key";
		signing = false;
		return;
	}

	if (!device.read_ed25519_key(key)) {
		signature_status = "Failed to read key";
		signing = false;
		return;
	}

	// Sign challenge
	std::vector<uint8_t> signature;
	if (!device.sign_ed25519_challenge(key, challenge, signature)) {
		signature_status = "Failed to sign challenge";
		signing = false;
		return;
	}

	current_signature_hex = bytes_to_hex(signature);

	// Verify signature
	if (verify_signature(key.get_pubkey(), challenge, signature)) {
		signature_status = "Signature VALID";
	} else {
		signature_status = "Signature INVALID";
	}

	signing = false;
}

void TuiApp::run() {
	if (!init_device()) {
		std::cerr << "Failed to initialize device\n";
		return;
	}

	auto sign_button = Button("Generate & Sign Challenge", [this] { handle_sign_challenge(); });
	auto find_device_button = Button("Reinitialize/find device", [this] { init_device(); });

	auto container = Container::Vertical({
	    sign_button,
		find_device_button
	});

	auto renderer = Renderer(container, [this, &sign_button, &find_device_button] {
		// Build log entries
		Elements log_elements;
		for (const auto &entry : log_entries) {
			log_elements.push_back(text(entry));
		}

		// Main layout
		return vbox({
		           // Header
		           hbox({
		               text("Tropikey - FTXUI Demo") | bold | color(Color::Cyan),
		               filler(),
		           }) | border,

		           // Status section
		           vbox({
		               text(device_initialized ? "✓ Device Ready" : "⟳ Initializing...") |
		                   (device_initialized ? color(Color::Green) : color(Color::Yellow)),
		           }) | border,

		           // Device Info
		           vbox({
		               hbox({
		                   text("HW Version: ") | color(Color::White),
		                   text(std::to_string(device.get_hw_version().major) + "." +
		                        std::to_string(device.get_hw_version().minor) + "." +
		                        std::to_string(device.get_hw_version().patch)),
		               }),
		               hbox({
		                   text("FW Version: ") | color(Color::White),
		                   text(std::to_string(device.get_fw_version().major) + "." +
		                        std::to_string(device.get_fw_version().minor) + "." +
		                        std::to_string(device.get_fw_version().patch)),
		               }),
		           }) | border,

		           // Controls
		           hbox({
		               sign_button->Render() | center,
		               find_device_button->Render() | center,
		           }) | border,

		           // Challenge section
		           vbox({
		               text("Challenge (hex):") | color(Color::Yellow),
		               text(current_challenge_hex.empty() ? "(none)" : current_challenge_hex) | dim,
		           }) | border,

		           // Signature section
		           vbox({
		               text("Signature (hex):") | color(Color::Yellow),
		               text(current_signature_hex.empty() ? "(none)" : current_signature_hex) | dim,
		           }) | border,

		           // Status
		           vbox({
		               text("Status:") | color(Color::Cyan),
		               text(signature_status.empty() ? "-" : signature_status) | bold |
		                   (signature_status.find("VALID") != std::string::npos
		                        ? color(Color::Green)
		                    : signature_status.find("Failed") != std::string::npos
		                        ? color(Color::Red)
		                        : color(Color::White)),
		           }) | border,

		           // Initialization log
		           vbox({
		               text("Initialization Log:") | color(Color::Magenta),
		               vbox(log_elements) | border,
		           }),

		           // Instructions
		           text("Press Ctrl+C to exit") | dim,
		       }) |
		       border | color(Color::White);
	});

	auto screen = App::TerminalOutput();
	screen.Loop(renderer);
}
