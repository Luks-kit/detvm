#pragma once
#include <cstdint>

namespace detvm {

enum class Opcode : uint8_t {
    // Data & Arithmetic
    LOADC   = 0x01, // A=reg, B=const
    LOADL   = 0x02, // A=reg, B=local
    STOREL  = 0x03, // A=local, B=reg
    MOV     = 0x04, // A=dst, B=src
    ADD     = 0x05, // A=dst, B=src1, C=src2
    SUB     = 0x06, // A=dst, B=src1, C=src2
    MUL     = 0x07, // A=dst, B=src1, C=src2
    DIV     = 0x08, // A=dst, B=src1, C=src2
    NEG     = 0x09, // A=dst, B=src
    CMP     = 0x0A, // A=dst, B=src1, C=src2
    NOT     = 0x0B, // A=dst, B=src
    AND     = 0x0C, // A=dst, B=src1, C=src2
    OR      = 0x0D, // A=dst, B=src1, C=src2

    // Control Flow
    JMP     = 0x10, // A=label
    JZ      = 0x11, // A=cond, B=label
    JNZ     = 0x12, // A=cond, B=label
    JL      = 0x13, // A=cond, B=label
    JG      = 0x14, // A=cond, B=label

    // Function Call & Stack
    CALL    = 0x20, // A=func_index, B=argc
    RET     = 0x21, // A=reg
    ENTER   = 0x22, // A=local_count
    LEAVE   = 0x23, // —

    // Array & Memory
    NEWARR  = 0x30, // A=dst, B=elem_type, C=len
    LOADARR = 0x31, // A=dst, B=array, C=index
    STOREARR= 0x32, // A=array, B=index, C=value
    LEN     = 0x33, // A=dst, B=array
    FREE    = 0x34, // A=reg

    // Tags & Type Info
    TAG     = 0x40, // A=dst, B=tag_index
    WHEN    = 0x41, // A=tag_reg, B=table_index
    TYPEOF  = 0x42, // A=dst, B=reg

    // Misc
    NOP     = 0x50, // do nothing
    PRINT   = 0x51, // print reg (debug only)
    HALT    = 0x52, // stop execution

    // Ownership & Borrowing
    OWN     = 0x60, // dest, type_id, flags
    MOVE    = 0x61, // dest, src, flags=0
    VIEW    = 0x62, // dest, src, refcount delta (+1)
    EDIT    = 0x63, // dest, src, exclusive=1
    CLONE   = 0x64, // dest, src, 0
    DROP    = 0x65, // src, 0

    // Refcount & Safety
    INCREF      = 0x70, // increment refcount
    DECREF      = 0x71, // decrement refcount
    CHECKEXCL   = 0x72, // ensure exclusive (→edit) ref before writing
    CHECKLIVE   = 0x73, // verify reference is still alive
    RAIIDROP    = 0x74  // auto-drop owned value at scope exit
};

}