
#include "assemble.hpp"
#include "linker.hpp"
#include "writer.hpp"
namespace detvm::Writer
{
    
void writeObject(const std::string& path, const assembler::AssemblerResult& result) {
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
                int32_t val = std::get<int32_t>(entry.value);
                out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                break;
            }
            case ConstType::DOUBLE: {
                double val = std::get<double>(entry.value);
                out.write(reinterpret_cast<const char*>(&val), sizeof(val));
                break;
            }
            case ConstType::STRING: {
                uint64_t len = static_cast<uint64_t>(std::get<std::string>(entry.value).size());
                out.write(reinterpret_cast<const char*>(&len), sizeof(len));
                out.write(std::get<std::string>(entry.value).data(), len);
                break;
            }
            case ConstType::CHAR: {
                char val = std::get<char>(entry.value);
                out.write(&val, sizeof(val));
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

    // === LABELS ===

    out.write("LBLS", 4);
     uint32_t label_count = static_cast<uint32_t>(result.label_to_pc.size());
    out.write(reinterpret_cast<const char*>(&label_count), sizeof(label_count));

     for (const auto& [label, pc] : result.label_to_pc) {
        uint32_t label_len = static_cast<uint32_t>(label.size());
        uint32_t target_pc = static_cast<uint32_t>(pc);

        out.write(reinterpret_cast<const char*>(&label_len), sizeof(label_len));
        out.write(label.data(), label_len);
        out.write(reinterpret_cast<const char*>(&target_pc), sizeof(target_pc));
     }

    // === CODE ===
    out.write("CODE", 4);
    uint32_t code_count = static_cast<uint32_t>(result.code.size());
    out.write(reinterpret_cast<const char*>(&code_count), sizeof(code_count));

    for (const auto& inst : result.code) {
        out.write(reinterpret_cast<const char*>(&inst), sizeof(Instruction));
    }

    out.close();
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


    
} // namespace detvm::Writer


