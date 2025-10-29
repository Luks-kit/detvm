#include "helpers.hpp"
#include "constant_pool.hpp"
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

size_t parseReg(const std::string& token, char& regtype) {
    if (token.size() >= 3 && token[0] == '%') {
       switch(token[1]) {
        case 'l':
        case 'a':
        case 'p':
        case 'r': 
            regtype = token[1]; break;
        default: throw std::runtime_error("Invalid register: " + token);
       }
        return std::stoi(token.substr(2));
    }
    throw std::runtime_error("Invalid register: " + token);
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
    {"LOADC",    detvm::Opcode::LOADC},   {"LOADCL",  detvm::Opcode::LOADCL}, {"LOADL",   detvm::Opcode::LOADL}, 
    {"STOREL",   detvm::Opcode::STOREL},  {"MOV",     detvm::Opcode::MOV},    {"MOVL",    detvm::Opcode::MOVL},
    {"ADD",      detvm::Opcode::ADD},     {"ADDL",    detvm::Opcode::ADDL},   {"SUB",     detvm::Opcode::SUB},
    {"SUBL",     detvm::Opcode::SUBL},    {"MUL",     detvm::Opcode::MUL},    {"MULL",    detvm::Opcode::MULL},
    {"DIV",      detvm::Opcode::DIV},     {"DIVL",    detvm::Opcode::DIVL},   {"NEG",     detvm::Opcode::NEG},
    {"NEGL",     detvm::Opcode::NEGL},    {"CMP",     detvm::Opcode::CMP},    {"CMPL",    detvm::Opcode::CMPL},
    {"NOT",      detvm::Opcode::NOT},     {"NOTL",    detvm::Opcode::NOTL},   {"AND",     detvm::Opcode::AND},
    {"ANDL",     detvm::Opcode::ANDL},    {"OR",      detvm::Opcode::OR},     {"ORL",     detvm::Opcode::ORL},
    {"LEN",      detvm::Opcode::LEN},     {"NEWARR",  detvm::Opcode::NEWARR}, {"LOADARR", detvm::Opcode::LOADARR},
    {"STOREARR", detvm::Opcode::STOREARR},{"JMP",     detvm::Opcode::JMP},    {"JZ",      detvm::Opcode::JZ},
    {"JNZ",      detvm::Opcode::JNZ},     {"JL",      detvm::Opcode::JL},     {"JG",      detvm::Opcode::JG},
    {"JLZ", detvm::Opcode::JLZ},          {"JLNZ", detvm::Opcode::JLNZ},      {"JLL", detvm::Opcode::JLL}, 
    {"JLG", detvm::Opcode::JLG},          {"LOADP", detvm::Opcode::LOADP},    {"LOADLP", detvm::Opcode::LOADLP},
    {"CALL",     detvm::Opcode::CALL},    {"RET",     detvm::Opcode::RET},    {"PRINT",   detvm::Opcode::PRINT},
    {"RAIIDROP", detvm::Opcode::RAIIDROP},{"HALT",    detvm::Opcode::HALT},   {"LOADARG", detvm::Opcode::LOADARG}
};

    auto it = table.find(mnemonic);
    if (it == table.end()) throw std::runtime_error("Unknown mnemonic: " + mnemonic);
    return it->second;
}

// === parseInstruction takes a line and turns it into an Instruction (to be taken by the assembler) ===
detvm::Instruction parseInstruction(const std::string& line, ConstantPool& pool) {
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

    detvm::Opcode op = mnemonicToOpcode(mnemonic);
    detvm::Instruction inst{};
    inst.opcode = op;
    
    if (op == detvm::Opcode::LOADC) {
        tokens.push_back(rest); // tokens[0] now holds ' "Hello, World!" '
    } else {
        // For all other instructions (ADD, CMP, etc.), arguments are expected to be comma-separated.
        std::istringstream opss(rest);
        std::string tok;
        while (std::getline(opss, tok, ',')) {
            tokens.push_back(trim(tok));
        }
    }




   char regtype;

switch(op) {
    case detvm::Opcode::LOADC: {
        if (tokens.size() != 1) throw std::runtime_error("LOADC needs one operand");
        inst.a = parseReg(dst, regtype);
        if (regtype != 'r') throw std::runtime_error("LOADC destination must be global (%rN)");
        size_t constidx;
        if(pool.isInt(tokens[0])) constidx = pool.addInt(std::stoi(tokens[0]));
        else if(pool.isFloat(tokens[0])) constidx = pool.addDouble(std::stod(tokens[0]));
        else constidx = pool.addString(tokens[0]);
        inst.b = static_cast<uint16_t>(constidx);
        break;
    }
    case detvm::Opcode::LOADCL: {
        if (tokens.size() != 1) throw std::runtime_error("LOADCL needs one operand");
        inst.a = parseReg(dst, regtype);
        if (regtype != 'l') throw std::runtime_error("LOADCL destination must be local (%lN)");
        size_t constidx;
        if(pool.isInt(tokens[0])) constidx = pool.addInt(std::stoi(tokens[0]));
        else if(pool.isFloat(tokens[0])) constidx = pool.addDouble(std::stod(tokens[0]));
        else constidx = pool.addString(tokens[0]);
        inst.b = static_cast<uint16_t>(constidx);
        break;
    }
    case detvm::Opcode::LOADL:
        if (tokens.size() != 1) throw std::runtime_error("LOADL needs one operand");
        inst.a = parseReg(dst, regtype);
        if (regtype != 'r') throw std::runtime_error("LOADL destination must be global (%rN)");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'l') throw std::runtime_error("LOADL source must be local (%lN)");
        break;

    case detvm::Opcode::STOREL:
        if (tokens.size() != 1) throw std::runtime_error("STOREL needs one operand");
        inst.a = parseReg(dst, regtype);
        if (regtype != 'l') throw std::runtime_error("STOREL destination must be local (%lN)");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'r') throw std::runtime_error("STOREL source must be global (%rN)");
        break;

    // Arithmetic (global)
    case detvm::Opcode::ADD:
    case detvm::Opcode::SUB:
    case detvm::Opcode::MUL:
    case detvm::Opcode::DIV:
    case detvm::Opcode::CMP:
        inst.a = parseReg(dst, regtype);
        if (regtype != 'r') throw std::runtime_error("Arithmetic destination must be global");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'r') throw std::runtime_error("Arithmetic source B must be global");
        inst.c = parseReg(tokens[1], regtype);
        if (regtype != 'r') throw std::runtime_error("Arithmetic source C must be global");
        break;

    case detvm::Opcode::NEG:
    case detvm::Opcode::NOT:
        inst.a = parseReg(dst, regtype);
        if (regtype != 'r') throw std::runtime_error("NEG/NOT destination must be global");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'r') throw std::runtime_error("NEG/NOT source must be global");
        inst.c = 0;
        break;

    case detvm::Opcode::AND:
    case detvm::Opcode::OR:
        inst.a = parseReg(dst, regtype);
        if (regtype != 'r') throw std::runtime_error("AND/OR destination must be global");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'r') throw std::runtime_error("AND/OR first source must be global");
        inst.c = parseReg(tokens[1], regtype);
        if (regtype != 'r') throw std::runtime_error("AND/OR second source must be global");
        break;

    // Arithmetic / logic local variants
    case detvm::Opcode::ADDL:
    case detvm::Opcode::SUBL:
    case detvm::Opcode::MULL:
    case detvm::Opcode::DIVL:
    case detvm::Opcode::CMPL:
    case detvm::Opcode::NEGL:
    case detvm::Opcode::NOTL:
    case detvm::Opcode::ANDL:
    case detvm::Opcode::ORL:
    case detvm::Opcode::MOVL:
        inst.a = parseReg(dst, regtype);
        if (regtype != 'l') throw std::runtime_error("Local arithmetic destination must be local (%lN)");
        inst.b = parseReg(tokens[0], regtype);
        inst.c = (tokens.size() > 1) ? parseReg(tokens[1], regtype) : 0;
        break;

    // Local control flow
    case detvm::Opcode::JLZ:
    case detvm::Opcode::JLNZ:
    case detvm::Opcode::JLL:
    case detvm::Opcode::JLG:
        inst.a = parseReg(tokens[0], regtype);
        if (regtype != 'l') throw std::runtime_error("Local jump condition must be local (%lN)");
        inst.b = 0xFF; // unresolved label
        inst.c = 0;
        break;

    case detvm::Opcode::NEWARR:
        inst.a = parseReg(dst, regtype);
        if (regtype != 'r') throw std::runtime_error("NEWARR destination must be global (%rN)");
        inst.c = std::stoi(tokens[0]);
        break;

    case detvm::Opcode::LOADARR:
    case detvm::Opcode::STOREARR:
        inst.a = parseReg(dst, regtype);
        char bLocal, cLocal;
        inst.b = parseReg(tokens[0], bLocal);
        inst.c = parseReg(tokens[1], cLocal);
        if (bLocal != 'r' || cLocal != 'r') throw std::runtime_error("Array operands must be global (%rN)");
        break;

    case detvm::Opcode::LEN:
        inst.a = parseReg(dst, regtype);
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'r') throw std::runtime_error("LEN source must be global (%rN)");
        break;

    case detvm::Opcode::PRINT:
    case detvm::Opcode::RET:
    case detvm::Opcode::RAIIDROP:
        inst.a = dst.empty() ? parseReg(tokens[0], regtype) : parseReg(dst, regtype);
        break;

    case detvm::Opcode::CALL:
        inst.a = 0xFF; // patched by linker
        inst.b = 0;    // argc placeholder
        inst.c = 0;    // locals placeholder
        break;

    case detvm::Opcode::JMP:
        inst.a = 0xFF;
        inst.b = inst.c = 0;
        break;

    case detvm::Opcode::JZ:
    case detvm::Opcode::JNZ:
    case detvm::Opcode::JL:
    case detvm::Opcode::JG:
        inst.a = parseReg(tokens[0], regtype);
        if (regtype != 'r') throw std::runtime_error("Global jump condition must be global (%rN)");
        inst.b = 0xFF; // unresolved label
        inst.c = 0;
        break;

    case detvm::Opcode::LOADARG:
        inst.a = parseReg(dst, regtype);
        if (regtype != 'l') throw std::runtime_error("Argument must be loaded into local (%lN)");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'a') throw std::runtime_error("LOADARG source must be argument register (%aN)");
        inst.c = 0;
        break;

    case detvm::Opcode::LOADP:
        inst.a = parseReg(dst, regtype);
        if (regtype != 'p') throw std::runtime_error("LOADP destination must be parameter register (%pN)");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'r') throw std::runtime_error("LOADP source must be global register(%rN), did you mean LOADLP?");
        inst.c = 0;
        break;

    case detvm::Opcode::LOADLP:
        inst.a = parseReg(dst, regtype);
        if(regtype != 'p') throw std::runtime_error("LOADLP destination must be parameter register (%pN)");
        inst.b = parseReg(tokens[0], regtype);
        if (regtype != 'l') throw std::runtime_error("LOADLP source must be local register(%lN)");
        inst.c = 0;
        break;

    default: break;
}
    return inst;
}

} // namespace detvm::assembler
