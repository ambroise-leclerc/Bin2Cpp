#include <array>
#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <version>

#if __has_include(<format>)
#include <format>
#else
#include <fmt/core.h>
using fmt::format;
#endif

template<typename InputStream, typename OutputStream>
class SourceCode {
    std::string structName, valueType;
    size_t maxColumns, bytesPerValue;
    InputStream src;
    OutputStream dst;

public:
    SourceCode(size_t maxColumns, size_t outputBitwidth, std::string structName)
        : structName(std::move(structName)), maxColumns(maxColumns) {
        setOutputBitwidth(outputBitwidth);
    }

    SourceCode(std::filesystem::path input, std::filesystem::path output, size_t maxColumns, size_t outputBitwidth)
        : SourceCode(maxColumns, outputBitwidth, output.stem().string()) {
        src.open(input, std::ios::binary);
        dst.open(output);
    }

    SourceCode(std::filesystem::path input, size_t maxColumns, size_t outputBitwidth, std::string structName)
        : SourceCode(maxColumns, outputBitwidth, std::move(structName)) {
        src.open(input, std::ios::binary);
    }

    /**
     * @brief Generate a C++ file from the configured input
     * 
     */
    void generate() {
        size_t bytesRead{0}, columns{4};
        const size_t sepWidth{4}, firstColumn{4};
        uint64_t outValue{0};

        dst << "#pragma once\n\n";
        dst << "#include <array>\n\n";
        dst << "struct " << structName << " {\n";
        dst << "    static constexpr std::array<" << valueType << "> data {\n    ";
        dst << std::hex << std::setfill('0');
        while (src) {
            char c;
            src.get(c);
            if (src) {
                outValue = (outValue << 8) + static_cast<uint8_t>(c);
                bytesRead++;
                if (bytesRead % bytesPerValue == 0) {
                    columns += sepWidth + bytesPerValue * 2;
                    if (bytesRead > bytesPerValue) dst << ",";
                    if (columns > maxColumns) {
                        dst << "\n    ";
                        columns = firstColumn + sepWidth + bytesPerValue * 2;
                    }
                    dst << " 0x" << std::setw(bytesPerValue * 2) << outValue;
                    outValue = 0;
                }
            }
        }
        auto paddingBytes = bytesRead % bytesPerValue;
        if (paddingBytes > 0) dst << ", 0x" << std::setw(bytesPerValue * 2) << (outValue << 8 * paddingBytes);
        dst << "\n    };\n    constexpr size_t data_size{" << std::dec << bytesRead << "};\n";
    }

    /**
     * @brief Launch a serie of tests with the configured options and input of SourceCode.
     * 
     * If InputStream is of std::stringstream type, an internal data stream will be used.
     */
    int selftest() {
        using namespace std;

        if constexpr (!is_same_v<decltype(src), ifstream>) src << "File to encode\n";
        generate();
        src.clear();
        src.seekg(0);
        string line;
        while (getline(dst, line)) {
            if (auto intType = line.find("std::array<uint"); intType != string::npos) {
                auto bytesCount = stoul(line.substr(intType + 15)) >> 3;
                while (getline(dst, line), line != "    };") {
                    size_t startPos = 0, nextPos = 0;
                    while (startPos < line.size()) {
                        uint64_t value = stoull(line.substr(startPos), &nextPos, 16);
                        startPos += nextPos + 1;

                        array<uint8_t, 8> data;
                        memcpy(data.data(), &value, bytesCount);
                        for (size_t index = 0; index < bytesCount; ++index) {
                            auto byte = static_cast<uint8_t>(src.get());
                            size_t readIndex = endian::native == endian::big ? index : bytesCount - index - 1;
                            REQUIRE(data[readIndex] == byte, format("data[{}] = {} == {}", readIndex, data[readIndex], byte));
                        }
                    }
                }
            }
        }
        cout << "Tests results :\n";
        cout << "Total tests : " << dec << testsCount << "  passed : " << testsCount - failedTestsCount
             << "  failed : " << failedTestsCount << "\n";
        return failedTestsCount > 0 ? -1 : 0;
    }

private:
    int testsCount{0}, failedTestsCount{0};

private:
    void setOutputBitwidth(size_t bits) {
        if (bits != 16 and bits != 32 and bits != 64) bits = 8;
        valueType = "uint" + std::to_string(bits) + "_t";
        bytesPerValue = bits >> 3;
    }

    void REQUIRE(bool passed, std::string legend = {}) {
        testsCount++;
        if (!passed) {
            std::cerr << "Test #" << testsCount << " failed : " << legend << "\n";
            failedTestsCount++;
        }
    }
};