#include "SourceCode.hpp"

#include <cxxopts.hpp>

using namespace std;

int main(int argc, char** argv) {
    try {
        cxxopts::Options options("bin2cpp", "Convert a binary file into a C++ header");
        options.add_options()("i,input", "input file to convert", cxxopts::value<string>())(
          "o,output", "name of C++ file header to output", cxxopts::value<string>())(
          "c,columns", "max number of columns in the target file", cxxopts::value<size_t>()->default_value("160"))(
          "b,bitWidth", "output type size : 8, 16, 32, 64 (i.e. uint8_t, uint16_t, uint32_t, uint64_t)",
          cxxopts::value<size_t>()->default_value("8"))("s,selftest", "launch automated tests")("h,help", "Print usage");
        options.parse_positional({"input"});
        options.positional_help("inputFileName");
        auto result = options.parse(argc, argv);
        if ((argc == 1) || result.count("help")) {
            cout << options.help() << '\n';
            return 0;
        }

        auto columns = result["columns"].as<size_t>();
        auto bitWidth = result["bitWidth"].as<size_t>();

        if (result.count("selftest")) {
            if (result.count("input")) {
                filesystem::path inputFile{result["input"].as<string>()};
                SourceCode<ifstream, stringstream> code{inputFile, columns, bitWidth, inputFile.stem().string()};
                return code.selftest();
            }
            else {
                SourceCode<stringstream, stringstream> code{columns, bitWidth, "Test"};
                return code.selftest();
            }
        }

        else if (result.count("input")) {
            filesystem::path inputFile{result["input"].as<string>()}, outputFile;
            if (result.count("output"))
                outputFile = result["output"].as<string>();
            else
                outputFile = filesystem::path(inputFile).replace_extension("hpp");

        SourceCode<ifstream, ofstream> code{inputFile, outputFile, columns, bitWidth};
        code.generate();
    }
}
catch (const exception& e) {
    cerr << "Error : " << e.what() << "\n";
    return -1;
}

return 0;
}