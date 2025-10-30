#pragma once
#include <vector>
#include <array>
#include <variant>
#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include <iostream>
#include "ops.hpp"
#include "reader.hpp"

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

    int32_t asInt() const {
        if (std::holds_alternative<int32_t>(data)) return std::get<int32_t>(data);
        if (std::holds_alternative<double>(data)) return static_cast<int32_t>(std::get<double>(data));
        if (std::holds_alternative<bool>(data)) return std::get<bool>(data) ? 1 : 0;
        throw std::runtime_error("Value is not numeric (int/bool/double)");
    }

    double asFloat() const {
        if (std::holds_alternative<double>(data)) return std::get<double>(data);
        if (std::holds_alternative<int32_t>(data)) return static_cast<double>(std::get<int32_t>(data));
        if (std::holds_alternative<bool>(data)) return std::get<bool>(data) ? 1.0 : 0.0;
        throw std::runtime_error("Value is not numeric (int/bool/double)");
    }

    bool asBool() const {
        if (std::holds_alternative<bool>(data)) return std::get<bool>(data);
        if (std::holds_alternative<int32_t>(data)) return std::get<int32_t>(data) != 0;
        if (std::holds_alternative<double>(data)) return std::get<double>(data) != 0.0;
        if (std::holds_alternative<std::string>(data)) return !std::get<std::string>(data).empty();
        if (std::holds_alternative<std::vector<Value>>(data)) return !std::get<std::vector<Value>>(data).empty();
        return false;
    }
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
    uint16_t a = 0;
    uint16_t b = 0;
    uint16_t c = 0;
};

struct Frame {
    std::vector<Value> locals;
    std::vector<Value> args;
    size_t return_pc = 0;
};




class VM {
public:
    std::vector<Instruction> code;
    std::vector<Value> regs;
    std::vector<Value> params;
    std::stack<Frame> callstack;
    std::vector<Value> constant_pool;
    size_t pc = 0;

    VM(size_t reg_count = 8);

    void run();
    void step();
    void dispatch(const Instruction& inst);
    void loadProgram(const std::vector<uint8_t>& data);

private:
    using OpFn = void(VM::*)(const Instruction&);
    std::unordered_map<Opcode, OpFn> op_table;
    std::array<OpFn, 0x100> dispatch_table{};

    const uint64_t CURRENT_VM_VERSION = 1;
    void setupDispatchTable();
    void setupOpTable();

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


    void op_mov_local(const Instruction&);
    void op_add_local(const Instruction&);
    void op_sub_local(const Instruction&);
    void op_mul_local(const Instruction&);
    void op_div_local(const Instruction&);
    void op_neg_local(const Instruction&);
    void op_cmp_local(const Instruction&);
    void op_not_local(const Instruction&);
    void op_and_local(const Instruction&);
    void op_or_local(const Instruction&);

    void op_jz_local(const Instruction&);
    void op_jnz_local(const Instruction&);
    void op_jl_local(const Instruction&);
    void op_jg_local(const Instruction&);

    void op_loadc_local(const Instruction&);
    void op_load_arg(const Instruction&);
    void op_load_param(const Instruction&);
    void op_load_paraml(const Instruction&);

};

} // namespace detvm



namespace detvm::assembler {

std::vector<uint8_t> readFile(const std::string& path);

}