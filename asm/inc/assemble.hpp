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


struct Function {
    std::string name;
    size_t pc_start = 0;      // where the function code starts
    uint8_t params = 0;       // number of parameters
    uint8_t locals = 0;       // number of locals
    std::unordered_map<std::string, uint8_t> local_names; // name -> local index
};


struct AssemblerResult {
    std::vector<detvm::Instruction> code;
    std::vector<UnresolvedJump> unresolved;
    std::unordered_map<std::string, size_t> label_to_pc;
    std::unordered_map<std::string, Function> funcs;
};


// first pass: parse lines, record labels and unresolved jumps
AssemblerResult assembleFirstPass(const std::vector<std::string>& lines);

// optional: assemble multiple lines without labels
std::vector<detvm::Instruction> assemble(const std::vector<std::string>& lines);

} // namespace detvm::assembler
