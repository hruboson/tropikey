#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdarg>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_posix_usb_dongle.h"
#include "psa/crypto.h"


struct lt_handle_t;
struct lt_dev_posix_usb_dongle_t;
struct lt_ctx_mbedtls_v4_t;

class Device {
public:
    Device();
    ~Device();

    bool init();
    bool print_info(std::ostream& out);

private:
    lt_handle_t* lt_handle_ptr();

    lt_handle_t lt_handle{};
    lt_dev_posix_usb_dongle_t dongle{};
    lt_ctx_mbedtls_v4_t crypto_ctx{};

    bool fail(const char* msg, lt_ret_t ret);
};
