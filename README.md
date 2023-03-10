# Bin2Cpp

A command-line utility for converting a binary file to a C++ sourcecode file.


[![Build Windows](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildWindows.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildWindows.yml)
[![Build Ubuntu](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinux.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinux.yml)
[![Ubuntu/Clang](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinuxClang.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildLinuxClang.yml)
[![Build MacOS](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildMacOS.yml/badge.svg)](https://github.com/ambroise-leclerc/Bin2Cpp/actions/workflows/BuildMacOS.yml)
[![codecov](https://codecov.io/github/ambroise-leclerc/Bin2Cpp/branch/main/graph/badge.svg?token=VMCG69MS5A)](https://codecov.io/github/ambroise-leclerc/Bin2Cpp)

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

Example : conversion of an icon file in a C++ header with 128 columns and data contained in a uint64_t array :
```bash
$ bin2cpp favicon.ico -c 128 -b 64
```



## Compilation
```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build . --configuration Release
```
