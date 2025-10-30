#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include "detvm.hpp" // includes Opcode, Instruction, ConstType, etc.
#include "constant_pool.hpp"

using namespace detvm;

static std::string opcodeName(detvm::Opcode op) {
    using namespace detvm;
    switch (op) {
        case Opcode::LOADC:    return "LOADC";
        case Opcode::LOADL:    return "LOADL";
        case Opcode::STOREL:   return "STOREL";
        case Opcode::MOV:      return "MOV";
        case Opcode::ADD:      return "ADD";
        case Opcode::SUB:      return "SUB";
        case Opcode::MUL:      return "MUL";
        case Opcode::DIV:      return "DIV";
        case Opcode::NEG:      return "NEG";
        case Opcode::CMP:      return "CMP";
        case Opcode::NOT:      return "NOT";
        case Opcode::AND:      return "AND";
        case Opcode::OR:       return "OR";

        case Opcode::JMP:      return "JMP";
        case Opcode::JZ:       return "JZ";
        case Opcode::JNZ:      return "JNZ";
        case Opcode::JL:       return "JL";
        case Opcode::JG:       return "JG";
        case Opcode::JLZ:      return "JLZ";
        case Opcode::JLNZ:     return "JLNZ";
        case Opcode::JLL:      return "JLL";
        case Opcode::JLG:      return "JLG";

        case Opcode::CALL:     return "CALL";
        case Opcode::RET:      return "RET";
        case Opcode::ENTER:    return "ENTER";
        case Opcode::LEAVE:    return "LEAVE";
        case Opcode::ADDL:     return "ADDL";
        case Opcode::SUBL:     return "SUBL";
        case Opcode::MULL:     return "MULL";
        case Opcode::DIVL:     return "DIVL";
        case Opcode::CMPL:     return "CMPL";
        case Opcode::NEGL:     return "NEGL";
        case Opcode::NOTL:     return "NOTL";
        case Opcode::ANDL:     return "ANDL";
        case Opcode::ORL:      return "ORL";
        case Opcode::MOVL:     return "MOVL";
        case Opcode::LOADCL:   return "LOADCL";
        case Opcode::LOADARG:  return "LOADARG";

        case Opcode::NEWARR:   return "NEWARR";
        case Opcode::LOADARR:  return "LOADARR";
        case Opcode::STOREARR: return "STOREARR";
        case Opcode::LEN:      return "LEN";
        case Opcode::FREE:     return "FREE";

        case Opcode::TAG:      return "TAG";
        case Opcode::WHEN:     return "WHEN";
        case Opcode::TYPEOF:   return "TYPEOF";

        case Opcode::NOP:      return "NOP";
        case Opcode::PRINT:    return "PRINT";
        case Opcode::HALT:     return "HALT";
        case Opcode::LOADP:    return "LOADP";
        case Opcode::LOADLP:   return "LOADLP";

        case Opcode::OWN:      return "OWN";
        case Opcode::MOVE:     return "MOVE";
        case Opcode::VIEW:     return "VIEW";
        case Opcode::EDIT:     return "EDIT";
        case Opcode::CLONE:    return "CLONE";
        case Opcode::DROP:     return "DROP";

        case Opcode::INCREF:    return "INCREF";
        case Opcode::DECREF:    return "DECREF";
        case Opcode::CHECKEXCL: return "CHECKEXCL";
        case Opcode::CHECKLIVE: return "CHECKLIVE";
        case Opcode::RAIIDROP:  return "RAIIDROP";

        default: return "UNKNOWN";
    }
}

static std::string constName(ConstType t) {
    switch (t) {
        case ConstType::INT: return "INT";
        case ConstType::DOUBLE: return "DOUBLE";
        case ConstType::STRING: return "STRING";
        case ConstType::CHAR: return "CHAR";
        case ConstType::FLOAT: return "FLOAT";
        default: return "???";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: disasm <file.detvm>\n";
        return 1;
    }

    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::cerr << "cannot open file: " << argv[1] << "\n";
        return 1;
    }

    auto read_u32 = [&](uint32_t& v) { in.read(reinterpret_cast<char*>(&v), 4); };
    auto read_u64 = [&](uint64_t& v) { in.read(reinterpret_cast<char*>(&v), 8); };
    auto read_u16 = [&](uint16_t& v) { in.read(reinterpret_cast<char*>(&v), 2); };
    auto read_u8  = [&](uint8_t& v) { in.read(reinterpret_cast<char*>(&v), 1); };

    auto expect = [&](const char* tag, size_t n) {
        std::string s(n, '\0');
        in.read(s.data(), n);
        if (s != std::string(tag, n))
            throw std::runtime_error("expected tag " + std::string(tag));
    };

    try {
        // === HEADER ===
        expect("DTVM", 4);
        uint64_t version;
        read_u64(version);
        std::cout << "Header: DTVM (version " << version << ")\n";

        // === POOL ===
        expect("POOL", 4);
        size_t pool_size;
        in.read(reinterpret_cast<char*>(&pool_size), sizeof(pool_size));
        std::cout << "\n[Constant Pool] (" << pool_size << " entries)\n";

        for (size_t i = 0; i < pool_size; ++i) {
            ConstType type;
            in.read(reinterpret_cast<char*>(&type), sizeof(type));

            size_t size;
            in.read(reinterpret_cast<char*>(&size), sizeof(size));

            std::cout << "  #" << i << " " << constName(type) << " ";

            switch (type) {
                case ConstType::INT: {
                    int32_t val;
                    in.read(reinterpret_cast<char*>(&val), sizeof(val));
                    std::cout << val;
                    break;
                }
                case ConstType::FLOAT:
                case ConstType::DOUBLE: {
                    double val;
                    in.read(reinterpret_cast<char*>(&val), sizeof(val));
                    std::cout << val;
                    break;
                }
                case ConstType::STRING: {
                    std::string s(size, '\0');
                    in.read(s.data(), size);
                    std::cout << "\"" << s << "\"";
                    break;
                }
                case ConstType::CHAR: {
                    char c;
                    in.read(&c, 1);
                    std::cout << "'" << c << "'";
                    break;
                }
                default:
                    std::cout << "(unknown const type)";
                    in.seekg(size, std::ios::cur);
                    break;
            }
            std::cout << "\n";
        }

        // === CODE ===
        expect("TEXT", 4);
        size_t text_size;
        in.read(reinterpret_cast<char*>(&text_size), sizeof(text_size));

        std::cout << "\n[Text Section] (" << text_size << " instructions)\n";
        for (size_t i = 0; i < text_size; ++i) {
            Opcode op;
            uint16_t a, b, c;
            in.read(reinterpret_cast<char*>(&op), sizeof(op));
            in.read(reinterpret_cast<char*>(&a), sizeof(a));
            in.read(reinterpret_cast<char*>(&b), sizeof(b));
            in.read(reinterpret_cast<char*>(&c), sizeof(c));

            std::cout << std::setw(4) << i << ": "
                      << opcodeName(op)
                      << "  a=" << a << "  b=" << b << "  c=" << c << "\n";
        }

        if (!in.eof()) {
            std::streampos pos = in.tellg();
            in.seekg(0, std::ios::end);
            auto end = in.tellg();
            if (end - pos > 0)
                std::cout << "\n[Warning] trailing " << (end - pos)
                          << " bytes after last section\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "disasm error: " << e.what() << "\n";
        return 1;
    }
}
