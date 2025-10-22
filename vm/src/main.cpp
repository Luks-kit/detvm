#include "detvm.hpp"
#include "ops.hpp"
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    using namespace detvm;

    if (argc < 2) {
        std::cerr << "Usage: vm <input.detbc>\n";
        return 1;
    }

    std::string filename = argv[1];

    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }

    std::vector<Instruction> code;
    Instruction inst;

    while (in.read(reinterpret_cast<char*>(&inst.opcode), sizeof(uint8_t))) {
        in.read(reinterpret_cast<char*>(&inst.a), sizeof(uint8_t));
        in.read(reinterpret_cast<char*>(&inst.b), sizeof(uint8_t));
        in.read(reinterpret_cast<char*>(&inst.c), sizeof(uint8_t));
        code.push_back(inst);
    }

    std::cout << "[vm] Loaded " << code.size() << " instructions.\n";

    VM vm;
    vm.code = std::move(code);
    vm.regs.resize(6); // base register count
    vm.run();

    std::cout << "[vm] Execution complete.\n";
    return 0;
}
