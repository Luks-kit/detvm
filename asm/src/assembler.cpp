#include "assemble.hpp"
#include <sstream>
#include <stdexcept>
#include <regex>

namespace detvm::assembler {



AssemblerResult assembleFirstPass(const std::vector<std::string>& lines) {
    AssemblerResult result;
    Function* current_func = nullptr;

    for (size_t i = 0; i < lines.size(); ++i) {
        std::string line = trim(lines[i]);
        if (line.empty() || line[0] == ';') continue;

        if (line.rfind(".func", 0) == 0) {
            std::string name = trim(line.substr(5));
            result.funcs[name] = {name, result.code.size()};
            current_func = &result.funcs[name];
            continue;
        }

        if (line.rfind(".label", 0) == 0) {
            std::string label = trim(line.substr(6));
            result.label_to_pc[label] = result.code.size();
            continue;
        }   

        if (line.rfind(".code", 0) == 0) {
            if (!current_func) throw std::runtime_error(".code: outside of function");
            result.label_to_pc[current_func->name] = result.code.size();
            current_func->pc_start = result.code.size();
            continue;
        }

        if (line.rfind(".params", 0) == 0) {
            if (!current_func) throw std::runtime_error(".params outside of function");
            current_func->params = static_cast<uint8_t>(std::stoi(trim(line.substr(7))));
            continue;
        }

        if (line.rfind(".locals", 0) == 0) {
            if (!current_func) throw std::runtime_error(".locals outside of function");
            current_func->locals = static_cast<uint8_t>(std::stoi(trim(line.substr(7))));
            continue;
        }

        if (line.rfind("var ", 0) == 0) {
            if (!current_func) throw std::runtime_error("var outside of function");
            std::string name = trim(line.substr(4));
            uint8_t index = current_func->local_names.size() + current_func->params;
            if (index >= current_func->params + current_func->locals)
                throw std::runtime_error("Too many named locals");
            current_func->local_names[name] = index;
            continue;
        }

        if (line.rfind(".end", 0) == 0) {
            current_func = nullptr;
            continue;
        }

         // === Replace local variable names with %lN before parsing ===
        if (current_func) {
            for (const auto& [name, index] : current_func->local_names) {
                std::regex var_regex("\\b" + name + "\\b");
                line = std::regex_replace(line, var_regex, "%l" + std::to_string(index));
            }
        }

        // existing instruction parsing
        detvm::Instruction inst = parseInstruction(line);
        result.code.push_back(inst);

        // record unresolved jumps / calls
        switch (inst.opcode) {
            case detvm::Opcode::JMP:
                result.unresolved.push_back({result.code.size()-1, getOperandToken(line,0), inst.opcode, false});
                break;
            case detvm::Opcode::JZ:
            case detvm::Opcode::JNZ:
            case detvm::Opcode::JL:
            case detvm::Opcode::JG:
                result.unresolved.push_back({result.code.size()-1, getOperandToken(line,1), inst.opcode, true});
                break;
            case detvm::Opcode::CALL:
                result.unresolved.push_back({result.code.size()-1, getOperandToken(line,0), inst.opcode, false});
                break;
            default: break;
        }
    }

    return result;
}


} // namespace detvm::assembler
