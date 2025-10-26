#include "assemble.hpp"
#include <sstream>
#include <stdexcept>
#include <regex>
#include <iostream>
#include <fstream>
#include <unordered_map>


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
            uint8_t index = current_func->local_names.size();
            if (index >= current_func->locals)
                throw std::runtime_error("Too many named locals");
            current_func->local_names[name] = index;
            continue;
        }

         if (line.rfind("param ", 0) == 0) {
            if (!current_func) throw std::runtime_error("param outside of function");
            std::string name = trim(line.substr(6));
            uint8_t index = current_func->param_names.size();
            if (index >= current_func->params)
                throw std::runtime_error("Too many named params");
            current_func->param_names[name] = index;
            continue;
        }

        if (line.rfind(".end", 0) == 0) {
            if (!current_func) throw std::runtime_error(".end outside of function");
            current_func->pc_end = result.code.size(); // 👈 mark end
            current_func = nullptr;
            continue;
        }

         // === Replace local variable names with %lN before parsing ===
        if (current_func) {
            for (const auto& [name, index] : current_func->local_names) {
                std::regex var_regex("\\b" + name + "\\b");
                line = std::regex_replace(line, var_regex, "%l" + std::to_string(index));
            }
            // === Do the same for args ===
            for (const auto& [pname, pindex] : current_func->param_names) {
                std::regex var_regex("\\b" + pname + "\\b");
                line = std::regex_replace(line, var_regex, "%a" + std::to_string(pindex));
            }
        }



        // existing instruction parsing
        detvm::Instruction inst = parseInstruction(line, result.pool);
        result.code.push_back(inst);

        // record unresolved jumps / calls
        switch (inst.opcode) {
            case detvm::Opcode::JMP:
                result.unresolved.push_back({result.code.size()-1, getOperandToken(line,0), inst.opcode, 0});
                break;
            case detvm::Opcode::JZ:
            case detvm::Opcode::JNZ:
            case detvm::Opcode::JL:
            case detvm::Opcode::JG:
            case detvm::Opcode::JLZ:
            case detvm::Opcode::JLNZ:
            case detvm::Opcode::JLL:
            case detvm::Opcode::JLG:
                result.unresolved.push_back({result.code.size()-1, getOperandToken(line,1), inst.opcode, 1});
                break;
            case detvm::Opcode::CALL:
                result.unresolved.push_back({result.code.size()-1, getOperandToken(line,0), inst.opcode, 0});
                break;
            default: break;
        }
    }

    return result;
}



void writeObject(const std::string& path, const AssemblerResult& result) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Failed to open output file: " + path);

    // === HEADER ===
    const uint32_t MAGIC = 0x44544F42; // "DTOB"
    const uint16_t VERSION = 1;
    out.write(reinterpret_cast<const char*>(&MAGIC), sizeof(MAGIC));
    out.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));

    // === CONSTANT POOL ===
    out.write("POOL", 4);
    uint32_t const_count = result.pool.entries.size();
    out.write(reinterpret_cast<const char*>(&const_count), sizeof(const_count));

    for (auto& entry : result.pool.entries) {
        uint8_t type = static_cast<uint8_t>(entry.type);
        out.write(reinterpret_cast<const char*>(&type), 1);

        switch (entry.type) {
            case ConstType::INT: {
                int64_t val = std::get<int64_t>(entry.value);
                out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                break;
            }
            case ConstType::DOUBLE: {
                double val = std::get<double>(entry.value);
                out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                break;
            }
            case ConstType::STRING: {
                uint32_t len = static_cast<uint32_t>(std::get<std::string>(entry.value).size());
                out.write(reinterpret_cast<const char*>(&len), sizeof(len));
                out.write(std::get<std::string>(entry.value).data(), len);
                break;
            }
        }
    }

    out.write("FUNC", 4);
    // === FUNCTION TABLE ===
    uint32_t func_count = result.funcs.size();
    out.write(reinterpret_cast<const char*>(&func_count), sizeof(func_count));

    for (auto& [name, fn] : result.funcs) {
        uint32_t name_len = static_cast<uint32_t>(name.size());
        out.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        out.write(name.data(), name_len);

        uint16_t params = fn.params;
        uint16_t locals = fn.locals;
        uint32_t offset = static_cast<uint32_t>(fn.pc_start);
        uint32_t size = static_cast<uint32_t>(fn.pc_end - fn.pc_start);


        out.write(reinterpret_cast<const char*>(&params), sizeof(params));
        out.write(reinterpret_cast<const char*>(&locals), sizeof(locals));
        out.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }

    // === UNRESOLVED ENTRIES ===
    out.write("UNRS", 4);
    uint32_t unresolved_count = static_cast<uint32_t>(result.unresolved.size());
    out.write(reinterpret_cast<const char*>(&unresolved_count), sizeof(unresolved_count));

    for (auto& u : result.unresolved) {
        uint32_t inst_idx = static_cast<uint32_t>(u.inst_index);
        uint8_t op = static_cast<uint8_t>(u.op);
        uint8_t target_in_b = u.target_in_b ? 1 : 0;
        uint32_t label_size = u.label.size();
        const char* label_name = u.label.data(); // store label in a string


        out.write(reinterpret_cast<const char*>(&inst_idx), sizeof(inst_idx));
        out.write(reinterpret_cast<const char*>(&op), sizeof(op));
        out.write(reinterpret_cast<const char*>(&target_in_b), sizeof(target_in_b));
        out.write(reinterpret_cast<const char*>(&label_size), sizeof(label_size));
        out.write(label_name, label_size);
    }

    // === CODE ===
    out.write("CODE", 4);
    uint32_t code_count = static_cast<uint32_t>(result.code.size());
    out.write(reinterpret_cast<const char*>(&code_count), sizeof(code_count));

    for (auto& inst : result.code) {
        out.write(reinterpret_cast<const char*>(&inst), sizeof(detvm::Instruction));
    }

    out.close();
}

} // namespace detvm::assembler
