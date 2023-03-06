#include <array>
#include <bit>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

template<typename InputStream, typename OutputStream>
class SourceCode {
    std::string structName, valueType = "uint8_t";
    size_t maxColumns = 160;
    size_t bytesPerValue = 1;
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

    void setOutputBitwidth(size_t bits) {
        if (bits != 16 and bits != 32 and bits != 64) bits = 8;
        valueType = "uint" + std::to_string(bits) + "_t";
        bytesPerValue = bits >> 3;
    }

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

    void selftest() {
        using namespace std;

        if constexpr (!is_same_v<decltype(src), ifstream>) src << "File to encode\n";
        generate();
        src.clear();
        src.seekg(0);
        cout << "Test output: \n" << dst.str() << "\n";
        string line;
        while (getline(dst, line)) {
            if (auto intType = line.find("std::array<uint"); intType != string::npos) {
                auto bytesCount = stoi(line.substr(intType+15)) / 8;
                while (getline(dst, line), line != "    };") {
                    size_t startPos = 0, nextPos = 0;
                    while (startPos < line.size()) {
                        uint64_t value = stoull(line.substr(startPos), &nextPos, 16);
                        cout << "value : " << hex << value << " - ";
                        startPos += nextPos + 1;

                        array<uint8_t, 8> data;
                        memcpy(data.data(), &value, bytesCount);
                        cout << "value : " << value << "  ->  data : ";
                        for (size_t index = 0; index < bytesCount; ++index)
                            cout << +data[index] << " ";
                        cout << "  (bytesCount : " << bytesCount << ")\n";
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
    }

private:
    int testsCount{0}, failedTestsCount{0};

private:
    void REQUIRE(bool passed, std::string legend = {}) {
        testsCount++;
        if (!passed) {
            std::cerr << "Test #" << testsCount << " failed : " << legend << "\n";
            failedTestsCount++;
        }
    }
};