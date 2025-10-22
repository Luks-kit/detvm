#pragma once
#include <string>
#include "detvm.hpp"

namespace detvm::assembler {

// === trim whitespace from string ===
std::string trim(const std::string& s);

// === parse %rN or %lN into register index, set isLocal flag ===
size_t parseReg(const std::string& token, bool& isLocal);

// === parse a single instruction line into a detvm::Instruction ===
detvm::Instruction parseInstruction(const std::string& line);

// === extract the N-th operand token from a comma-separated instruction line ===
std::string getOperandToken(const std::string& line, size_t index);

// === map mnemonic string to Opcode ===
detvm::Opcode mnemonicToOpcode(const std::string& mnemonic);

} // namespace detvm::assembler
