#include "tui.hpp"
#include "clipboard.hpp"
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
#include "ftxui/screen/screen.hpp""
#include "key.hpp"

/**
 * TODO:
 *  - maybe common interface for tui and (future) gui app
 *  - version stored in variable somewhere
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
	searching_for_device = true;

	log_entries.clear();
	log_entries.push_back("Initializing device...");
	status = "Initializing device...";

	if (!device.init()) {
		// todo logs
		log_entries.push_back("Device not found");
		status = "Device not found";

		searching_for_device = false;
		return false;
	}

	if (!device.print_info()) {
		log_entries.push_back("Could not print device info");

		status = "Could not print device info";
		searching_for_device = false;
		return false;
	}

	if (!device.start_secure_session()) {
		log_entries.push_back("Secure session could not be initialized");

		status = "Secure session could not be initialized";
		searching_for_device = false;
		return false;
	}

	device_initialized = true;
	searching_for_device = false;
	status = "Ready";

	return true;
}

void TuiApp::read_key() {
	if (!device_initialized){
		status = "Device not initialized";
		key.reset();
		return;
	}

	reading_key = true;

	// Initialize and read key
	key.emplace(TR01_ECC_SLOT_0);
	if (!device.initialize_ed25519_key(*key)) {
		status = "Failed to initialize key";

		key.reset();
		reading_key = false;
		return;
	}

	if (!device.read_ed25519_key(*key)) {
		status = "Failed to read key";

		key.reset();
		reading_key = false;
		return;
	}

	reading_key = false;
}

void TuiApp::handle_sign_challenge() {
	if (!device_initialized) {
		status = "Device not initialized";
		return;
	}

	signing = true;

	// Generate challenge
	auto challenge = generate_challenge();
	current_challenge_hex = bytes_to_hex(challenge);

	// Initialize and read key
	key.emplace(TR01_ECC_SLOT_0);
	if (!device.initialize_ed25519_key(*key)) {
		status = "Failed to initialize key";
		signing = false;
		return;
	}

	if (!device.read_ed25519_key(*key)) {
		status = "Failed to read key";
		signing = false;
		return;
	}

	// Sign challenge
	std::vector<uint8_t> signature;
	if (!device.sign_ed25519_challenge(*key, challenge, signature)) {
		status = "Failed to sign challenge";
		signing = false;
		return;
	}

	current_signature_hex = bytes_to_hex(signature);

	// Verify signature
	if (verify_signature(key->get_pubkey(), challenge, signature)) {
		status = "Signature VALID";
	} else {
		status = "Signature INVALID";
	}

	signing = false;
}

void TuiApp::copy_pubkey_to_clipboard() {
	if (!key.has_value()) {
		status = "No key loaded";
		return;
	}

	if (copy_to_clipboard(key->to_ssh_ed25519())) {
		status = "Pubkey copied to clipboard";
	} else {
		status = "Failed to copy pubkey (no clipboard tool found)";
	}
}

Element AsciiArt(const std::string &art) {
	Elements lines;
	std::stringstream ss(art);
	std::string line;
	while (std::getline(ss, line)) {
		lines.push_back(text(line));
	}
	return vbox(std::move(lines));
}

static const std::string tropikeyAciiArt = R"(
     ┌───────┐
   ┌─┘       └─┐
 ┌─┘           └─┐
 ▌   ┌──┐        └──+"+───·~^~·────..-─┐
 ▌   [  ]           TROPIKEY   v. 0.1  ]]
 ▌   └──┘        ┌⌐¿¬──..────.   .───'`┘
 └─┐           ┌─┘            `"'
   └─┐       ┌─┘
     └───────┘

)";

void TuiApp::run() {
	if (!init_device()) {
		key.reset();
	} else {
		read_key();
	}

	auto screen = App::TerminalOutput();

	auto container = Container::Vertical({});

	auto renderer = Renderer(container, [this] {
		// Build log entries
		Elements log_elements;
		for (const auto &entry : log_entries) {
			log_elements.push_back(text(entry));
		}

		// Main layout
		return vbox({
		    hbox({
		        AsciiArt(tropikeyAciiArt) | center,
		    }) | center,

		    vbox({
		        hbox({
		            vbox({
		                text("Status "),
		            }),
		            separator(),
		            vbox({
		                text(
								device_initialized ? "Device ready" : searching_for_device ? "Searching..." : "Not found"
							) | center
							  | ( device_initialized ? color(Color::Green) : searching_for_device ? color(Color::Blue) : color(Color::Yellow) ),
		            }) | flex,
		        }),
		        separator(),

		        hbox({
		            vbox({
		                text("Pubkey "),
		            }),
		            separator(),
		            vbox({
		                text(
								key.has_value() ? this->key->to_ssh_ed25519() : " ... "
							) | center,
		            }) | flex,
		        }),
		    }) | border,

		    hbox({
		        text(" q - exit ") | inverted,
		        text(" "),
		        text(" y - copy pubkey ") | inverted,
		        text(" "),
				// TODO think about how I want to go abou this - if automatically
				// generate one key on first use or allow re-generating (since I
				// use single-slot mode on the HW chip)
		        /*text(" g - generate key ") | inverted,
		        text(" "),*/
				// Add button/shortcut to regenerate key but first prompt for
				// something like "Input the following: I_REALLYWANT2REGENERATEKEY" (github-like when deleting repo)
		        text(" r - reload device ") | inverted,
		    }) | center,
		    text( ">_ " + status) | dim | center,
		});
	});

	auto main_component = CatchEvent(renderer, [this, &screen](Event event) {
		if (event == Event::Character('q')) {
			screen.Exit();
			return true;
		}
		if (event == Event::Character('y')) {
			// copy pubkey to clipboard
			copy_pubkey_to_clipboard();
			return true;
		}
		if (event == Event::Character('r')) {
			if(!init_device()){
				key.reset();
				return false;
			}

			read_key();
			return true;
		}
		return false; // not handled, let it propagate
	});

	screen.Loop(main_component);
}
