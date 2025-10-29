#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include "detvm.hpp"
#include "helpers.hpp"
#include "constant_pool.hpp"


namespace detvm::assembler {


struct UnresolvedJump {
    size_t inst_index;      // index in code
    std::string label;      // target label
    detvm::Opcode op;       // opcode (JMP/JZ/etc)
    uint8_t target_in_b;       // true if target PC goes into i.b, false for i.a
};


struct Function {
    std::string name;
    size_t pc_start = 0;      // where the function code starts
    size_t pc_end = 0;   // where the function code ends
    uint16_t params = 0;       // number of parameters
    uint16_t locals = 0;       // number of locals
    std::unordered_map<std::string, uint16_t> local_names; // name -> local index
    std::unordered_map<std::string, uint16_t> param_names; // parameter -> local index
};


struct AssemblerResult {
    ConstantPool pool;
    std::vector<detvm::Instruction> code;
    std::vector<UnresolvedJump> unresolved;
    std::unordered_map<std::string, size_t> label_to_pc;
    std::unordered_map<std::string, Function> funcs;
    
};




// first pass: parse lines, record labels and unresolved jumps
AssemblerResult assembleFirstPass(const std::vector<std::string>& lines);

void writeObject(const std::string& path, const AssemblerResult& result);

} // namespace detvm::assembler
