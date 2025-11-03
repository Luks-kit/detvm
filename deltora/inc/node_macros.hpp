#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <utility>

// ============================================================
//  Base Node Definition Macros
// ============================================================

#define DEFINE_NODE_BASE(BASE, ...) \
struct BASE { \
    using Kind = enum struct __VA_ARGS__; \
    Kind kind; \
}; \
 \ 
struct BASE##Ptr { \
    private: \
        BASE* ptr = nullptr; \
    public: \
        explicit BASE##Ptr(BASE* p = nullptr) : ptr(p) {} \
        BASE##Ptr(BASE##Ptr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; } \
        BASE##Ptr& operator=(BASE##Ptr&& other) noexcept { \
            if (this != &other) { reset(); ptr = other.ptr; other.ptr = nullptr; } \
            return *this; \
        } \
        ~BASE##Ptr() { reset(); } \
        BASE* get() const { return ptr; } \
        BASE& operator*() const { return *ptr; } \
        BASE* operator->() const { return ptr; } \
        explicit operator bool() const { return ptr != nullptr; } \
        void reset(BASE* p = nullptr); \
    }; \
inline void BASE##Ptr::reset(BASE* p) { if (ptr) delete_##BASE(ptr); ptr = p; } \
inline void delete_##BASE(BASE* n); \
using BASE##DeleterFn = void(*)(BASE* n);

// ============================================================
//  Derived Node Definition
// ============================================================

#define DEFINE_NODE_TYPE(BASE, TYPE, ENUMVAL, ...) \
    struct TYPE { BASE base; static constexpr BASE##::Kind kind = ##BASE::ENUMVAL; __VA_ARGS__ }; \
    inline TYPE* as_##TYPE(BASE* n) { \
        return (n && n->kind ==  ##TYPE::kind) ? reinterpret_cast<TYPE*>(n) : nullptr; \
    } \
    inline void delete_##TYPE(BASE* n) { delete as_##TYPE(n); }

// ============================================================
//  Node Table Construction
// ============================================================

#define BEGIN_NODE_TABLE(BASE) \
    inline BASE##DeleterFn BASE##_deleter_table[] = {

#define NODE_ENTRY(TYPE) delete_##TYPE,

#define END_NODE_TABLE(BASE) }; \
inline void delete_##BASE(BASE* n) { \
    if (n) BASE##_deleter_table[(int)n->kind](n); \
} \



// ============================================================
//  Factory Creation Macro
// ============================================================

#define DEFINE_NODE_FACTORY(BASE, TYPE, ENUMVAL, INITCODE, ...) \
    inline BASE##Ptr make_##TYPE(__VA_ARGS__) { \
        auto* n = new TYPE(); \
        n->base.kind = ##BASE::ENUMVAL; \
        INITCODE(n); \
        return BASE##Ptr(&n->base); \
    }


    