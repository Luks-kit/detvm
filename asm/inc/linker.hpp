#pragma once
#include <vector>
#include <unordered_map>
#include "detvm.hpp"
#include "assemble.hpp"

namespace detvm::linker {

// resolve all unresolved jumps/calls using the label table
void linkLabels(std::vector<detvm::Instruction>& code,
                const std::unordered_map<std::string, size_t>& label_to_pc,
                const std::vector<assembler::UnresolvedJump>& unresolved,
                const std::unordered_map<std::string, detvm::assembler::Function>& funcs);



} // namespace detvm::linker
