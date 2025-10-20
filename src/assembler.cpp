#include "assemble.hpp"
#include <sstream>
#include <stdexcept>

namespace detvm::assembler {



AssemblerResult assembleFirstPass(const std::vector<std::string>& lines) {
    AssemblerResult result;

    for (size_t line_index = 0; line_index < lines.size(); ++line_index) {
        std::string line = trim(lines[line_index]);
        if (line.empty() || line[0] == ';') continue;

        // Label detection
        if (line.back() == ':') {
            std::string label = trim(line.substr(0, line.size()-1));
            result.label_to_pc[label] = result.code.size();
            continue;
        }

        // Parse instruction
        detvm::Instruction inst = parseInstruction(line);

        // Record unresolved jumps / calls
        switch (inst.opcode) {
            case detvm::Opcode::JMP:
                result.unresolved.push_back({result.code.size(), getOperandToken(line,0), inst.opcode, false});
                break;
            case detvm::Opcode::JZ:
            case detvm::Opcode::JNZ:
            case detvm::Opcode::JL:
            case detvm::Opcode::JG:
                result.unresolved.push_back({result.code.size(), getOperandToken(line,1), inst.opcode, true});
                break;
            case detvm::Opcode::CALL:
                result.unresolved.push_back({result.code.size(), getOperandToken(line,0), inst.opcode, false});
                break;
            default: break;
        }

        result.code.push_back(inst);
    }

    return result;
}

std::vector<detvm::Instruction> assemble(const std::vector<std::string>& lines) {
    std::vector<detvm::Instruction> code;
    for (const auto& line : lines) {
        detvm::Instruction inst = parseInstruction(line);
        if (inst.opcode != detvm::Opcode::NOP) code.push_back(inst);
    }
    return code;
}

} // namespace detvm::assembler
