# Bin2Cpp

A command-line utility for converting a binary file to a C++ source code file.


[![Build Windows](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildWindows.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildWindows.yml)
[![Build Ubuntu](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinux.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinux.yml)
[![Ubuntu/Clang](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinuxClang.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinuxClang.yml)
[![Build MacOS](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildMacOS.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildMacOS.yml)
[![codecov](https://codecov.io/github/ambroise-leclerc/Bin2Cpp/branch/main/graph/badge.svg?token=VMCG69MS5A)](https://codecov.io/github/ambroise-leclerc/Bin2Cpp)
[![SonarCloud](https://sonarcloud.io/api/project_badges/measure?project=ambroise-leclerc_Bin2Cpp&metric=alert_status)](https://sonarcloud.io/dashboard?id=ambroise-leclerc_Bin2Cpp)

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
      --withDecoder   add a filestream-like interface with endian-aware
                      read() and get() functions for multi-bytes outputs
  -s, --selftest      launch automated tests
  -h, --help          Print usage
```

Example : conversion of an icon file to a C++ header with 128 columns and data contained in a uint64_t array :
```bash
$ bin2cpp favicon.ico -c 128 -b 64
  Code generated in file : "/home/leclerc/devel/Bin2Cpp/build/favicon.ico.hpp"
```

A favicon.ico.hpp file is generated : 
```cpp
#pragma once

#include <array>

struct FaviconIco {
    static constexpr size_t dataSize{ 766 };
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

Example : conversion with a compressed file
```bash
$ bin2cpp favicon.ico.gz -c 128 -b 64
  Code generated in file : "/home/leclerc/devel/Bin2Cpp/build/favicon.ico.gz.hpp"
```

A favicon.ico.gz.hpp file is generated with an "encoding" field describing the compression format ("gzip" or "br"): 
```cpp
#pragma once

#include <array>

struct FaviconIcoGz {
    static constexpr std::string_view encoding {"gzip"};
    static constexpr size_t dataSize{ 522 };
    static constexpr std::array<uint64_t, 66> data {
     0x1f8b0808a6fd4664, 0x000366617669636f, 0x6e2e69636f008d52, 0x4f68d36014ffb56a, 0xdc4171afa211c25c, 0xea1f500a0a8d94b1,
     0x4b863a191e448550, 0x9c07831ddf3cac84, 0x08358740e9657af1, 0xb0ab1451705e7670, 0xae225a03b57af2a8, 0x9789b7f5301ddbc1,
     0x8e40103cc497a4ce, 0xced37e5f7e79eff7, 0xbeefbdef7df902a4, 0x78a8ea20bf77632d, 0x0d1c06708aa932c7, 0x9851fc7f0cf6ecc8,
     0xb93c56ea19ac7f7b, 0x8999e9db78f1a482, 0xef8df3b87e65029d, 0xf9d378ec9ec5fb77, 0x4dcc562dfcfef513, 0x6f1fa8b85f1ec1da,
     0xc7324a372ec13bde, 0x5f35ef35b76f5357, 0xb7ebf4d3d8941f7e, 0x5a8e9d5d2660e20b, 0xd9360d6d2d9a911d, 0x86fdb745899c04dd,
     0x446ff6a4339c68d9, 0xb11951462c5332c9, 0x3d4415cc4db22986, 0x4dc3bc173ed03f70, 0x71b0c9c8adaaf283, 0x1d9edecf66a3ea91,
     0x9265e733a4481f73, 0x73d73291ae99d847, 0x74e8f5c6ddacd238, 0x49d4867487d3b38d, 0xa34bca99d5685e1a, 0x207257df504be9f0,
     0xfab62995b91ce7e6, 0xe2facba66411b5dc, 0x0eb9373d6f89baa6, 0x24fada21c1fa6bd2, 0x3b7f1267489807c4, 0x341f8c87c3c7bc27,
     0x4c214ace160aa22b, 0x85c16cb1e8448f33, 0x69594204e168d130, 0x2a15c33016c3a02b, 0x425d7fcebe513426, 0xfd422108425fd3c6,
     0xa3c0baa669256bd1, 0x67adcd8f5fbc3caa, 0x697a30f5caf2b567, 0x0b3138e0e71b170a, 0xbad6831e2ee4bce6, 0xa33ed4bd36df612d,
     0xfac5768a345fca41, 0xe61ef64f1c0106e6, 0x8009e6d85560ee56, 0x6d27256a09532bc0, 0xde3f23cd980cfe02, 0x0000000000000000
    };
};
```


## Compilation
```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build . --config Release
```

## Test
```bash
$ ctest -C Release
```
