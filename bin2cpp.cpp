/// @date 2023/03/07 17:27:42
/// @author Ambroise Leclerc
/// @brief Command-line utility which converts a binary file to a C++ source code file
///
/// This file is part of Bin2Cpp.
/// Bin2Cpp is free software : you can redistribute it and or modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation, either version 3 of the License, or any later version.
/// Bin2Cpp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
/// You should have received a copy of the GNU General Public License along with Bin2Cpp. If not, see
/// <https://www.gnu.org/licenses/>.
#include "SourceCode.hpp"

#include <cxxopts.hpp>

#include <stdexcept>

using namespace std;

int main(int argc, char** argv) {
    try {
        cxxopts::Options options(
          "bin2cpp",
          "bin2cpp : converts a binary file to a C++ source code file ( https://github.com/ambroise-leclerc/Bin2Cpp )\n");
        options.add_options()("i,input", "input file to convert", cxxopts::value<string>())(
          "o,output", "name of C++ file header to output", cxxopts::value<string>())(
          "c,columns", "max number of columns in the target file", cxxopts::value<size_t>()->default_value("160"))(
          "b,bitWidth", "output type size : 8, 16, 32, 64 (i.e. uint8_t, uint16_t, uint32_t, uint64_t)", cxxopts::value<size_t>()->default_value("8"))(
          "withDecoder", "add a filestream-like interface with endian-aware read() and get() functions for multi-bytes outputs")(
          "s,selftest", "launch automated tests")("h,help", "Print usage");
        options.parse_positional({"input"});
        options.positional_help("inputFileName");
        auto result = options.parse(argc, argv);
        if ((argc == 1) || result.count("help")) {
            cout << options.help() << '\n';
            return 0;
        }

        auto columns = result["columns"].as<size_t>();
        auto bitWidth = result["bitWidth"].as<size_t>();

        auto testDecoderOption = [&result, bitWidth](auto& code) {
            if (result.count("withDecoder")) {
                if (bitWidth == 8)
                    throw invalid_argument("no decoder can be generated for 8 bit width output");
                else
                    code.setOptionalDecoder();
            }
        };

        if (result.count("selftest")) {
            if (result.count("input")) {
                filesystem::path inputFile{result["input"].as<string>()};
                SourceCode<ifstream, stringstream> code{inputFile, columns, bitWidth, inputFile.stem().string()};
                testDecoderOption(code);
                return code.selftest();
            }
            else {
                SourceCode<stringstream, stringstream> code{columns, bitWidth, "Test"};
                testDecoderOption(code);
                return code.selftest();
            }
        }

        else if (result.count("input")) {
            filesystem::path inputFile{result["input"].as<string>()}, outputFile;
            if (result.count("output"))
                outputFile = result["output"].as<string>();
            else
                outputFile = filesystem::path(inputFile.filename()).replace_extension("hpp");

            SourceCode<ifstream, ofstream> code{inputFile, outputFile, columns, bitWidth};
            testDecoderOption(code);
            code.encodeSourceToCpp();
        }
    }
    catch (const exception& e) {
        cerr << "Error : " << e.what() << "\n";
        return -1;
    }

    return 0;
}