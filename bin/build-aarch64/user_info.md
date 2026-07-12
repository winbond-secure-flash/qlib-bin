# Build Environment

| Item             | Value                                                             |
|------------------|-------------------------------------------------------------------|
| Architecture     | AArch64                                                           |
| OS               | Ubuntu 24.04 LTS (WSL2)                                           |
| Cross-compiler   | LLVM/Clang (Hikari) — aarch64-poky-linux (Yocto SDK)                                                    |
| Yocto SDK        | poky 5.0.14                                                      |

# Library Compatibility

| Property       | Value            |
|----------------|------------------|
| Architecture   | AArch64    |
| Class          | ELF64      |
| Byte Order     | Little Endian     |
| OS/ABI         | UNIX - System V      |
| Min GLIBC      | 2.17  |

# Library Size

- Code (text): 102688 bytes
- Data: 688 bytes

# Shared Library Dependencies

- `libc.so.6`

# Tested Environment

| Item             | Value                              |
|------------------|------------------------------------|  
| Board            | Renesas R-Car V4H (Sparrow Hawk)   |
