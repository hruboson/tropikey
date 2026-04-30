cmake_minimum_required(VERSION 3.21.0)
include(FetchContent)

project(tropikey LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
set(ENABLE_TESTING OFF CACHE BOOL "Disable mbedtls_v4 test building.")
set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable mbedtls_v4 examples building.")
FetchContent_Declare(
    mbedtls_v4
    URL https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-4.0.0/mbedtls-4.0.0.tar.bz2
    URL_HASH SHA256=2f3a47f7b3a541ddef450e4867eeecb7ce2ef7776093f3a11d6d43ead6bf2827
)
FetchContent_MakeAvailable(mbedtls_v4)
target_link_libraries(tropic PUBLIC mbedtls)

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

add_executable(${PROJECT_NAME}
    src/main.cpp
	src/device.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    LT_USB_DEVKIT_PATH=\"${LT_USB_DEVKIT_PATH}\"
)

target_link_libraries(${PROJECT_NAME} PRIVATE tropic)
