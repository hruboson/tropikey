# Tropikey
Implementation of the [PKCS#11](https://en.wikipedia.org/wiki/PKCS_11) API utilizing the [Tropic Square USB dev kit](https://tropicsquare.github.io/libtropic/latest/tutorials/linux/usb_devkit/).

This project is mostly for me to learn how cryptography in computers really works and my attempt to create an open-source hardware security key (similar to a YubiKey). I don't have much prior knowledge about this topic, so the implementation may end up being sloppy and maybe not even secure at all (we shall see in a few months).

The idea is to create a hardware security token for my [NixOS machine](https://github.com/hruboson/nixos-conf). Mostly for my git remote authentication keys, ssh keys and possibly more.

I was fortunate enough to stumble upon an interesting presentation by [Ondřej Vičar](https://www.linkedin.com/in/ondrej-vicar/) at the [Moravian IT Beer](https://luma.com/fulnesul) conference. The presentation was about the [TROPIC01 chip](https://tropicsquare.com/tropic01), its architecture, features, open source mindset and testing. Even though I would not consider myself a "hardware person", I found the idea of open source secure chip fascinating. To me the idea of open source means a product (and mostly code) someone truly stands behind. Their catch-prhase *"Forget obscurity"* (and the less-pr-friendly *Fcuk obscurity*) is something I think everyone should get behind - a system should stay secure even when everything is known about it except for the key ([the Kerckhoff's principle](https://en.wikipedia.org/wiki/Kerckhoffs%27s_principle)). 

## PKCS#11

PKCS#11 (also known as *Cryptoki*) is a standardized API for interacting with cryptographic tokens such as hardware security modules (HSMs), smart cards, or custom devices. Instead of accessing private keys directly, applications use this interface to perform cryptographic operations (e.g. signing) inside a secure device.

The main advantage of PKCS#11 is that **private keys never leave the device**. This provides strong security guarantees and allows integration with existing software like OpenSSH, browsers, or TLS libraries without modifying them.

In the context of SSH, PKCS#11 enables hardware-backed authentication: the SSH client requests a signature, and the device performs it internally, preventing key extraction.

### General flow of PKCS#11 functions

1. `C_GetFunctionList` – obtain the function table exported by the module
2. `C_Initialize` – initialize PKCS#11 library
3. `C_GetSlotList` – enumerate available token slots/devices
4. `C_OpenSession` – open a session with a selected slot
5. `C_Login` – authenticate to the token (e.g. PIN, if required)
6. `C_Logout` – end authenticated session
7. `C_CloseSession` – close the session
8. `C_Finalize` – clean up and shut down the library

# Usage

So far I have only tried this on NixOS. This repo contains a `flake.nix` so you can add this to your flake based config as an input.

```
inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.11";
    tropikey = {
        url = "github:hruboson/tropikey";
        inputs.nixpkgs.follows = "nixpkgs";
    };
};
```

Then simply enable it in your config:

```
imports = [
    inputs.tropikey.nixosModules.default
];

programs.tropikey = {
    enable = true; # get the tropikey binary to test the USB devkit
    enableSshPkcs11 = true; # automatically integrates with the system-wide ssh config
};
```

You can find example usage in my [NixOS configuration repo](https://github.com/hruboson/nixos-conf/blob/parts/parts/services/tropikey.nix)|[flake.nix](https://github.com/hruboson/nixos-conf/blob/parts/flake.nix) (`parts` branch).

## Build

- Using Nix:
    - `nix-shell` or `nix develop` to get into the development environment.
    - `nix build` to build the app and PKCS#11 `.so` library object.

- Without Nix:
    - Install CMake and Make. Optionally openssh and pkcs11-tools for testing.
    - Download mbedtls and libsodium libraries.
    - `make build` to generate the `.so` library implementation for your ssh agent.
    - `make run` to run the sample program (good to check communication with the USB dev kit).

### Libraries used

- [libtropic](https://github.com/tropicsquare/libtropic/) - communication with the TROPIC01 chip through the USB dev kit.
- [empty-pkcs11](https://github.com/Pkcs11Interop/empty-pkcs11) - PKCS#11 minimalist implementation.
- [pkcs11.h](https://docs.oasis-open.org/pkcs11/pkcs11-base/v2.40/os/pkcs11-base-v2.40-os.html) - PKCS #11 Cryptographic Token Interface Base Specification Version 2.40
- [mbedtls](https://github.com/Mbed-TLS/mbedtls) - for libtropic
- [libsodium](https://github.com/jedisct1/libsodium) - only for testing

## Testing

There are no unit tests at the moment and I'm mostly discovering how to actually test the pkcs#11 implementation using the `pkcs11-tool` and `ssh-keygen`. Both of these commands are available in the nix shell.

# References

Learning and reference materials I used while writing this...

1. https://github.com/Pkcs11Interop/empty-pkcs11
1. https://docs.oracle.com/en/java/javase/11/security/pkcs11-reference-guide1.html
1. https://tropicsquare.github.io/libtropic/latest/tutorials/model/hw_wallet/
1. https://tropicsquare.github.io/libtropic/latest/tutorials/linux/usb_devkit/full_chain_verification/
1. https://tropicsquare.com/tropic01
1. https://tropicsquare.github.io/libtropic/latest/reference/libtropic_architecture/
1. https://docs.oasis-open.org/pkcs11/pkcs11-base/v2.40/os/pkcs11-base-v2.40-os.html
1. https://tropicsquare.github.io/libtropic/latest/tutorials/linux/usb_devkit/
1. https://tropicsquare.github.io/libtropic/latest/doxygen/build/html/
1. https://cryptobook.nakov.com/
1. https://cryptobook.nakov.com/digital-signatures/eddsa-and-ed25519
1. https://www.youtube.com/watch?v=KrfRfQlE5k0
1. https://www.cryptsoft.com/pkcs11doc/
1. https://www.youtube.com/watch?v=zTt9wp5vXDE

---

[LICENSE](LICENSE)
