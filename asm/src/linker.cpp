#include "linker.hpp"
#include <stdexcept>
#include <iostream>

namespace detvm::linker {

void linkLabels(
    std::vector<detvm::Instruction>& code,
    const std::unordered_map<std::string, size_t>& label_to_pc,
    const std::vector<detvm::assembler::UnresolvedJump>& unresolved,
    const std::unordered_map<std::string, FunctionEntry>& funcs)
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
                inst.a = static_cast<uint16_t>(target_pc);
                break;

            case detvm::Opcode::JZ:
            case detvm::Opcode::JNZ:
            case detvm::Opcode::JL:
            case detvm::Opcode::JG:
            case detvm::Opcode::JLZ:
            case detvm::Opcode::JLNZ:
            case detvm::Opcode::JLL:
            case detvm::Opcode::JLG:
                inst.b = static_cast<uint16_t>(target_pc);
                break;

            case detvm::Opcode::CALL: {
                // automatic argc and local count
                const auto& f = it_func->second;
                inst.a = static_cast<uint16_t>(target_pc);
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


assembler::AssemblerResult linkObjects(const std::vector<assembler::AssemblerResult>& objects) {
    assembler::AssemblerResult linked;

    uint32_t code_offset = 0;
    std::unordered_map<std::string, uint32_t> global_labels;

    // Constant pool remap: for each object, store mapping old_idx → new_idx
    std::vector<std::vector<uint32_t>> const_remap(objects.size());

    // === PASS 1: Merge constant pools ===
    for (size_t i = 0; i < objects.size(); ++i) {
        const auto& obj = objects[i];
        for (size_t old_idx = 0; old_idx < obj.pool.entries.size(); ++old_idx) {
            const auto& entry = obj.pool.entries[old_idx];
            uint32_t new_idx = linked.pool.add(entry); // deduplicating add()
            const_remap[i].push_back(new_idx);
        }
    }

    // === PASS 2: Merge functions, code, and labels ===
    for (size_t i = 0; i < objects.size(); ++i) {
        const auto& obj = objects[i];
        auto& remap = const_remap[i];

        // Merge functions (adjust PC)
        for (auto& [name, fn] : obj.funcs) {
            assembler::Function f = fn;
            f.pc_start += code_offset;
            f.pc_end += code_offset;
            linked.funcs[name] = f;
        }

        // Merge code with constant remapping
        for (const auto& inst : obj.code) {
            detvm::Instruction patched = inst;

            // If this instruction uses a constant index, remap it
            if (patched.opcode == Opcode::LOADC || patched.opcode == Opcode::LOADCL) patched.b = remap[patched.b];

            linked.code.push_back(patched);
        }

        // Merge label table
        for (auto& [label, pc] : obj.label_to_pc) {
            global_labels[label] = pc + code_offset;
        }

        code_offset = static_cast<uint32_t>(linked.code.size());
    }

    // === PASS 3: Merge unresolved references ===
    for (size_t i = 0; i < objects.size(); ++i) {
        const auto& obj = objects[i];
        uint32_t obj_code_offset = 0;
        for (size_t j = 0; j < i; ++j)
            obj_code_offset += static_cast<uint32_t>(objects[j].code.size());

        for (auto u : obj.unresolved) {
            u.inst_index += obj_code_offset;

            auto it = global_labels.find(u.label);
            if (it != global_labels.end()) {
                // Resolved immediately
                uint32_t target_pc = it->second;
                if (u.target_in_b) linked.code[u.inst_index].b = target_pc;
                else linked.code[u.inst_index].a = target_pc;
            } else {
                // Keep unresolved
                linked.unresolved.push_back(u);
            }
        }
    }

    return linked;
}

void writeProgramBinary(const std::string& path, const assembler::AssemblerResult& result) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Failed to open output file: " + path);

    // === HEADER ===
    out.write("DTVM", 4);
    uint64_t version = 1;
    out.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // === CONSTANT POOL ===
    out.write("POOL", 4);
    size_t pool_size = result.pool.entries.size();
    out.write(reinterpret_cast<const char*>(&pool_size), sizeof(pool_size));

    for (auto& entry : result.pool.entries) {
        uint8_t type = static_cast<uint8_t>(entry.type);
        out.write(reinterpret_cast<const char*>(&type), 1);

        switch (entry.type) {
            case ConstType::INT: {
                int32_t val = std::get<int32_t>(entry.value);
                size_t sz = sizeof(val);
                out.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
                out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                break;
            }
            case ConstType::DOUBLE: {
                double val = std::get<double>(entry.value);
                size_t sz = sizeof(val);
                out.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
                out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                break;
            }
            case ConstType::STRING: {
                const std::string& s = std::get<std::string>(entry.value);
                size_t sz = s.size();
                out.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
                out.write(s.data(), sz);
                break;
            }
            case ConstType::CHAR: {
                char val = std::get<char>(entry.value);
                size_t sz = sizeof(val);
                out.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
                out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                break;
            }
        }
    }

    // === CODE SECTION ===
    out.write("TEXT", 4);
    size_t code_count = result.code.size();
    out.write(reinterpret_cast<const char*>(&code_count), sizeof(code_count));

    for (auto& inst : result.code) {
        out.write(reinterpret_cast<const char*>(&inst.opcode), sizeof(inst.opcode));
        out.write(reinterpret_cast<const char*>(&inst.a), 2);
        out.write(reinterpret_cast<const char*>(&inst.b), 2);
        out.write(reinterpret_cast<const char*>(&inst.c), 2);
    }

    out.close();
}



} // namespace detvm::linker
