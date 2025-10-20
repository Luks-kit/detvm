#include "detvm.hpp"

namespace detvm {


// === Minimal Arithmetic / MOV / PRINT ===
void VM::op_loadc(const Instruction& i) { regs[i.a] = Value(i.b); pc++; }
// Load local variable (Frame.locals) into a global register
void VM::op_loadl(const Instruction& i) {
    if (callstack.empty()) { pc++; return; }
    Frame& f = callstack.top();

    if (i.b >= f.locals.size()) {
        std::cerr << "[VM ERROR] op_loadl: invalid local index " << (int)i.b << "\n";
        std::exit(1);
    }

    regs[i.a] = f.locals[i.b];  // load local into register
    pc++;
}

// Store value from global register into a local variable
void VM::op_storel(const Instruction& i) {
    if (callstack.empty()) { pc++; return; }
    Frame& f = callstack.top();

    if (i.a >= f.locals.size()) {
        std::cerr << "[VM ERROR] op_storel: invalid local index " << (int)i.a << "\n";
        std::exit(1);
    }

    f.locals[i.a] = regs[i.b];  // store register into local
    pc++;
}

void VM::op_mov(const Instruction& i)   { regs[i.a] = regs[i.b]; pc++; }
void VM::op_add(const Instruction& i)   { regs[i.a] = Value(regs[i.b].asInt() + regs[i.c].asInt()); pc++; }
void VM::op_sub(const Instruction& i)   { regs[i.a] = Value(regs[i.b].asInt() - regs[i.c].asInt()); pc++; }
void VM::op_mul(const Instruction& i)   { regs[i.a] = Value(regs[i.b].asInt() * regs[i.c].asInt()); pc++; }
void VM::op_div(const Instruction& i)   { regs[i.a] = Value(regs[i.b].asInt() / regs[i.c].asInt()); pc++; }
void VM::op_neg(const Instruction& i) {
    regs[i.a] = Value(-regs[i.b].asInt());
    pc++;
}

void VM::op_cmp(const Instruction& i) {
    int32_t lhs = regs[i.b].asInt();
    int32_t rhs = regs[i.c].asInt();
    regs[i.a] = Value(lhs < rhs ? -1 : (lhs > rhs ? 1 : 0));
    pc++;
}
void VM::op_not(const Instruction& i) {
    regs[i.a] = Value(!regs[i.b].asBool());
    pc++;
}
void VM::op_and(const Instruction& i) {
    regs[i.a] = Value(regs[i.b].asBool() && regs[i.c].asBool());
    pc++;
}
void VM::op_or(const Instruction& i) {
    regs[i.a] = Value(regs[i.b].asBool() || regs[i.c].asBool());
    pc++;
}


void VM::op_print(const Instruction& i) { std::cout << regs[i.a].str() << "\n"; pc++; }

void VM::op_newarr(const Instruction& i) {
    size_t len = static_cast<size_t>(i.c); // cast to size_t for safety

    // Sanity checks
    if (len > SIZE_MAX) { // arbitrary upper bound
        throw std::runtime_error(
            "[VM ERROR] NEWARR length too large: " + std::to_string(len));
    }

    // Optional: log the allocation
    std::cout << "[VM] Allocating array of length " << len
              << " into register %r" << int(i.a) << "\n";

    try {
        regs.at(i.a).data = std::vector<Value>(len); // may throw std::bad_alloc
    } catch (const std::bad_alloc&) {
        throw std::runtime_error("[VM ERROR] NEWARR failed: out of memory");
    }
}

void VM::op_loadarr(const Instruction& i) {
    regs[i.a] = regs[i.b].asArray()[regs[i.c].asInt()];
    pc++;
}
void VM::op_storearr(const Instruction& i) {
    regs[i.a].asArray()[regs[i.b].asInt()] = regs[i.c];
    pc++;
}
void VM::op_len(const Instruction& i) {
    regs[i.a] = Value((int32_t)regs[i.b].asArray().size());
    pc++;
}

void VM::op_jmp(const Instruction& i) {
    pc = i.a; // direct label (resolved as instruction index)
}

void VM::op_jz(const Instruction& i) {
    if (!regs[i.a].asInt()) pc = i.b;
    else pc++;
}

void VM::op_jnz(const Instruction& i) {
    if (regs[i.a].asInt()) pc = i.b;
    else pc++;
}

void VM::op_jl(const Instruction& i) {
    if (regs[i.a].asInt() < 0) pc = i.b;
    else pc++;
}

void VM::op_jg(const Instruction& i) {
    if (regs[i.a].asInt() > 0) pc = i.b;
    else pc++;
}

constexpr size_t RETURN_REG = 0; // always return to regs[0]


void VM::op_enter(const Instruction& i) {
    Frame f;
    f.return_pc = pc + 1;
    f.locals.resize(i.a);
    callstack.push(f);
}


void VM::op_leave(const Instruction&) {
    if (callstack.empty()) return;

    Frame f = callstack.top();
    callstack.pop();

     // cleanup RAII for locals
    for (auto& v : f.locals) {
        if (v.refcount <= 1)
            v = Value();
        else
            v.refcount--;
    }

    // return to caller
    pc = f.return_pc;
}
// === CALL / ENTER / RET ===

// Call a function at PC = i.a, passing `argc` arguments from regs[0..argc-1]
void VM::op_call(const Instruction& i) {
    uint8_t argc = i.b;          // number of arguments
    size_t func_pc = i.a;        // target function PC

    // push new frame
    op_enter({Opcode::ENTER, argc, 0, 0}); // argc is passed as op_enter's i.a

    // Copy arguments into frame locals
    Frame& f = callstack.top();
    for (size_t j = 0; j < argc; ++j) {
        f.locals[j] = regs[j];  // first `argc` locals are parameters
    }

    // jump to function start
    pc = func_pc;
}

// Return from function
void VM::op_ret(const Instruction& i) {
    Value retVal;

    if (callstack.empty()) {
        pc = code.size(); // terminate program
        return;
    }

    Frame& f = callstack.top();

    // read return value from callee locals
    if (i.a != 0xFF && i.a < f.locals.size())
        retVal = f.locals[i.a];

    // leave frame (cleans locals and restores PC)
    op_leave({});

    // store return value into fixed return register
    regs[RETURN_REG] = retVal;
}

void VM::op_nop(const Instruction&) {
    // do nothing
    pc++;
}

void VM::op_halt(const Instruction&) {
    std::cout << "HALT encountered. Stopping VM.\n";
    pc = code.size(); // terminate loop
}

// === Ownership Skeletons ===
// === Ownership System ===

void VM::op_own(const Instruction& i) {
    // Create an owned copy of register B into register A
    regs[i.a] = regs[i.b];
    regs[i.a].refcount = 1;
    pc++;
}

void VM::op_move(const Instruction& i) {
    // Move value from B to A, invalidating B
    regs[i.a] = std::move(regs[i.b]);
    regs[i.a].refcount = 1;
    regs[i.b] = Value(); // clear old value
    pc++;
}

void VM::op_view(const Instruction& i) {
    // Create a non-exclusive reference (shared view)
    regs[i.a] = regs[i.b]; // shallow copy
    regs[i.a].refcount = ++regs[i.b].refcount; // bump both
    pc++;
}

void VM::op_edit(const Instruction& i) {
    // Create an exclusive reference (edit view)
    if (regs[i.b].refcount > 1) {
        std::cerr << "[VM ERROR] Cannot EDIT shared value (refcount=" 
                  << regs[i.b].refcount << ")\n";
        std::exit(1);
    }

    regs[i.a] = regs[i.b]; // shallow transfer
    regs[i.a].refcount = 1; // exclusive
    pc++;
}

void VM::op_raiidrop(const Instruction& i) {
    // Auto-drop owned resource (simulate destructor)
    if (regs[i.a].refcount > 1) {
        regs[i.a].refcount--;
        std::cout << "[RAII] Decremented refcount -> " << regs[i.a].refcount << "\n";
    } else {
        std::cout << "[RAII] Dropped value in r" << (int)i.a << "\n";
        regs[i.a] = Value(); // clear content
    }
    pc++;
}

}