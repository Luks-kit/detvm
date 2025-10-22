#include "assemble.hpp"
#include "linker.hpp"
#include "helpers.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    using namespace detvm;

    if (argc < 3) {
        std::cerr << "Usage: detasm <input.detasm> <output.detbc>\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_path = argv[2];

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

        // 2. Assemble (first pass)
        auto result = assembler::assembleFirstPass(lines);

        // 3. Link (resolve labels/functions)
        linker::linkLabels(result.code, result.label_to_pc, result.unresolved, result.funcs);

        // 4. Write to binary
        std::ofstream out(output_path, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "Error: cannot write to " << output_path << "\n";
            return 1;
        }

        for (const auto& inst : result.code) {
            out.write(reinterpret_cast<const char*>(&inst.opcode), sizeof(uint8_t));
            out.write(reinterpret_cast<const char*>(&inst.a), sizeof(uint8_t));
            out.write(reinterpret_cast<const char*>(&inst.b), sizeof(uint8_t));
            out.write(reinterpret_cast<const char*>(&inst.c), sizeof(uint8_t));
        }

        std::cout << "[detasm] Assembled " << result.code.size()
                  << " instructions -> " << output_path << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Assembler error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
