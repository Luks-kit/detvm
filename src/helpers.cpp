#include "helpers.hpp"
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace detvm::assembler {

std::string trim(const std::string& s) {
    auto first = s.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    auto last = s.find_last_not_of(" \t");
    return s.substr(first, last - first + 1);
}

size_t parseReg(const std::string& token, bool& isLocal) {
    if (token.size() >= 3 && token[0] == '%') {
        if (token[1] == 'r') {
            isLocal = false;
            return std::stoi(token.substr(2));
        } else if (token[1] == 'l') {
            isLocal = true;
            return std::stoi(token.substr(2));
        }
    }
    throw std::runtime_error("Invalid register/local: " + token);
}

std::string getOperandToken(const std::string& line, size_t index) {
    size_t pos = line.find(' ');
    if (pos == std::string::npos) throw std::runtime_error("Malformed instruction: " + line);
    std::string rest = trim(line.substr(pos+1));

    std::vector<std::string> tokens;
    std::istringstream iss(rest);
    std::string tok;
    while (std::getline(iss, tok, ',')) tokens.push_back(trim(tok));

    if (index >= tokens.size())
        throw std::runtime_error("Operand index out of range in line: " + line);

    return tokens[index];
}

detvm::Opcode mnemonicToOpcode(const std::string& mnemonic) {
    static std::unordered_map<std::string, detvm::Opcode> table = {
        {"LOADC",   detvm::Opcode::LOADC}, {"LOADL",   detvm::Opcode::LOADL}, {"STOREL",  detvm::Opcode::STOREL},
        {"MOV",     detvm::Opcode::MOV},   {"ADD",     detvm::Opcode::ADD},   {"SUB",     detvm::Opcode::SUB},
        {"MUL",     detvm::Opcode::MUL},   {"DIV",     detvm::Opcode::DIV},   {"NEG",     detvm::Opcode::NEG},
        {"CMP",     detvm::Opcode::CMP},   {"NOT",     detvm::Opcode::NOT},   {"AND",     detvm::Opcode::AND},
        {"OR",      detvm::Opcode::OR},    {"LEN",     detvm::Opcode::LEN},   {"NEWARR",  detvm::Opcode::NEWARR},
        {"LOADARR", detvm::Opcode::LOADARR},{"STOREARR",detvm::Opcode::STOREARR},{"JMP", detvm::Opcode::JMP},
        {"JZ",      detvm::Opcode::JZ},    {"JNZ",     detvm::Opcode::JNZ},   {"JL",      detvm::Opcode::JL},
        {"JG",      detvm::Opcode::JG},    {"CALL",    detvm::Opcode::CALL},  {"RET",     detvm::Opcode::RET},
        {"PRINT",   detvm::Opcode::PRINT}, {"RAIIDROP",detvm::Opcode::RAIIDROP}, {"HALT", detvm::Opcode::HALT},
    };
    auto it = table.find(mnemonic);
    if (it == table.end()) throw std::runtime_error("Unknown mnemonic: " + mnemonic);
    return it->second;
}

// === parseInstruction takes a line and turns it into an Instruction (to be taken by the assembler) ===
detvm::Instruction parseInstruction(const std::string& line) {
    std::string cleaned = trim(line);
    if (cleaned.empty() || cleaned[0] == ';') return {};

    size_t pos = cleaned.find("->");
    std::string left = (pos != std::string::npos) ? trim(cleaned.substr(0, pos)) : cleaned;
    std::string dst  = (pos != std::string::npos) ? trim(cleaned.substr(pos + 2)) : "";

    std::istringstream iss(left);
    std::string mnemonic;
    iss >> mnemonic;

    std::string rest;
    std::getline(iss, rest);
    std::vector<std::string> tokens;
    std::istringstream opss(rest);
    std::string tok;
    while (std::getline(opss, tok, ',')) tokens.push_back(trim(tok));

    detvm::Opcode op = mnemonicToOpcode(mnemonic);
    detvm::Instruction inst{};
    inst.opcode = op;

    bool isLocal = false;

    switch(op) {
        case detvm::Opcode::LOADC: {
            if (tokens.size() != 1) throw std::runtime_error("LOADC needs one operand");
            inst.a = parseReg(dst, isLocal);
            if (isLocal) throw std::runtime_error("LOADC destination cannot be local");
            inst.b = std::stoi(tokens[0]);
            break;
        }
        case detvm::Opcode::MOV:
        case detvm::Opcode::LEN: {
            if (tokens.size() != 1) throw std::runtime_error("MOV/LEN need one operand");
            inst.a = parseReg(dst, isLocal);
            bool srcIsLocal;
            size_t src = parseReg(tokens[0], srcIsLocal);
            if (op == detvm::Opcode::LEN && srcIsLocal) 
                throw std::runtime_error("LEN source must be a global array register");
            inst.b = src;
            break;
        }
        case detvm::Opcode::LOADL:{
            if (tokens.size() != 1) throw std::runtime_error("LOADL needs one operand");
            inst.a = parseReg(dst, isLocal);
            if (isLocal) throw std::runtime_error("LOADL destination must be a global register");
            bool srcLocal;
            inst.b = parseReg(tokens[0], srcLocal);
            if (!srcLocal) throw std::runtime_error("LOADL source must be a local (%lN)");
            break;
        }
        case detvm::Opcode::STOREL: {
            if (tokens.size() != 1) throw std::runtime_error("STOREL needs one operand");
            inst.a = parseReg(dst, isLocal);
            if (!isLocal) throw std::runtime_error("STOREL destination must be a local (%lN)");
            bool srcIsL;
            inst.b = parseReg(tokens[0], srcIsL);
            if (srcIsL) throw std::runtime_error("STOREL source must be a global register (%rN)");
            break;
        }
        case detvm::Opcode::ADD:
        case detvm::Opcode::SUB:
        case detvm::Opcode::MUL:
        case detvm::Opcode::DIV:
        case detvm::Opcode::CMP: {
            bool dummyLocal;
            inst.a = parseReg(dst, dummyLocal);
            if (dummyLocal) throw std::runtime_error("Arithmetic destination cannot be local");
            inst.b = parseReg(tokens[0], dummyLocal);
            if (dummyLocal) throw std::runtime_error("Arithmetic source B cannot be local");
            inst.c = parseReg(tokens[1], dummyLocal);
            if (dummyLocal) throw std::runtime_error("Arithmetic source C cannot be local");
            break;
        }
        case detvm::Opcode::NEG:
        case detvm::Opcode::NOT: {
            bool dstLocal = false, bLocal = false;
            inst.a = parseReg(dst, dstLocal);
            if (dstLocal) throw std::runtime_error("Destination of NEG/NOT must be a global register (%rN)");
            inst.b = parseReg(tokens[0], bLocal);
            if (bLocal) throw std::runtime_error("Source of NEG/NOT must be a global register (%rN)");
            inst.c = 0;
            break;
        }
        case detvm::Opcode::AND:
        case detvm::Opcode::OR: {
            bool dstLocal=false, bLocal=false, cLocal=false;
            inst.a = parseReg(dst, dstLocal);
            if (dstLocal) throw std::runtime_error("Destination of AND/OR must be a global register (%rN)");
            inst.b = parseReg(tokens[0], bLocal);
            if (bLocal) throw std::runtime_error("First source of AND/OR must be a global register (%rN)");
            inst.c = parseReg(tokens[1], cLocal);
            if (cLocal) throw std::runtime_error("Second source of AND/OR must be a global register (%rN)");
            break;
        }
        case detvm::Opcode::NEWARR: {
            bool dstLocal=false;
            inst.a = parseReg(dst, dstLocal);
            if (dstLocal) throw std::runtime_error("Destination of NEWARR must be a global register (%rN)");
            inst.c = std::stoi(tokens[0]);
            break;
        }
        case detvm::Opcode::LOADARR: {
            bool dstLocal=false, bLocal=false, cLocal=false;
            inst.a = parseReg(dst, dstLocal);
            if (dstLocal) throw std::runtime_error("Destination of LOADARR must be a global register (%rN)");
            inst.b = parseReg(tokens[0], bLocal);
            inst.c = parseReg(tokens[1], cLocal);
            if (bLocal || cLocal) throw std::runtime_error("Sources of LOADARR must be global registers (%rN)");
            break;
        }
        case detvm::Opcode::STOREARR: {
            bool aLocal=false, bLocal=false, cLocal=false;
            inst.a = parseReg(dst, aLocal);
            inst.b = parseReg(tokens[0], bLocal);
            inst.c = parseReg(tokens[1], cLocal);
            if (aLocal || bLocal || cLocal) throw std::runtime_error("All operands of STOREARR must be global registers (%rN)");
            break;
        }
        case detvm::Opcode::PRINT:
        case detvm::Opcode::RET:
        case detvm::Opcode::RAIIDROP: {
            if (!dst.empty()) inst.a = parseReg(dst, isLocal);
            else if (!tokens.empty()) inst.a = parseReg(tokens[0], isLocal);
            else throw std::runtime_error("Instruction requires a target register");
            break;
        }
       case detvm::Opcode::CALL: {
            if (tokens.size() != 2) throw std::runtime_error("CALL needs one operand for argc");
            uint8_t argc = static_cast<uint8_t>(std::stoi(tokens[1]));

            inst.a = 0xFF;       // placeholder for function PC, patched by linker
            inst.b = argc;       // actual argument count
            inst.c = 0;          // unused
            break;
        }

        case detvm::Opcode::JMP: {
            inst.a = 0xFF;       // unresolved target label
            inst.b = 0;          // unused
            inst.c = 0;
            break;
        }

        case detvm::Opcode::JZ:
        case detvm::Opcode::JNZ:
        case detvm::Opcode::JL:
        case detvm::Opcode::JG: {
            if (tokens.size() != 2) throw std::runtime_error("Conditional jump needs target label");
            inst.a = parseReg(tokens[0], isLocal);  // register to check
            inst.b = 0xFF;                     // unresolved target label, patched later
            inst.c = 0;
            break;
        }
        default: break;
    }

    return inst;
}

} // namespace detvm::assembler
