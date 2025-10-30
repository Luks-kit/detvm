// typecheck.hpp
#pragma once
#include "ast.hpp"

namespace deltora {

class TypeChecker {
public:
    void check(Program& program);
};

} // namespace deltora
