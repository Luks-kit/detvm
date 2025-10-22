#include "linker.hpp"
#include <stdexcept>
#include <iostream>

namespace detvm::linker {

void linkLabels(
    std::vector<detvm::Instruction>& code,
    const std::unordered_map<std::string, size_t>& label_to_pc,
    const std::vector<detvm::assembler::UnresolvedJump>& unresolved,
    const std::unordered_map<std::string, detvm::assembler::Function>& funcs)
{
    for (const auto& u : unresolved) {
        auto it_label = label_to_pc.find(u.label);
        auto it_func  = funcs.find(u.label);

        size_t target_pc = 0;
        if (it_label != label_to_pc.end()) {
            target_pc = it_label->second;
        } else if (it_func != funcs.end()) {
            target_pc = it_func->second.pc_start;
        } else {
            throw std::runtime_error("Undefined label or function: " + u.label);
        }

        detvm::Instruction& inst = code[u.inst_index];

        switch (u.op) {
            case detvm::Opcode::JMP:
                inst.a = static_cast<uint8_t>(target_pc);
                break;

            case detvm::Opcode::JZ:
            case detvm::Opcode::JNZ:
            case detvm::Opcode::JL:
            case detvm::Opcode::JG:
                inst.b = static_cast<uint8_t>(target_pc);
                break;

            case detvm::Opcode::CALL: {
                // automatic argc and local count
                const auto& f = it_func->second;
                inst.a = static_cast<uint8_t>(target_pc);
                inst.b = f.params; // argument count
                inst.c = f.locals; // local variable count
                break;
            }

            default:
                std::cerr << "[LINKER WARNING] Unknown opcode in unresolved entry.\n";
                break;
        }
    }

    std::cout << "[LINKER] Resolved " << unresolved.size() << " label(s)/function(s).\n";
}

} // namespace detvm::linker
