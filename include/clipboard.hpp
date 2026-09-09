#pragma once

#include <string>

// Copies `text` to the system clipboard.
// Returns true on success, false if no clipboard mechanism was available
// or the copy failed.
bool copy_to_clipboard(const std::string &text);
