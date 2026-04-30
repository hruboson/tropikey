# Tropikey
Implementation of the [PKCS#11](https://en.wikipedia.org/wiki/PKCS_11) API utilizing the [Tropic Square USB dev kit](https://tropicsquare.github.io/libtropic/latest/tutorials/linux/usb_devkit/).

This project is mostly for me to learn how cryptography in computers really works and my attempt to create an open-source hardware security key (similar to a YubiKey). I don't have much prior knowledge about this topic, so the implementation may end up being sloppy and maybe not even secure at all (we shall see in a few months).

The idea is to create a hardware security token for my [NixOS machine](https://github.com/hruboson/nixos-conf). Mostly for my git remote authentication keys, ssh keys and possibly more.

I was fortunate enough to stumble upon an interesting presentation by [Ondřej Vičar](https://www.linkedin.com/in/ondrej-vicar/) at the [Moravian IT Beer](https://luma.com/fulnesul) conference. The presentation was about the [TROPIC01 chip](https://tropicsquare.com/tropic01), its architecture, features, open source mindset and testing. Even though I would not consider myself a "hardware person", I found the idea of open source secure chip fascinating. To me the idea of open source means a product (and mostly code) someone truly stands behind. Their catch-prhase *"Forget obscurity"* (and the less-pr-friendly *Fcuk obscurity*) is something I think everyone should get behind - a system should stay secure even when everything is known about it except for the key ([the Kerckhoff's principle](https://en.wikipedia.org/wiki/Kerckhoffs%27s_principle)). 

## Build

Using Nix:
- `nix-shell` to get into the development environment
- `make build` to generate the `.so` library implementation for your ssh agent
- `make run` to run the sample program (good to check communication with the USB dev kit)

### Libraries used

All necessary libraries to buld the project are already included in the repo. There should be no need to download or set up any manually.

- [libtropic](https://github.com/tropicsquare/libtropic/) - communication with the TROPIC01 chip through the USB dev kit.
- [empty-pkcs11](https://github.com/Pkcs11Interop/empty-pkcs11) - PKCS#11 minimalist implementation.
- [pkcs11.h](https://docs.oasis-open.org/pkcs11/pkcs11-base/v2.40/os/pkcs11-base-v2.40-os.html) - PKCS #11 Cryptographic Token Interface Base Specification Version 2.40

## Testing

There are no unit tests at the moment and I'm mostly discovering how to actually test the pkcs#11 implementation using the `pkcs11-tool` and `ssh-keygen`. Both of these commands are available in the nix shell.

---

[LICENSE](LICENSE)
