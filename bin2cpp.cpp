#include <cxxopts.hpp>
//#include <doctest/doctest.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

using namespace std;

template<typename InputStream, typename OutputStream>
class SourceCode {
    string structName, valueType = "uint8_t";
    size_t maxColumns = 160;
    size_t bytesPerValue = 1;
    InputStream src;
    OutputStream dst;
public:
    SourceCode(string structName, size_t maxColumns, size_t outputBitwidth) : structName(structName), maxColumns(maxColumns) {
        setOutputBitwidth(outputBitwidth);
    }

    SourceCode(filesystem::path input, filesystem::path output, size_t maxColumns, size_t outputBitwidth)
     : SourceCode(filesystem::path(output).replace_extension().string(), maxColumns, outputBitwidth) {
        src.open(input, ios::binary);
        dst.open(output);
     }

    void setOutputBitwidth(size_t bits) {
        if (bits != 16 and bits != 32 and bits != 64) bits = 8;
        valueType = "uint" + to_string(bits) + "_t";
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
        dst << hex << setfill('0');
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
                    dst << " 0x" << setw(bytesPerValue * 2) << outValue;
                    outValue = 0;
                }
            }
        }
        auto paddingBytes = bytesRead % bytesPerValue;
        if (paddingBytes > 0) dst << ", 0x" << setw(bytesPerValue * 2) << (outValue<<8*paddingBytes) << "\n";
        dst << "    }\n    constexpr size_t data_size{" << dec << bytesRead << "};\n";
    }

    void selftest() {

    }
};

int main(int argc, char** argv) {
    cxxopts::Options options("bin2cpp", "Convert a binary file into a C++ header");
    options.add_options()
        ("i,input", "input file to convert", cxxopts::value<string>())
        ("o,output", "name of C++ file header to output", cxxopts::value<string>())
        ("c,columns", "max number of columns in the target file", cxxopts::value<size_t>()->default_value("160"))
        ("b,bitWidth", "output type size : 8, 16, 32, 64 (i.e. uint8_t, uint16_t, uint32_t, uint64_t)", cxxopts::value<size_t>()->default_value("8"))
        ("s,selftest", "launch automated tests")
        ("h,help", "Print usage");
    options.parse_positional({"input"});
    options.positional_help("inputFileName");
    auto result = options.parse(argc, argv);
    if ((argc == 1) || result.count("help")) {
        cout << options.help() << '\n';
        return 0;
    }

    if (result.count("input")) {
        filesystem::path inputFile{result["input"].as<string>()};
        filesystem::path outputFile;
        if (result.count("output")) 
            outputFile = result["output"].as<string>();
        else
            outputFile = filesystem::path(inputFile).replace_extension("hpp");
        SourceCode<ifstream, ofstream> code{inputFile, outputFile, result["columns"].as<size_t>(), result["bitWidth"].as<size_t>()};
        code.generate();
    }

    if (result.count("selftest")) {
        SourceCode<ifstream, ofstream> code("Test", 160, 64);
        code.selftest();
    }

    return 0;
}