#pragma once
#include <vector>
#include <unordered_map>
#include "detvm.hpp"
#include "assemble.hpp"

namespace detvm::linker {

struct FunctionEntry {
    std::string name;
    uint16_t params;
    uint16_t locals;
    uint32_t pc_start;
    uint32_t size;
};

struct ObjectFile {
    ConstantPool pool;
    std::unordered_map<std::string, FunctionEntry> functions;
    std::vector<detvm::Instruction> code;
    std::vector<assembler::UnresolvedJump> unresolved;
};

assembler::AssemblerResult readObject(const std::string& path);


// resolve all unresolved jumps/calls using the label table
void linkLabels(std::vector<detvm::Instruction>& code,
                const std::unordered_map<std::string, size_t>& label_to_pc,
                const std::vector<assembler::UnresolvedJump>& unresolved,
                const std::unordered_map<std::string, assembler::Function>& funcs);

assembler::AssemblerResult linkObjects(const std::vector<assembler::AssemblerResult>& objects);



} // namespace detvm::linker
