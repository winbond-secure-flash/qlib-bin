# Makefile for building qlib_samples using the prebuilt libqlib.so library.
#
# The user MUST provide platform implementation files that implement
# PLAT_SPI_WriteReadTransaction, PLAT_HASH_*, PLAT_GetNONCE, CORE_RESET
# and any other required platform callbacks declared in qlib_platform.h.
# Place them in platform/ — the Makefile compiles all .c files there.
#
# To build with LMS support: make LMS=1

CC = gcc
# Mandatory flags:
#   -DQLIB_MAX_DIES_SUPPORTED=4  must be 4 DO NOT CHANGE this. must match the value used to build the supplied Qlib library
#   -Iinc                        QLIB public API headers
#   -Iinc/defs                   Winbond portability layer headers
CFLAGS = -O2 -Wall -Wextra \
  -DQLIB_MAX_DIES_SUPPORTED=4 \
  -Iinc \
  -Iinc/defs
LDFLAGS = -lqlib

# --- Flags needed only for building the sample application ---
# NOTE: QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE=SIZE_4MB_ is required when the flash
# has a boot (non-QLIB) section at section 0 (e.g. Renesas Sparrow Hawk / R-Car V4H).
# It tells the sample code to skip/reserve 4 MB at the start of the address space.
# Increase the size if your boot section is larger (e.g. SIZE_8MB_, SIZE_16MB_).
# If your platform has no reserved boot section, remove this define entirely.
$(warning NOTE: Building with QLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE=SIZE_4MB_, set size of your boot section if relevant.)
$(warning       Remove this define if your platform has no boot section.)
$(warning NOTE: Building with QLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION, required for CDI sample if boot section is relevant.)
$(warning       Remove this define if your platform has no boot section.)
QLIB_SAMPLES_FLAGS = \
  -Isamples \
  -DQLIB_SAMPLE_RESERVED_BOOT_SECTION_SIZE=SIZE_4MB_ \
  -DQLIB_SAMPLE_CDI_PRESERVE_BOOT_SECTION

CFLAGS += $(QLIB_SAMPLES_FLAGS)

OUTPUT = qlib_samples

# --- User platform implementation ---
# platform/ contains qlib_platform_stubs.c with stub implementations of all platform hooks.
# Replace the stubs with your platform's actual implementation.
# The wildcard lets test scripts remove stubs and add specific platform .c files.
PLATFORM_SRCS = $(wildcard platform/*.c)

SRCS = \
  test.c \
  samples/qlib_sample_run.c \
  samples/qlib_sample_fw.c \
  samples/qlib_sample_secure_storage.c \
  samples/qlib_sample_get_ids.c \
  samples/qlib_sample_fw_update.c \
  samples/qlib_sample_recovery.c \
  samples/qlib_sample_secure_log.c \
  samples/qlib_sample_vault.c \
  samples/qlib_sample_wd.c \
  samples/qlib_sample_safety.c \
  samples/qlib_sample_erase.c \
  samples/qlib_sample_integrity.c \
  samples/qlib_sample_direct_attestation.c \
  samples/qlib_sample_cdi.c \
  samples/qlib_utils_crc.c \
  samples/qlib_utils_digest.c \
  $(PLATFORM_SRCS)

# --- LMS support (optional) ---
# Build with: make LMS=1
ifdef LMS
  LMS_ROOT = crypto/lms

  CFLAGS += \
    -DQLIB_LMS_SUPPORT \
    -DUSE_LMS_EXT \
    -I$(LMS_ROOT)/inc \
    -I$(LMS_ROOT)/src \
    -I$(LMS_ROOT)/imports \
    -I$(LMS_ROOT)/winbond_stubs

  SRCS += \
    samples/qlib_sample_lms_common.c \
    samples/qlib_sample_lms_fw_update.c \
    samples/qlib_sample_lms_key_backup.c \
    samples/qlib_sample_lms_attestation.c \
    samples/qlib_utils_lms.c \
    $(LMS_ROOT)/src/lms.c \
    $(LMS_ROOT)/winbond_stubs/winbond_sha256.c \
    $(LMS_ROOT)/winbond_stubs/winbond_nonce.c \
    $(LMS_ROOT)/winbond_stubs/winbond_random.c \
    $(LMS_ROOT)/winbond_stubs/winbond_key_storage.c \
    $(LMS_ROOT)/winbond_stubs/winbond_key_storage_in_file.c \
    $(LMS_ROOT)/winbond_stubs/winbond_key_storage_in_ram.c \
    $(LMS_ROOT)/winbond_stubs/platform_sha256.c
endif

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OUTPUT)
