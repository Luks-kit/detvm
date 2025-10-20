    #include "detvm.hpp"

    namespace detvm {

    VM::VM(size_t reg_count) : regs(reg_count) {
        setupDispatchTable();
    }

    void VM::setupDispatchTable() {
        op_table = {
            // Data & Arithmetic
            {Opcode::LOADC,   &VM::op_loadc},
            {Opcode::LOADL,   &VM::op_loadl},
            {Opcode::STOREL,  &VM::op_storel},
            {Opcode::MOV,     &VM::op_mov},
            {Opcode::ADD,     &VM::op_add},
            {Opcode::SUB,     &VM::op_sub},
            {Opcode::MUL,     &VM::op_mul},
            {Opcode::DIV,     &VM::op_div},
            {Opcode::NEG,     &VM::op_neg},
            {Opcode::CMP,     &VM::op_cmp},
            {Opcode::NOT,     &VM::op_not},
            {Opcode::AND,     &VM::op_and},
            {Opcode::OR,      &VM::op_or},

            // Control Flow
            {Opcode::JMP,     &VM::op_jmp},
            {Opcode::JZ,      &VM::op_jz},
            {Opcode::JNZ,     &VM::op_jnz},
            {Opcode::JL,      &VM::op_jl},
            {Opcode::JG,      &VM::op_jg},

            // Function Call & Stack
            {Opcode::CALL,    &VM::op_call},
            {Opcode::RET,     &VM::op_ret},
            {Opcode::ENTER,   &VM::op_enter},
            {Opcode::LEAVE,   &VM::op_leave},

            // Array & Memory
            {Opcode::NEWARR,  &VM::op_newarr},
            {Opcode::LOADARR, &VM::op_loadarr},
            {Opcode::STOREARR,&VM::op_storearr},
            {Opcode::LEN,     &VM::op_len},
            
            {Opcode::NOP, &VM::op_nop},
            {Opcode::PRINT,   &VM::op_print},
            {Opcode::HALT, &VM::op_halt},
            // Ownership & Borrowing
            {Opcode::OWN,     &VM::op_own},
            {Opcode::MOVE,    &VM::op_move},
            {Opcode::VIEW,    &VM::op_view},
            {Opcode::EDIT,    &VM::op_edit},
        /* {Opcode::CLONE,   &VM::op_clone},
            {Opcode::DROP,    &VM::op_drop},

            // Refcount & Safety
        /* {Opcode::INCREF,      &VM::op_incref},
            {Opcode::DECREF,      &VM::op_decref},
            {Opcode::CHECKEXCL,   &VM::op_checkexcl},
            {Opcode::CHECKLIVE,   &VM::op_checklive}, */
            {Opcode::RAIIDROP,    &VM::op_raiidrop},
        };
    }



    void VM::run() {
        while (pc < code.size()) {
            step();
        }
    }

    void VM::step() {
        const auto& inst = code[pc];
        dispatch(inst);
        std::cout << "[PC " << pc << "] ";
        for (size_t i = 0; i < regs.size(); ++i)
            std::cout << "%r" << i << "=" << regs[i].str() << " ";
        std::cout << "\n";
    }


    void VM::dispatch(const Instruction& inst) {
        auto it = op_table.find(inst.opcode);
        if (it != op_table.end()) {
            (this->*(it->second))(inst);
        } else {
            std::cerr << "Unknown opcode\n";
            pc++;
        }
    }


} // namespace detvm
