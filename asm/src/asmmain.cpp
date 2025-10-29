#include "assemble.hpp"
#include "writer.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    using namespace detvm;
    if (argc < 2) {
        std::cerr << "Usage: detasm <input.detasm> [output.dto]\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_path;

    if (argc >= 3) output_path = argv[2];
    else
        output_path = std::filesystem::path(input_path).replace_extension(".dto").string();

    try {

        // 1. Read assembly file
        std::ifstream infile(input_path);
        if (!infile.is_open()) {
            std::cerr << "Error: cannot open " << input_path << "\n";
            return 1;
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(infile, line)) {
            if (!line.empty()) lines.push_back(line);
        }

        auto result = assembler::assembleFirstPass(lines); // your assembler pipeline

       Writer::writeObject(output_path, result);
        std::cout << " Assembled " << input_path << " -> " << output_path << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << " Assembly failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
