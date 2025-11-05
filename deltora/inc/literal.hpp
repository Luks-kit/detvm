#pragma once
#include "node_macros.hpp"

enum class LitKind { Long, Double, String, Char, Bool };

// ----------------- Base -----------------
struct LiteralBase {
    const LitKind kind;
    LiteralBase(LitKind k) : kind(k) {}
};

using LiteralPtr = Ptr<LiteralBase>;

// ----------------- Derived -----------------
struct LongLiteral   { LiteralBase base; long value;
    LongLiteral(long v) : base(LitKind::Long), value(v) {} };

struct DoubleLiteral { LiteralBase base; double value;
    DoubleLiteral(double v) : base(LitKind::Double), value(v) {} };

struct StringLiteral { LiteralBase base; std::string value;
    StringLiteral(const std::string& v) : base(LitKind::String), value(v) {} };

struct CharLiteral   { LiteralBase base; char value;
    CharLiteral(char v) : base(LitKind::Char), value(v) {} };

struct BoolLiteral   { LiteralBase base; bool value;
    BoolLiteral(bool v) : base(LitKind::Bool), value(v) {} };



inline LiteralPtr makeLongLiteral(long v)
{
    auto* l = new LongLiteral(v);
    return LiteralPtr(&l->base);
}

inline LiteralPtr makeDoubleLiteral(double v)
{
    auto* l = new DoubleLiteral(v);
    return LiteralPtr(&l->base);
}

inline LiteralPtr makeStringLiteral(const std::string& v)
{
    auto* l = new StringLiteral(v);
    return LiteralPtr(&l->base);
}

inline LiteralPtr makeCharLiteral(char v)
{
    auto* l = new CharLiteral(v);
    return LiteralPtr(&l->base);
}

inline LiteralPtr makeBoolLiteral(bool v)
{
    auto* l = new BoolLiteral(v);
    return LiteralPtr(&l->base);
}
