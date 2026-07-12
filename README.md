# QLIB Library Release

Prebuilt shared library for the Winbond W77Q/W77T secure flash family.

## Package Contents

| Directory   | Description |
|-------------|-------------|
| `inc/`      | Public API headers (`qlib.h`, `qlib_platform.h`, etc.) |
| `bin/`      | Prebuilt `libqlib.so` per platform, `install.sh`, and per-platform `user_info.md` |
| `platform/` | Stub platform implementations (`qlib_platform_stubs.c`) — replace with your hardware port |
| `samples/`  | Sample source code demonstrating QLIB features |
| `docs/`     | API reference documentation (HTML) |
| `lms/`      | LMS library (if included) |

## Key API Headers

| Header | Description |
|:-------|:------------|
| `qlib.h` | Main QLIB API — initialization, session management, secure read/write/erase, FW update, attestation, watchdog, etc. |
| `qlib_platform.h` | Platform callback declarations — SPI transaction, SHA-256, nonce, and optional macros you must implement |
| `qlib_types.h` | Type definitions — contexts, keys, bus modes, section descriptors, etc. |
| `qlib_errors.h` | Error codes returned by all QLIB functions |
| `qlib_safety.h` | Safety-related API — safe read, write, and erase with CRC verification |
| `qlib_targets.h` | Supported W77Q / W77T target device definitions |

## Quick Start

1. Install the library on your board:

   ```sh
   chmod +x bin/install.sh
   bin/install.sh
   ```

   On a Linux PC you may need to run with `sudo`:

   ```sh
   chmod +x bin/install.sh
   sudo bin/install.sh
   ```

   The script detects the board architecture, copies `libqlib.so` to `/usr/lib`,
   and refreshes the library cache.

   Expected output:

   ```
   Checking installation environment...
   Installing libqlib.so (aarch64) to /usr/lib...
   Success: Library cache updated.
   Done.
   ```

2. Implement the mandatory platform functions declared in `inc/qlib_platform.h`:

   - **PLAT_SPI_WriteReadTransaction** — SPI master read-write transaction. Ensure the function generates correct waveforms as described in the function documentation. The precompiled library uses `QLIB_MAX_SPI_INPUT_SIZE` = 256 bytes; the library splits large transfers internally, so your implementation must support input sizes of up to 256 bytes.

   - **CORE_RESET** — Resets the core CPU.

   - **PLAT_HASH_Init / PLAT_HASH_Update / PLAT_HASH_Finish** — SHA-256 hash implementation. You must provide your own implementation (e.g. OpenSSL or hardware-accelerated).

   - **PLAT_GetNONCE** — Returns a unique, non-repeating random number each time it is called. Use a hardware-based TRNG for high entropy.

   > **Note:** `platform/qlib_platform_stubs.c` provides stub implementations of all mandatory platform callbacks. Replace them with your platform's actual implementation.

3. Build using `make` from the root of the deliverable package (see below).

## Initialize QLIB

```c
#include "qlib.h"

QLIB_CONTEXT_T qlibContext;
QLIB_STATUS_T  status;

// 1. Initialize the QLIB context (call once at startup)
status = QLIB_InitLib(&qlibContext);
if (QLIB_STATUS__OK != status) { /* handle error */ }

// 2. Store your SPI user-data pointer in the context (retrieved in platform callbacks)
QLIB_SetUserData(&qlibContext, &mySpiUserData);

// 3. Take ownership of the SPI bus / communication channel
status = QLIB_Connect(&qlibContext);
if (QLIB_STATUS__OK != status) { /* handle error */ }

// 4. Detect and initialize the flash device (SPI single mode, no DTR (Double Transfer Rate))
status = QLIB_InitDevice(&qlibContext, QLIB_BUS_FORMAT(QLIB_BUS_MODE_1_1_1, false));
if (QLIB_STATUS__OK != status) { /* handle error */ }

// ... use QLIB APIs here ...

// 5. Release the SPI bus when done
status = QLIB_Disconnect(&qlibContext);
if (QLIB_STATUS__OK != status) { /* handle error */ }
```

Refer to the `samples/` directory for complete usage examples.

## Building the Samples

The root directory contains a Makefile for building the sample application.

Build from the root of the deliverable package:

   ```sh
   make
   ```

By default, this will build the `qlib_samples` executable in the current directory.

> **Note:** `test.c` provides stub implementations of `QLIB_SAMPLE_GetFlashHandle` and
> `QLIB_SAMPLE_ReleaseFlashHandle`, which handle SPI interface setup and teardown for the
> sample application. Replace the stubs with your platform's actual implementation.

## Running the Samples

   ```sh
   ./qlib_samples ids
   ./qlib_samples fw_update
   ./qlib_samples secure_storage
   ./qlib_samples vault
   ./qlib_samples watchdog
   ./qlib_samples log
   ./qlib_samples safety
   ./qlib_samples erase
   ./qlib_samples dice_attest
   ./qlib_samples direct_attest
   ./qlib_samples lms_fw_update    # LMS builds only
   ./qlib_samples lms_key_backup   # LMS builds only
   ./qlib_samples lms_attest       # LMS builds only
   ```

   Expected output for a passing sample:

   ```
   QLIB sample 'ids' PASSED
   ```

## License

See [License.txt](License.txt).

---

Copyright (c) 2026 Winbond Electronics Corporation. All Rights Reserved.
