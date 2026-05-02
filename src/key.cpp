#include "key.hpp"
#include "libtropic_common.h"
#include <string>
#include <vector>

static const char b64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void append_u32(std::vector<uint8_t>& v, uint32_t val) {
    v.push_back((val >> 24) & 0xFF);
    v.push_back((val >> 16) & 0xFF);
    v.push_back((val >> 8) & 0xFF);
    v.push_back(val & 0xFF);
}

static std::string base64_encode(const std::vector<uint8_t>& data){
    std::string out;

    size_t i = 0;
    uint32_t buf = 0;
    int bits = 0;

    for (uint8_t byte : data) {
        buf = (buf << 8) | byte;
        bits += 8;

        while (bits >= 6) {
            bits -= 6;
            out.push_back(b64_table[(buf >> bits) & 0x3F]);
        }
    }

    if (bits > 0) {
        buf <<= (6 - bits);
        out.push_back(b64_table[buf & 0x3F]);
    }

    while (out.size() % 4) {
        out.push_back('=');
    }

    return out;
}

Ed25519Key::Ed25519Key(lt_ecc_slot_t slot){
	this->slot = slot;
}

uint8_t* Ed25519Key::data() {
    return pubkey.data();
}

const uint8_t* Ed25519Key::data() const {
    return pubkey.data();
}

std::array<uint8_t, ED25519_KEY_LEN>& Ed25519Key::get_pubkey() {
    return pubkey;
}

const std::array<uint8_t, ED25519_KEY_LEN>& Ed25519Key::get_pubkey() const {
    return pubkey;
}

lt_ecc_slot_t Ed25519Key::get_slot() const {
    return slot;
}

std::string Ed25519Key::to_ssh_ed25519() {
    std::vector<uint8_t> blob;

    std::string key_type = "ssh-ed25519";

    // string "ssh-ed25519"
    append_u32(blob, key_type.size());
    blob.insert(blob.end(), key_type.begin(), key_type.end());

    // public key
    append_u32(blob, pubkey.size());
    blob.insert(blob.end(), pubkey.begin(), pubkey.end());

    return "ssh-ed25519 " + base64_encode(blob);
}
