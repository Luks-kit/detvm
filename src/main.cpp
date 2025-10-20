#include "assemble.hpp"
#include "linker.hpp"
#include "detvm.hpp"

void runWithDebug(detvm::VM& vm) {
    std::cout << "=== VM DEBUG START ===\n";
    size_t step = 0;
    while (vm.pc < vm.code.size()) {
        const detvm::Instruction& inst = vm.code[vm.pc];

        // Print current instruction info
        std::cout << "[STEP " << step << "] PC=" << vm.pc
                  << " OPCODE=" << static_cast<int>(inst.opcode)
                  << " a=" << int(inst.a) << " b=" << int(inst.b)
                  << " c=" << int(inst.c) << "\n";

        // Execute instruction
        vm.dispatch(inst);  // assumes you have a dispatch method

        // Print all registers after execution
        std::cout << "REGS: ";
        for (size_t i = 0; i < vm.regs.size(); ++i) {
            std::cout << "%r" << i << "=" << vm.regs[i].str() << " ";
        }
        std::cout << "\n\n";

        ++step;
    }
    std::cout << "=== VM DEBUG END ===\n";
}

#ifdef ASSEMBLING 
int main() {
    std::vector<std::string> lines = {
    // --- main program ---
    "LOADC 5 -> %r1          ; n = 5",
    "STOREL %r1 -> %l0       ; pass n as local[0] for call",
    "CALL fact, 1",       
    "PRINT %r0",             // result of factorial in %r0
    "HALT",

    // --- factorial function ---
    "fact:",
    "LOADL %l0 -> %r1        ; n = local[0]",
    "LOADC 1 -> %r2",
    "CMP %r1, %r2 -> %r3",
    "JZ %r3, base_case",     // if n == 1, go to base_case

    "SUB %r1, %r2 -> %r4     ; n - 1",
    "STOREL %r4 -> %l0       ; argument for recursive call",
    "CALL fact, 1",
    "MOV %r0 -> %r5          ; result of fact(n-1)",
    "LOADL %l0 -> %r6        ; original n",
    "MUL %r6, %r5 -> %r0     ; n * fact(n-1)",
    "RET %r0",

    "base_case:",
    "LOADC 1 -> %r0",
    "RET %r0"
};

    auto result = detvm::assembler::assembleFirstPass(lines);
    detvm::linker::linkLabels(result.code, result.label_to_pc, result.unresolved);

    detvm::VM vm(8);   // or however many registers you need
    vm.code = result.code;
    runWithDebug(vm);


}
#endif 
int main (){
    using namespace detvm;
    std::vector<Instruction> program = {
        {Opcode::LOADC, 0, 0, 0}, // r0 = 0
        {Opcode::LOADC, 3, 1, 0}, // r3 = 1 (increment)
        
        // cmp_start (PC=2)
        {Opcode::LOADC, 1, 5, 0}, // r1 = 5
        {Opcode::CMP, 2, 0, 1},   // r2 = r0 < r1 ? -1 : (r0 > r1 ? 1 : 0)
        {Opcode::JL, 2, 6, 0},    // if r2 < 0 jump to increment
        
        {Opcode::JMP, 8, 0, 0},   // else jump to print/halt

        // increment (PC=6)
        {Opcode::ADD, 0, 0, 3},   // r0 = r0 + r3
        {Opcode::JMP, 2, 0, 0},   // jump back to cmp_start

        // print/halt (PC=9)
        {Opcode::PRINT, 0, 0, 0}, // print r0
        {Opcode::HALT, 0, 0, 0}   // stop VM
};

    VM test;
    test.code = program;
    test.run();

}