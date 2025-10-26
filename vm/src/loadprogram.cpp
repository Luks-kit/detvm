#include <iostream>
#include "detvm.hpp"
#include "constant_pool.hpp"
#include <fstream>

namespace detvm {

void VM::loadProgram(const std::vector<uint8_t>& data) {
    Reader r(data);

    r.expect("DTVM", 4);

    uint64_t version = r.read<uint64_t>();
    if (version > CURRENT_VM_VERSION)
        throw std::runtime_error("Unsupported VM version");

    r.expect("POOL", 4);
    size_t pool_size = r.read<size_t>();

    for (size_t i = 0; i < pool_size; ++i) {
        ConstType type = r.read<ConstType>();
        size_t size = r.read<size_t>();

        switch (type) {
            case ConstType::INT: { // example: integer
                int32_t val = r.read<int32_t>();
                constant_pool.push_back(Value(val));
                break;
            }
            case ConstType::STRING: { // string
                std::string s = r.readString(size);
                constant_pool.push_back(Value(s));
                break;
            }
            case ConstType::FLOAT: { // example: float
                double val = r.read<double>();
                constant_pool.push_back(Value(val));
                break;
            }
            case ConstType::CHAR: { // example: char
                char val = r.read<char>();
                constant_pool.push_back(Value(val));
                break;
            }
            // add other pool entry types as needed
        }
    }

    r.expect("TEXT", 4);
    size_t text_size = r.read<size_t>();
    code.reserve(text_size);

    for (size_t i = 0; i < text_size; ++i) {
        Opcode opcode = r.read<Opcode>();
        uint16_t a = r.read<uint8_t>();
        uint16_t b = r.read<uint8_t>();
        uint16_t c = r.read<uint8_t>();
        code.push_back(Instruction{opcode, a, b, c});
    }

    if (!r.eof())
        std::cerr << "[warn] trailing bytes at end of file\n";
}

}

namespace detvm::assembler {

    std::vector<uint8_t> readFile(const std::string& path){
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Failed to open file: " + path);

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read file: " + path);

    return buffer;
}

}