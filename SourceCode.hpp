/// @date 2023/03/07 17:27:42
/// @author Ambroise Leclerc
/// @brief Encodes binary file into C++ source code
///
/// This file is part of Bin2Cpp.
/// Bin2Cpp is free software : you can redistribute it and or modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation, either version 3 of the License, or any later version.
/// Bin2Cpp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
/// You should have received a copy of the GNU General Public License along with Bin2Cpp. If not, see <https://www.gnu.org/licenses/>.
#pragma once

#include <array>
#include <bit>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <utility>

#if __has_include(<fmt/core.h>)
#include <fmt/core.h>
using fmt::format;
#else
#include <format>
#endif

template<typename InputStream, typename OutputStream>
class SourceCode {
    std::string structName, valueType;
    size_t maxColumns, bytesPerValue;
    InputStream src;
    OutputStream dst;

    bool optionalDecoder{false};

public:
    SourceCode(size_t maxColumns, size_t outputBitwidth, std::string structName)
        : structName(std::move(structName)), maxColumns(maxColumns) {
        setOutputBitwidth(outputBitwidth);
    }

    SourceCode(std::filesystem::path input, std::filesystem::path output, size_t maxColumns, size_t outputBitwidth)
        : SourceCode(maxColumns, outputBitwidth, output.stem().string()) {
        src.open(input, std::ios::binary);
        dst.open(output);
        std::cout << "Code will be generated in file : " << std::filesystem::absolute(output) << "\n";
    }

    SourceCode(std::filesystem::path input, size_t maxColumns, size_t outputBitwidth, std::string structName)
        : SourceCode(maxColumns, outputBitwidth, std::move(structName)) {
        src.open(input, std::ios::binary);
    }

    void setOptionalDecoder(bool set = true) { optionalDecoder = set; }

    /**
     * @brief Generate a C++ file from the configured input
     *
     */
    void encodeSourceToCpp() {
        size_t bytesRead{0}, columns{4};
        const size_t sepWidth{4}, firstColumn{4};
        uint64_t outValue{0};

        dst << "#pragma once\n\n";
        dst << "#include <array>\n";
        if (optionalDecoder)
            dst << "#include <bit>\n#include <cstring>\n";
        dst << "\nstruct " << structName << " {\n";

        auto fileSize = src.tellg();
        src.seekg(0, std::ios::end);
        fileSize = src.tellg() - fileSize;
        src.clear();
        src.seekg(0);
        dst << "    static constexpr size_t data_size{ " << fileSize << " };\n";
        dst << "    static constexpr std::array<" << valueType << ", " << 1 + fileSize / bytesPerValue << "> data {\n    ";
        dst << std::hex << std::setfill('0');
        while (src) {
            auto byte = static_cast<uint8_t>(src.get());
            if (!src.eof()) {
                outValue = (outValue << 8) + byte;
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
        auto paddingBits = ((bytesPerValue - (bytesRead % bytesPerValue)) % bytesPerValue) * 8;
        if (paddingBits > 0) dst << ", 0x" << std::setw(bytesPerValue * 2) << (outValue << paddingBits);
        dst << "\n    };\n";
        if (optionalDecoder) dst << generateDecoder(bytesPerValue);
        dst << "\n};";
    }

    /**
     * @brief Launch a serie of tests with the configured options and input of SourceCode.
     *
     * If InputStream is of std::stringstream type, an internal data stream will be used.
     */
    int selftest() {
        using namespace std;

        if constexpr (!is_same_v<decltype(src), ifstream>) generateRandomSourceData();

        encodeSourceToCpp();

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
                        REQUIRE(!src.eof(), "Source file EOF before data");
                        for (size_t index = 0; index < bytesCount; ++index) {
                            auto byte = static_cast<uint8_t>(src.get());
                            if (!src.eof()) {
                                size_t readIndex = endian::native == endian::big ? index : bytesCount - index - 1;
                                REQUIRE(data[readIndex] == byte,
                                        format("data[{}] = {} == {}", readIndex, data[readIndex], byte));
                            }
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

    void generateRandomSourceData() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(-128, 127);

        size_t inputSize = gen() % 32768;
        for (auto bytes = 0u; bytes < inputSize; ++bytes) src.put(static_cast<char>(distrib(gen)));
    }

    void REQUIRE(bool passed, std::string legend = {}) {
        testsCount++;
        if (!passed) {
            std::cerr << "Test #" << testsCount << " failed : " << legend << "\n";
            failedTestsCount++;
        }
    }

    static std::string generateDecoder(size_t bytes) {
        auto width = std::to_string(bytes);
        return "\n\
    struct istream {\n\
        size_t readIndex, lastReadCount;\n\
        bool eofBit, badBit;\n\
\n\
        size_t tellg() const { return readIndex; }\n\
        istream& seekg(size_t pos) { readIndex = pos; return *this; }\n\
        size_t gcount() const { return lastReadCount; }\n\
        bool eof() const { return eofBit; }\n\
        bool bad() const { return badBit; }\n\
        bool fail() const { return false; }\n\
        void clear() { eofBit = false; badBit = false; }\n\
        explicit operator bool() const { return !fail(); }\n\
        bool operator!() const { return eof() || bad(); }\n\
    };\n\
\n\
    static istream& read(char* s, size_t count) { return get(s, count); }\n\
    static istream& get(char* s, size_t count) {\n\
        static istream stream{.readIndex = 0};\n\
        static std::array<char, " + width +"> chunk;\n\
\n\
        stream.lastReadCount = 0;\n\
        if (stream.eof()) return stream;\n\
        for (size_t index = 0; index < count; ++index) {\n\
            if (stream.eof()) {\n\
                stream.badBit = true;\n\
                stream.lastReadCount = index;\n\
                return stream;\n\
            }\n\
            if (stream.readIndex % " + width + " == 0) memcpy(chunk.data(), &data[stream.readIndex / " + width + "], " + width + ");\n\
            size_t readIndex = std::endian::native == std::endian::big ? stream.readIndex % " + width + " : " + width + " - 1 - (stream.readIndex % " + width + ");\n\
            s[index] = chunk[readIndex];\n\
            \n\
            stream.readIndex++;\n\
            if (stream.readIndex == data_size) stream.eofBit = true;\n\
        }\n\
        stream.lastReadCount = count;\n\
        return stream;\n\
    }";
}
};