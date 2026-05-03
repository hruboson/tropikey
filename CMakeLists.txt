cmake_minimum_required(VERSION 3.21.0)

project(tropikey LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

###########################################################################
#                                                                         #
#   Configuration                                                         #
#                                                                         #
###########################################################################

if(NOT DEFINED LT_USB_DEVKIT_PATH)
    set(LT_USB_DEVKIT_PATH "/dev/ttyACM0" CACHE STRING "Path to the USB device representing the USB devkit serial port.")
endif()
message(STATUS "Using serial port: ${LT_USB_DEVKIT_PATH}. You can change it by passing -DLT_USB_DEVKIT_PATH=<path> to cmake.")

# Select pairing keys written during manufacturing into your TROPIC01
set(LT_SH0_KEYS "prod0" CACHE STRING "Choose which pairing keys in slot 0 will be used")
set_property(CACHE LT_SH0_KEYS PROPERTY STRINGS "eng_sample" "prod0")

# These are internal macros for selecting SH0 keys (examples/tests)
set(LT_USE_SH0_ENG_SAMPLE 0 CACHE INTERNAL "")
set(LT_USE_SH0_PROD0      0 CACHE INTERNAL "")

# Define SH0 macros based on selected string (examples/tests)
if(LT_SH0_KEYS STREQUAL "eng_sample")
    message(STATUS "Using Engineering sample keys in examples/tests")
    set(LT_USE_SH0_ENG_SAMPLE 1)
    set(LT_USE_SH0_PROD0      0)
elseif(LT_SH0_KEYS STREQUAL "prod0")
    message(STATUS "Using Production 0 keys in examples/tests")
    set(LT_USE_SH0_ENG_SAMPLE 0)
    set(LT_USE_SH0_PROD0      1)
else()
    get_property(lt_sh0_keys_choices CACHE LT_SH0_KEYS PROPERTY STRINGS)
    message(FATAL_ERROR "Incorrect SH0 keys for examples/tests specified: '${LT_SH0_KEYS}'\nAvailable SH0 keys: ${lt_sh0_keys_choices}")
endif()

###########################################################################
#                                                                         #
#   Libtropic                                                             #
#                                                                         #
###########################################################################

add_subdirectory(libtropic)
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

###########################################################################
#                                                                         #
#   External dependencies                                                 #
#                                                                         #
###########################################################################

# MbedTLS v4.0.0
find_package(MbedTLS QUIET)

if(MbedTLS_FOUND)
    target_link_libraries(tropic PUBLIC MbedTLS::mbedtls)
else()
    # fallback: pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MBEDTLS REQUIRED mbedtls)
    target_include_directories(tropic PUBLIC ${MBEDTLS_INCLUDE_DIRS})
    target_link_libraries(tropic PUBLIC ${MBEDTLS_LIBRARIES})
    target_link_directories(tropic PUBLIC ${MBEDTLS_LIBRARY_DIRS})
endif()

# Sodium
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

###########################################################################
#                                                                         #
#   CAL + HAL                                                             #
#                                                                         #
###########################################################################

# MbedTLS v4 Cryptographic Abstraction Layer
add_subdirectory(libtropic/cal/mbedtls_v4 "mbedtls_v4_cal")
target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

# POSIX USB dongle Hardware Abstraction Layer
add_subdirectory(libtropic/hal/posix/usb_dongle "posix_usb_dongle")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

###########################################################################
#                                                                         #
#   Application                                                           #
#                                                                         #
###########################################################################

set(PROJECT_SHARED_SOURCES
    src/device.cpp
	src/key.cpp
    src/pkcs11_module.cpp
)

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pkcs11
)

set(LT_COMMON_DEFS
    LT_USE_SH0_ENG_SAMPLE=${LT_USE_SH0_ENG_SAMPLE}
    LT_USE_SH0_PROD0=${LT_USE_SH0_PROD0}
    LT_USB_DEVKIT_PATH=\"${LT_USB_DEVKIT_PATH}\"
)

###########################################################################
#                                                                         #
#   Executable                                                            #
#                                                                         #
###########################################################################

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${PROJECT_SHARED_SOURCES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE_DIRS} ${SODIUM_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    OUTPUT_NAME "${PROJECT_NAME}"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${LT_COMMON_DEFS})
target_link_libraries(${PROJECT_NAME} PRIVATE tropic ${SODIUM_LIBRARIES})

###########################################################################
#                                                                         #
#   PKCS#11 shared library (for SSH / ssh-agent / PKCS11Provider)         #
#                                                                         #
###########################################################################

add_library(${PROJECT_NAME}_pkcs11 SHARED
    ${PROJECT_SHARED_SOURCES}
)
target_include_directories(${PROJECT_NAME}_pkcs11 PRIVATE ${PROJECT_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME}_pkcs11 PROPERTIES
    LINKER_LANGUAGE CXX
    PREFIX "" # produces tropikey_pkcs11.so, not libtropikey_pkcs11.so
    OUTPUT_NAME "${PROJECT_NAME}_pkcs11"
)

target_compile_definitions(${PROJECT_NAME}_pkcs11 PRIVATE ${LT_COMMON_DEFS})
target_link_libraries(${PROJECT_NAME}_pkcs11 PRIVATE tropic)
