#include "linker.hpp"
#include <stdexcept>
#include <iostream>

namespace detvm::linker {

void linkLabels(
    std::vector<detvm::Instruction>& code,
    const std::unordered_map<std::string, size_t>& label_to_pc,
    const std::vector<detvm::assembler::UnresolvedJump>& unresolved)
{
    for (const auto& u : unresolved) {
        auto it = label_to_pc.find(u.label);
        if (it == label_to_pc.end()) {
            throw std::runtime_error("Undefined label: " + u.label);
        }

        size_t target_pc = it->second;
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

            case detvm::Opcode::CALL:
                inst.a = static_cast<uint8_t>(target_pc);
                break;

            default:
                std::cerr << "[LINKER WARNING] Unknown opcode in unresolved entry.\n";
                break;
        }
    }

    std::cout << "[LINKER] Resolved " << unresolved.size() << " label(s).\n";
}

} // namespace detvm::linker
