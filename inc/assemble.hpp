#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "detvm.hpp"
#include "helpers.hpp"

namespace detvm::assembler {

struct UnresolvedJump {
    size_t inst_index;      // index in code
    std::string label;      // target label
    detvm::Opcode op;       // opcode (JMP/JZ/etc)
    bool target_in_b;       // true if target PC goes into i.b, false for i.a
};

struct AssemblerResult {
    std::vector<detvm::Instruction> code;
    std::vector<UnresolvedJump> unresolved;
    std::unordered_map<std::string, size_t> label_to_pc;
};

// first pass: parse lines, record labels and unresolved jumps
AssemblerResult assembleFirstPass(const std::vector<std::string>& lines);

// optional: assemble multiple lines without labels
std::vector<detvm::Instruction> assemble(const std::vector<std::string>& lines);

} // namespace detvm::assembler
