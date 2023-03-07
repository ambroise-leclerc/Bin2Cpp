# Bin2Cpp

A command-line utility for converting a binary file to a C++ source code file.


[![Build Windows](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildWindows.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildWindows.yml)
[![Build Ubuntu](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinux.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinux.yml)
[![Ubuntu/Clang](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinuxClang.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinuxClang.yml)
[![Build MacOS](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildMacOS.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildMacOS.yml)


## Getting started
```bash
$ bin2cpp
bin2cpp : converts a binary file to a C++ source code file ( https://github.com/ambroise-leclerc/Bin2Cpp )

Usage:
  bin2cpp [OPTION...] inputFileName

  -o, --output arg    name of C++ file header to output
  -c, --columns arg   max number of columns in the target file (default:
                      160)
  -b, --bitWidth arg  output type size : 8, 16, 32, 64 (i.e. uint8_t,
                      uint16_t, uint32_t, uint64_t) (default: 8)
  -s, --selftest      launch automated tests
  -h, --help          Print usage
```

Example : conversion of an icon file to a C++ header with 128 columns and data contained in a uint64_t array :
```bash
$ bin2cpp favicon.ico -c 128 -b 64
  Code generated in file : "/home/leclerc/devel/Bin2Cpp/build/favicon.hpp"
```

A favicon.hpp file is generated : 
```cpp
#pragma once

#include <array>

struct favicon {
    constexpr size_t data_size{ 766 };
    static constexpr std::array<uint64_t, 96> data {
     0x0000010001002020, 0x100001000400e802, 0x0000160000002800, 0x0000200000004000, 0x0000010004000000, 0x0000000000000000,
     0x0000000000001000, 0x0000000000003834, 0x3200e09b1200ead9, 0xb0006c676200ae9f, 0x7500e6b24200524e, 0x4800e1a429009d7b,
     0x2f00bfbab900857e, 0x6f00fbf8ef00b887, 0x2000866d3800e8c2, 0x6d0063594600bb23, 0x0000000000000000, 0x0000000032bbb900,
     0x0000000000000000, 0x00000000009b2000, 0x0000000000000000, 0x000000000002a000, 0x000000006d8cc8d6, 0x0000000000036000,
     0x006000d111717111, 0x1d00000000000000, 0x00006c1777777777, 0x7110000000000000, 0x0006117777777777, 0x7777f00000000000,
     0x00f1777777777777, 0x77771f0000000000, 0x0017777171717177, 0x7777710000000000, 0x0117111717171717, 0x1717171000000060,
     0xf111711111111111, 0x1171111f00600000, 0xc111111111111111, 0x1111111100000600, 0x1111111217be7e1b, 0xe71111116000000d,
     0x111111eb7ebb111b, 0x21111111d000060d, 0x111111227b2b5112, 0x211111118060000c, 0x111115b5eb72211b, 0xb2251111c0000668,
     0x11111221b21eb11b, 0x2ee5111180600608, 0x11117be5b711be1b, 0xe1111111c060066d, 0x1111eb1221112b1b, 0xe7111111d660066f,
     0x1111be7be1117b5b, 0xbbbbb111f0600666, 0x1111111111111111, 0x1111111166600666, 0xd717171717171711, 0x7171771d66600f66,
     0x6777777177717777, 0x1777717666606666, 0x6377777777777777, 0x7777773666f006ff, 0xf685575777575777, 0x57775a6f6f6666f6,
     0xff3a575555757555, 0x5555affff6f066ff, 0x3f3fa55555555557, 0x555af33636f6f6ff, 0xf333334555555555, 0xea333333636faff3,
     0xf3333333a445444a, 0x3a33333ff664b36f, 0xf333a3aaaaaaaaaa, 0xaa3a3333f332b243, 0x363f333333333333, 0x33333fffaa2bbbb9,
     0x9999999999999999, 0x999999999bbbc000, 0x0003800000010000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
     0x0000000000000000, 0x0000000000000002, 0x1000001310000005, 0x0000002418000008, 0x9000004890000040, 0x500000905f800000,
     0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000800000008000, 0x0001e00000070000
    };
};
```




## Compilation
```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build . --configuration Release
```
