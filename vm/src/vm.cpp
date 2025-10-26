    #include "detvm.hpp"

    namespace detvm {

    VM::VM(size_t reg_count) : regs(reg_count) {
        setupDispatchTable();
    }

    void VM::setupOpTable() {
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
            {Opcode::DROP,    &VM::op_raiidrop}
        };
    }

    void VM::setupDispatchTable(){
        dispatch_table[(uint16_t)Opcode::LOADC]   = &VM::op_loadc;
        dispatch_table[(uint16_t)Opcode::LOADL]   = &VM::op_loadl;
        dispatch_table[(uint16_t)Opcode::STOREL]  = &VM::op_storel;
        dispatch_table[(uint16_t)Opcode::MOV]     = &VM::op_mov;
        dispatch_table[(uint16_t)Opcode::ADD]     = &VM::op_add;
        dispatch_table[(uint16_t)Opcode::SUB]     = &VM::op_sub;
        dispatch_table[(uint16_t)Opcode::MUL]     = &VM::op_mul;
        dispatch_table[(uint16_t)Opcode::DIV]     = &VM::op_div;
        dispatch_table[(uint16_t)Opcode::NEG]     = &VM::op_neg;
        dispatch_table[(uint16_t)Opcode::CMP]     = &VM::op_cmp;
        dispatch_table[(uint16_t)Opcode::NOT]     = &VM::op_not;
        dispatch_table[(uint16_t)Opcode::AND]     = &VM::op_and;
        dispatch_table[(uint16_t)Opcode::OR]      = &VM::op_or;

        // -----------------------------
        // Control Flow
        // -----------------------------
        dispatch_table[(uint16_t)Opcode::JMP]     = &VM::op_jmp;
        dispatch_table[(uint16_t)Opcode::JZ]      = &VM::op_jz;
        dispatch_table[(uint16_t)Opcode::JNZ]     = &VM::op_jnz;
        dispatch_table[(uint16_t)Opcode::JL]      = &VM::op_jl;
        dispatch_table[(uint16_t)Opcode::JG]      = &VM::op_jg;

        // (extended jump variants — optional, may map to same logic)
        dispatch_table[(uint16_t)Opcode::JLZ]     = &VM::op_jz_local;
        dispatch_table[(uint16_t)Opcode::JLNZ]    = &VM::op_jnz_local;
        dispatch_table[(uint16_t)Opcode::JLL]     = &VM::op_jl_local;
        dispatch_table[(uint16_t)Opcode::JLG]     = &VM::op_jg_local;

        // -----------------------------
        // Function Call & Stack
        // -----------------------------
        dispatch_table[(uint16_t)Opcode::CALL]    = &VM::op_call;
        dispatch_table[(uint16_t)Opcode::RET]     = &VM::op_ret;
        dispatch_table[(uint16_t)Opcode::ENTER]   = &VM::op_enter;
        dispatch_table[(uint16_t)Opcode::LEAVE]   = &VM::op_leave;

        // -----------------------------
        // Local Arithmetic Variants
        // -----------------------------
        dispatch_table[(uint16_t)Opcode::ADDL]    = &VM::op_add_local;
        dispatch_table[(uint16_t)Opcode::SUBL]    = &VM::op_sub_local;
        dispatch_table[(uint16_t)Opcode::MULL]    = &VM::op_mul_local;
        dispatch_table[(uint16_t)Opcode::DIVL]    = &VM::op_div_local;
        dispatch_table[(uint16_t)Opcode::CMPL]    = &VM::op_cmp_local;
        dispatch_table[(uint16_t)Opcode::NEGL]    = &VM::op_neg_local;
        dispatch_table[(uint16_t)Opcode::NOTL]    = &VM::op_not_local;
        dispatch_table[(uint16_t)Opcode::ANDL]    = &VM::op_and_local;
        dispatch_table[(uint16_t)Opcode::ORL]     = &VM::op_or_local;
        dispatch_table[(uint16_t)Opcode::MOVL]    = &VM::op_mov_local;
        dispatch_table[(uint16_t)Opcode::LOADCL]  = &VM::op_loadc_local;
        dispatch_table[(uint16_t)Opcode::LOADARG] = &VM::op_load_arg;

        // -----------------------------
        // Array & Memory
        // -----------------------------
        dispatch_table[(uint16_t)Opcode::NEWARR]  = &VM::op_newarr;
        dispatch_table[(uint16_t)Opcode::LOADARR] = &VM::op_loadarr;
        dispatch_table[(uint16_t)Opcode::STOREARR]= &VM::op_storearr;
        dispatch_table[(uint16_t)Opcode::LEN]     = &VM::op_len;
        dispatch_table[(uint16_t)Opcode::FREE]    = &VM::op_raiidrop; // can alias raiidrop/free

        // -----------------------------
        // Tagging / Type Info (for later)
        // -----------------------------
        // dispatch_table[(uint16_t)Opcode::TAG]     = &VM::op_tag;
        // dispatch_table[(uint16_t)Opcode::WHEN]    = &VM::op_when;
        // dispatch_table[(uint16_t)Opcode::TYPEOF]  = &VM::op_typeof;

        // -----------------------------
        // Misc
        // -----------------------------
        dispatch_table[(uint16_t)Opcode::NOP]     = &VM::op_nop;
        dispatch_table[(uint16_t)Opcode::PRINT]   = &VM::op_print;
        dispatch_table[(uint16_t)Opcode::HALT]    = &VM::op_halt;
        dispatch_table[(uint16_t)Opcode::LOADP]   = &VM::op_load_param;
        dispatch_table[(uint16_t)Opcode::LOADLP]  = &VM::op_load_paraml;

        // -----------------------------
        // Ownership & Borrowing
        // -----------------------------
        dispatch_table[(uint16_t)Opcode::OWN]     = &VM::op_own;
        dispatch_table[(uint16_t)Opcode::MOVE]    = &VM::op_move;
        dispatch_table[(uint16_t)Opcode::VIEW]    = &VM::op_view;
        dispatch_table[(uint16_t)Opcode::EDIT]    = &VM::op_edit;
        dispatch_table[(uint16_t)Opcode::CLONE]   = &VM::op_view; // optional alias
        dispatch_table[(uint16_t)Opcode::DROP]    = &VM::op_raiidrop;

        // -----------------------------
        // Refcount & Safety
        // -----------------------------
        // These can be mapped to helper ops or placeholders for now
        dispatch_table[(uint16_t)Opcode::INCREF]     = &VM::op_view;
        dispatch_table[(uint16_t)Opcode::DECREF]     = &VM::op_raiidrop;
        dispatch_table[(uint16_t)Opcode::CHECKEXCL]  = &VM::op_edit;
        dispatch_table[(uint16_t)Opcode::CHECKLIVE]  = &VM::op_view;
        dispatch_table[(uint16_t)Opcode::RAIIDROP]   = &VM::op_raiidrop;


    }


    void VM::run() {
        pc = 0;
        while (pc < code.size()) {
            dispatch(code[pc]);
        }
    }

    void VM::step() {
        const auto& inst = code[pc];
        std::cout << "opcode: " << std::to_string(static_cast<uint8_t>(inst.opcode) ) 
        << ", a = " << std::to_string(inst.a)
        << ", b = " << std::to_string(inst.b)
        << " ,c = " << std::to_string(inst.c) 
        << "\n";
        dispatch(inst);
        std::cout << "[PC " << pc << "] ";
        for (size_t i = 0; i < regs.size(); ++i)
            std::cout << "%r" << i << "=" << regs[i].str() << " ";
        std::cout << "\n";
    }


    void VM::dispatch(const Instruction& inst) {
        uint16_t opcode = static_cast<uint16_t>(inst.opcode);
        auto handler = dispatch_table[opcode];

        if (!handler) {
            throw std::runtime_error("Unimplemented opcode: " + std::to_string(opcode));
        }

        (this->*handler)(inst);
    }


} // namespace detvm
