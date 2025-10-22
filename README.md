# detvm

**detvm** is a lightweight virtual machine and compiler toolchain designed for experimenting with custom bytecode execution, register/stack hybrids, and language design.  
It’s small, fast, and written in C++ — built to be easily hacked on or embedded into other projects.

---

## 🧩 Structure

The project is split into two main parts:

- **Compiler** – Assembles `.detasm` source files into bytecode (`.detbc`).
- **PVM (Program Virtual Machine)** – Executes compiled bytecode files with isolated stack frames, local registers, and a simple instruction set.


---

## ⚙️ Build

Using **CMake** + **Ninja**:

```bash
mkdir build
cmake -S . -B build
cmake --build build

```

You’ll end up with three binaries:
- `detasm` — compiler / assembler
- `detdisasm` — disassembler for debugging purposes 
- `detvm` — virtual machine runtime

---

## 🚀 Usage

### Compile
```bash
./build/asm/detasm program.detasm program.detbc
```

### Run
```bash
./build/vm/detvm program.detbc
```

---

## 🧠 About the VM

- **Hybrid model:** combines register and stack semantics  
- **Scoped frames:** each function call has its own locals  
- **Linkable assembly:** functions can be split across files and resolved at link time  
- **Minimal opcodes:** easy to extend or modify

Example assembly snippet:

```asm
.func main
    LOADC %r0, 42
    CALL print_num, %r0
.end
```
Bigger example in docs/example/factorial.detasm


---

## 🧪 Goals

- Be small, clean, and comprehensible  
- Serve as a foundation for compiler and language experiments  
- Stay portable — no dependencies beyond the STL  

---

## 🐚 License

MIT — do whatever you want, just keep the credit.

---

> “detvm isn’t meant to be a product — it’s a sandbox for ideas that want to run.”
