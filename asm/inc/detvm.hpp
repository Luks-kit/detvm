#pragma once
#include <vector>
#include <variant>
#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include <iostream>
#include "ops.hpp"

namespace detvm {

struct Value {
    std::variant<int32_t, double, bool, std::string, std::vector<Value>, std::monostate> data;
    int refcount = 1; // for OWN/VIEW/EDIT

    Value() = default;
    Value(int32_t v) : data(v) {}
    Value(double v) : data(v) {}
    Value(bool v) : data(v) {}
    Value(std::string v) : data(std::move(v)) {}
    Value(std::vector<Value> v) : data(std::move(v)) {}

    int32_t asInt() const { return std::get<int32_t>(data); }
    double asFloat() const { return std::get<double>(data); }
    bool asBool() const { return std::get<bool>(data); }
    const std::vector<Value>& asArray() const { return std::get<std::vector<Value>>(data); }
    std::vector<Value>& asArray() { return std::get<std::vector<Value>>(data); }

    std::string str() const {
        if (std::holds_alternative<int32_t>(data)) return std::to_string(asInt());
        if (std::holds_alternative<double>(data)) return std::to_string(asFloat());
        if (std::holds_alternative<bool>(data)) return asBool() ? "true" : "false";
        if (std::holds_alternative<std::string>(data)) return std::get<std::string>(data);
        if (std::holds_alternative<std::vector<Value>>(data)) return "[array]";
        return "<unknown>";
    }
};

struct Instruction {
    Opcode opcode;
    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t c = 0;
};

struct Frame {
    std::vector<Value> locals;
    size_t return_pc = 0;
    size_t base = 0; // base register offset
};

class VM {
public:
    std::vector<Instruction> code;
    std::vector<Value> regs;
    std::stack<Frame> callstack;
    size_t pc = 0;

    VM(size_t reg_count = 8);

    void run();
    void step();
    void dispatch(const Instruction& inst);
    
private:
    using OpFn = void(VM::*)(const Instruction&);
    std::unordered_map<Opcode, OpFn> op_table;


    void setupDispatchTable();

    // Essential opcode functions
    void op_loadc(const Instruction&);
    void op_loadl(const Instruction&);
    void op_storel(const Instruction&);
    void op_mov(const Instruction&);
    void op_add(const Instruction&);
    void op_sub(const Instruction&);
    void op_mul(const Instruction&);
    void op_div(const Instruction&);
    void op_cmp(const Instruction&);
    void op_not(const Instruction&);
    void op_and(const Instruction&);
    void op_or(const Instruction&);
    void op_neg(const Instruction&);
    
    void op_print(const Instruction&);

    void op_jmp(const Instruction&);
    void op_jz(const Instruction&);
    void op_jnz(const Instruction&);
    void op_jl(const Instruction&);
    void op_jg(const Instruction&);


    void op_newarr(const Instruction&);
    void op_loadarr(const Instruction&);
    void op_storearr(const Instruction&);
    void op_len(const Instruction&);

    void op_call(const Instruction&);
    void op_ret(const Instruction&);
    void op_enter(const Instruction&);
    void op_leave(const Instruction&);
    void op_nop(const Instruction&);
    void op_halt(const Instruction&);


    void op_own(const Instruction&);
    void op_move(const Instruction&);
    void op_view(const Instruction&);
    void op_edit(const Instruction&);
    void op_raiidrop(const Instruction&);
};

} // namespace detvm
