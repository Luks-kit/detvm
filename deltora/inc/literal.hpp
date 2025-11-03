#pragma once
#include "node_macros.hpp"

DEFINE_NODE_BASE(Literal, LitType {Long, Double, String, Char, Bool})

DEFINE_NODE_TYPE(Literal, LongLiteral, Kind::Long, 
long value;
)

DEFINE_NODE_TYPE(Literal, DoubleLiteral, Kind::Double,
double value;
)

DEFINE_NODE_TYPE(Literal, StringLiteral, Kind::String,
std::string value;
)

DEFINE_NODE_TYPE(Literal, CharLiteral, Kind::Char,
char value;
)

DEFINE_NODE_TYPE(Literal, BoolLiteral, Kind::Bool, 
bool value;
)

BEGIN_NODE_TABLE(Literal)
    NODE_ENTRY(LongLiteral)
    NODE_ENTRY(DoubleLiteral)
    NODE_ENTRY(StringLiteral)
    NODE_ENTRY(CharLiteral)
    NODE_ENTRY(BoolLiteral)
END_NODE_TABLE(Literal)

DEFINE_NODE_FACTORY(Literal, LongLiteral, Kind::Long,
 [&](LongLiteral* l) {l->value = input;},
 long input
)

DEFINE_NODE_FACTORY(Literal, DoubleLiteral, Kind::Double,
 [&](DoubleLiteral* l) {l->value = input;},
 double input
)

DEFINE_NODE_FACTORY(Literal, StringLiteral, Kind::String,
 [&](StringLiteral* l) {l->value = input;},
 const std::string& input
)

DEFINE_NODE_FACTORY(Literal, CharLiteral, Kind::Char,
 [&](CharLiteral* l) {l->value = input;},
 char input
)

DEFINE_NODE_FACTORY(Literal, BoolLiteral, Kind::Bool,
 [&](BoolLiteral* l) {l->value = input;},
 bool input
)