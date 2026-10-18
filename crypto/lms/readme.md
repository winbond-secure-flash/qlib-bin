# LMS module
## Description
The following package includes the lms module.
The LMS module is SW implementation of the LMS cryptographic algorithm written in C using C99 standard
## Module content
- inc - lms external API
  - lms_api.h - lms module lms_api
- src - lms module source files:
  - lms.c - lms algorithm implementation
  - lms.h - lms definitions
  - lms_version.h - lms module version
- imports - platform functions provided by the user
  - nonce.h - Get nonce API
  - random.h - Get random API
  - sha256.h - SHA 256 API
  - key_storage.h - key storage API
- winbond_stubs - stub implementation of the import APIs
  - winbond_nonce.c - stub implementation of Get nonce API
  - winbond_random.c - stub implementation of Get random API
  - winbond_sha256.c - SW implementation of SHA 256
  - winbond_key_storage.c - sample implementation of key storage API
- cmake - cmake configuration file used to compile the module
  - lms_config.cmake - defines the files, includes and definitions needed for the compilation
- tests\compilation_tests - sample code with test vectors
  - main.c - test vectors code
- LICENSE - License Agreement
- README.md - this instructions
- CMakeLists.txt - simple cmake script to compile the module
## Usage
In order to use the module, some platfrom specific functions are needed.
These functions are defined in imports directory and include nonce, sha256, random and key storage functions.
We provide stub implementation of these functions (located under winbond_stubs directory) for compilation and initial testing.
User must override these functions with its own implementation.

In order to compile the module without the private operations user should define the VERIFY_ONLY definition.

After platrform specific functions were provided, compile the module using cmake and ninja and the following commands:

## Windows Test Script

```batch
@include "test_release.bat"
```

## Linux Test Script

```sh
@include "test_release.sh"
```
