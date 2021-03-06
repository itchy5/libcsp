cmake_minimum_required (VERSION 2.6)
project (csp)
include (CheckIncludeFiles)
set (CSP_VERSION "1.4.0" CACHE STRING "")

option (RDP "" OFF)
option (QOS "" OFF)
option (PROMISC "" OFF)
option (CRC32 "" OFF)
option (HMAC "" OFF)
option (XTEA "" OFF)
option (BINDINGS "" OFF)
option (EXAMPLES "" OFF)
option (DEDUP "" OFF)
option (VERBOSE "" OFF)
option (IF_I2C "" OFF)
option (IF_KISS "" OFF)
option (IF_CAN "" OFF)
option (IF_ZMQHUB "" OFF)
option (CSP_DEBUG "" ON)
option (CAN_SOCKETCAN "" OFF)

set (FREERTOS "" CACHE STRING "FreeRTOS root dir")
option (INIT_SHUTDOWN "" OFF)

#TODO FREERTOS
set (RDP_MAX_WINDOW "20" CACHE STRING "Set maximum window size for RDP")
set (MAX_BIND_PORT "31" CACHE STRING "Set maximum bindable port")
set (MAX_CONNECTIONS "10" CACHE STRING "Set maximum number of concurrent connections")
set (CONN_QUEUE_LENGTH "100" CACHE STRING "Set maximum number of packets in queue for a connection")
set (ROUTER_QUEUE_LENGTH "10" CACHE STRING "Set maximum number of packets to be queued at the input of the router")
set (PADDING "8" CACHE STRING "Set padding bytes before packet length field")
set (LOGLEVEL "debug" CACHE STRING "Set minimum compile time log level. Must be one of 'error', 'warn', 'info' or 'debug'")
set (RTABLE "static" CACHE STRING "Set routing table type")
set (CONNECTION_SO "0x0000" CACHE STRING "Set outgoing connection socket options, see csp.h for valid values")

execute_process (COMMAND git describe --always
                 OUTPUT_VARIABLE GIT_REV
                 OUTPUT_STRIP_TRAILING_WHITESPACE
                 ERROR_QUIET)

add_library (csp STATIC
             src/csp_bridge.c
             src/csp_buffer.c
             src/csp_conn.c
             $<$<BOOL:${CRC32}>:src/csp_crc32.c>
             $<$<BOOL:${CSP_DEBUG}>:src/csp_debug.c>
             $<$<BOOL:${DEDUP}>:src/csp_dedup.c>
             src/csp_endian.c
             src/csp_iflist.c
             src/csp_io.c
             src/csp_port.c
             src/csp_promisc.c
             src/csp_qfifo.c
             src/csp_route.c
             src/csp_service_handler.c
             src/csp_services.c
             src/csp_sfp.c
             $<$<BOOL:${HMAC}>:src/crypto/csp_hmac.c>
             $<$<OR:$<BOOL:${HMAC}>,$<BOOL:${XTEA}>>:src/crypto/csp_sha1.c>
             $<$<BOOL:${XTEA}>:src/crypto/csp_xtea.c>
             $<$<BOOL:${CAN_SOCKETCAN}>:src/drivers/can/can_socketcan.c>
             src/interfaces/csp_if_lo.c
             src/rtable/csp_rtable_${RTABLE}.c
             $<$<BOOL:${RDP}>:src/transport/csp_rdp.c>
             src/transport/csp_udp.c)

target_compile_definitions (csp PRIVATE GIT_REV=\"${GIT_REV}\")
target_include_directories (csp PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})

# TODO detect endianness
set (CSP_LITTLE_ENDIAN ON)
if (TARGET_LIKE_FREERTOS)
    set (CSP_FREERTOS ON)
    set (CSP_ARCH "freertos")
    target_include_directories (csp PUBLIC ${FREERTOS_INCLUDES})
elseif (TARGET_LIKE_LINUX)
    set (CSP_POSIX ON)
    set (CSP_ARCH "posix")
    target_link_libraries (csp PRIVATE rt pthread util)
elseif (TARGET_LIKE_OSX)
    set (CSP_MACOSX ON)
    set (CSP_ARCH "macosx")
    target_link_libraries (csp PRIVATE pthread)
else()
    message ("TARGET_OS current ${YOTTA_CFG_TARGET_OS}")
    message (FATAL ERROR "Set correct target")
endif()

if (TARGET_LIKE_MSP430_GCC)
    set (MAX_CONNECTIONS "2" CACHE STRING "Set maximum number of concurrent connections")
    set (CONN_QUEUE_LENGTH "10" CACHE STRING "Set maximum number of packets in queue for a connection")
    set (ROUTER_QUEUE_LENGTH "5" CACHE STRING "Set maximum number of packets to be queued at the input of the router")
endif()

file (GLOB_RECURSE ARCH_FILES src/arch/${CSP_ARCH}/*.c)
target_sources (csp PUBLIC ${ARCH_FILES})

target_compile_options(csp PUBLIC -O2 -Wall -g -std=gnu99)

if (LOGLEVEL STREQUAL "debug")
    set (CSP_LOG_LEVEL_DEBUG ON)
endif()

if (LOGLEVEL STREQUAL "debug" OR LOGLEVEL STREQUAL "info")
    set (CSP_LOG_LEVEL_INFO ON)
endif()

if (LOGLEVEL STREQUAL "debug" OR LOGLEVEL STREQUAL "info" OR
    LOGLEVEL STREQUAL "warn")
    set (CSP_LOG_LEVEL_WARN ON)
endif()

if (LOGLEVEL STREQUAL "debug" OR LOGLEVEL STREQUAL "info" OR
    LOGLEVEL STREQUAL "warn" OR LOGLEVEL STREQUAL "error")
    set (CSP_LOG_LEVEL_ERROR ON)
endif()

check_include_files (stdbool.h CSP_HAVE_STDBOOL_H)
configure_file (csp_autoconfig.h.in csp/csp_autoconfig.h @ONLY)
