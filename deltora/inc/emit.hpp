// emit.hpp
#pragma once
#include "ast.hpp"

namespace deltora {

class Emitter {
public:
    void emit(const Program& program);
};

} // namespace deltora
