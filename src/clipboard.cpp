#include "clipboard.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    // uses pbcopy via popen, no extra headers needed
#else
    // Linux/BSD: uses wl-copy / xclip / xsel via popen
#endif

namespace {

#if !defined(_WIN32)

/**
 * Runs cmd, writes text to its stdin, returns true if the process
 * launched and exited with status 0.
 *
 * @param cmd Command to run
 * @param text Text to write to stdin
 */
bool run_with_stdin(const std::string &cmd, const std::string &text) {
	FILE *pipe = popen(cmd.c_str(), "w");
	if (!pipe) return false;

	// fwrite may fail if the target program isn't actually installed
	// (popen still "succeeds" via the shell in that case), so we check
	// pclose's exit status too.
	size_t written = fwrite(text.data(), 1, text.size(), pipe);
	int status = pclose(pipe);

	return written == text.size() && status == 0;
}

bool command_exists(const std::string &name) {
	std::string check = "command -v " + name + " >/dev/null 2>&1";
	return system(check.c_str()) == 0;
}
#endif

} // namespace

bool copy_to_clipboard(const std::string &text) {
#if defined(_WIN32)
	if (!OpenClipboard(nullptr)) return false;

	if (!EmptyClipboard()) {
		CloseClipboard();
		return false;
	}

	HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (!hmem) {
		CloseClipboard();
		return false;
	}

	void *ptr = GlobalLock(hmem);
	if (!ptr) {
		GlobalFree(hmem);
		CloseClipboard();
		return false;
	}
	memcpy(ptr, text.c_str(), text.size() + 1);
	GlobalUnlock(hmem);

	// Clipboard now owns hmem; do not free it ourselves.
	bool ok = SetClipboardData(CF_TEXT, hmem) != nullptr;
	CloseClipboard();
	return ok;

#elif defined(__APPLE__)
	return run_with_stdin("pbcopy", text);

#else // Linux / BSD (X11 and Wayland)
	// Prefer wl-copy under Wayland sessions.
	const char *wayland_display = getenv("WAYLAND_DISPLAY");
	if (wayland_display && *wayland_display && command_exists("wl-copy")) {
		if (run_with_stdin("wl-copy", text)) return true;
	}

	// X11 fallbacks. xclip and xsel both work fine under XWayland too,
	// so try them regardless if wl-copy isn't available/failed.
	if (command_exists("xclip")) {
		if (run_with_stdin("xclip -selection clipboard", text)) return true;
	}
	if (command_exists("xsel")) {
		if (run_with_stdin("xsel --clipboard --input", text)) return true;
	}

	return false;
#endif
}
