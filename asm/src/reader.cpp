#include "helpers.hpp"
#include "linker.hpp"
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace detvm
{
    

assembler::AssemblerResult linker::readObject(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("Failed to open object file: " + path);

    assembler::AssemblerResult result;

    // === HEADER ===
    uint32_t magic;
    uint16_t version;
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    in.read(reinterpret_cast<char*>(&version), sizeof(version));

    if (magic != 0x44544F42) throw std::runtime_error("Invalid object file magic");
    if (version != 1) throw std::runtime_error("Unsupported object file version");

    // === CONSTANT POOL ===
    char pool_tag[4];
    in.read(pool_tag, 4);
    if (std::string(pool_tag, 4) != "POOL") throw std::runtime_error("Expected POOL section");

    uint32_t const_count;
    in.read(reinterpret_cast<char*>(&const_count), sizeof(const_count));

    for (uint32_t i = 0; i < const_count; ++i) {
        uint8_t type;
        in.read(reinterpret_cast<char*>(&type), 1);
        switch (static_cast<ConstType>(type)) {
            case ConstType::INT: {
                int32_t val;
                in.read(reinterpret_cast<char*>(&val), sizeof(val));
                result.pool.addInt(val);
                break;
            }
            case ConstType::DOUBLE: {
                double val;
                in.read(reinterpret_cast<char*>(&val), sizeof(val));
                result.pool.addDouble(val);
                break;
            }
            case ConstType::STRING: {
                uint64_t len;
                in.read(reinterpret_cast<char*>(&len), sizeof(len));
                std::string s(len, '\0');
                in.read(s.data(), len);
                result.pool.addString(s);
                break;
            }
            case ConstType::CHAR: {
                char val;
                in.read(reinterpret_cast<char*>(&val), sizeof(val));
                result.pool.addChar(val);
                break;
            }
            default:
                throw std::runtime_error("Unknown constant pool type: " + type);
        }
    }

    // === FUNCTION TABLE ===
    char func_tag[4];
    in.read(func_tag, 4);
    if (std::string(func_tag, 4) != "FUNC") throw std::runtime_error("Expected FUNC section");

    uint32_t func_count;
    in.read(reinterpret_cast<char*>(&func_count), sizeof(func_count));

    for (uint32_t i = 0; i < func_count; ++i) {
        uint32_t name_len;
        in.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        std::string name(name_len, '\0');
        in.read(name.data(), name_len);

        assembler::Function fn;
        fn.name = name;

        uint16_t params, locals;
        uint32_t offset, size;

        in.read(reinterpret_cast<char*>(&params), sizeof(params));
        in.read(reinterpret_cast<char*>(&locals), sizeof(locals));
        in.read(reinterpret_cast<char*>(&offset), sizeof(offset));
        in.read(reinterpret_cast<char*>(&size), sizeof(size));

        fn.params = params;
        fn.locals = locals;
        fn.pc_start = offset;
        fn.pc_end = offset + size;

        result.funcs[name] = fn;
    }



    // === UNRESOLVED ENTRIES ===
    char unrs_tag[4];
    in.read(unrs_tag, 4);
    if (std::string(unrs_tag, 4) != "UNRS") throw std::runtime_error("Expected UNRS section");

    uint32_t unresolved_count;
    in.read(reinterpret_cast<char*>(&unresolved_count), sizeof(unresolved_count));

    for (uint32_t i = 0; i < unresolved_count; ++i) {
        assembler::UnresolvedJump u;
        uint32_t inst_idx, label_size;
        uint8_t op, target_in_b;

        in.read(reinterpret_cast<char*>(&inst_idx), sizeof(inst_idx));
        in.read(reinterpret_cast<char*>(&op), sizeof(op));
        in.read(reinterpret_cast<char*>(&target_in_b), sizeof(target_in_b));
        in.read(reinterpret_cast<char*>(&label_size), sizeof(label_size));

        std::string label(label_size, '\0');
        in.read(label.data(), label_size);

        u.inst_index = inst_idx;
        u.op = static_cast<Opcode>(op);
        u.target_in_b = target_in_b != 0;
        u.label = std::move(label);

        result.unresolved.push_back(u);
    }

    // === LABELS ===

    char lbls_tag[4];
    in.read(lbls_tag, 4);
    if (std::string(lbls_tag, 4) != "LBLS") throw std::runtime_error("Expected LBLS section");

    uint32_t label_count;
    in.read(reinterpret_cast<char*>(&label_count), sizeof(label_count));

    for (uint32_t i = 0; i < label_count; ++i) {
        uint32_t label_len;
        uint32_t target_pc;

        in.read(reinterpret_cast<char*>(&label_len), sizeof(label_len));
        std::string label(label_len, '\0');
        in.read(label.data(), label_len);
        in.read(reinterpret_cast<char*>(&target_pc), sizeof(target_pc));

        result.label_to_pc[std::move(label)] = target_pc;
    }
    // === CODE ===
    char code_tag[4];
    in.read(code_tag, 4);
    if (std::string(code_tag, 4) != "CODE") throw std::runtime_error("Expected CODE section");

    uint32_t code_count;
    in.read(reinterpret_cast<char*>(&code_count), sizeof(code_count));

    result.code.resize(code_count);
    for (uint32_t i = 0; i < code_count; ++i) {
        in.read(reinterpret_cast<char*>(&result.code[i]), sizeof(Instruction));
    }

    return result;
}    


} // namespace detvm