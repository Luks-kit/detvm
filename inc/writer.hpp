#pragma once
#include "assemble.hpp"
#include "linker.hpp"

namespace detvm::Writer {
    void writeObject(const std::string& path, const assembler::AssemblerResult& result);

    void writeProgramBinary(const std::string& path, const assembler::AssemblerResult& result);

}