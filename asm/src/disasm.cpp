#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "ops.hpp"   // for detvm::Opcode enum
#include "detvm.hpp" // for Instruction definition

using namespace detvm;

static std::string opcodeToString(uint8_t op) {
    switch (static_cast<Opcode>(op)) {
        case Opcode::LOADC: return "LOADC";
        case Opcode::LOADL: return "LOADL";
        case Opcode::STOREL: return "STOREL";
        case Opcode::MOV: return "MOV";
        case Opcode::ADD: return "ADD";
        case Opcode::SUB: return "SUB";
        case Opcode::MUL: return "MUL";
        case Opcode::DIV: return "DIV";
        case Opcode::CMP: return "CMP";
        case Opcode::JMP: return "JMP";
        case Opcode::JZ: return "JZ";
        case Opcode::JNZ: return "JNZ";
        case Opcode::JL: return "JL";
        case Opcode::JG: return "JG";
        case Opcode::CALL: return "CALL";
        case Opcode::RET: return "RET";
        case Opcode::ENTER: return "ENTER";
        case Opcode::LEAVE: return "LEAVE";
        case Opcode::PRINT: return "PRINT";
        case Opcode::HALT: return "HALT";
        default: return "UNKNOWN";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: disasm <file.detbc>\n";
        return 1;
    }

    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open file: " << argv[1] << "\n";
        return 1;
    }

    

    std::vector<Instruction> code;
    Instruction inst;

    while (in.read(reinterpret_cast<char*>(&inst), sizeof(inst))) {
        code.push_back(inst);
    }

    std::cout << "[DISASM] Loaded " << code.size() << " instructions.\n\n";

    size_t pc = 0;
    for (const auto& i : code) {
        std::cout << "[" << pc++ << "] "
                  << opcodeToString(static_cast<uint8_t>(i.opcode))
                  << "  a=" << (int)i.a
                  << "  b=" << (int)i.b
                  << "  c=" << (int)i.c
                  << "\n";
    }

    return 0;
}
