#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <utility>

// ============================================================
//  Typed pointer with safe cast
// ============================================================

template<typename Base>
struct Ptr {
private:
    Base* ptr = nullptr;
public:
    Ptr() = default;
    explicit Ptr(Base* p) : ptr(p) {}
    Ptr(Ptr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
    Ptr& operator=(Ptr&& other) noexcept {
        if (this != &other) { reset(); ptr = other.ptr; other.ptr = nullptr; }
        return *this;
    }
    ~Ptr() { reset(); }

    Base* get() const { return ptr; }
    Base& operator*() const { return *ptr; }
    Base* operator->() const { return ptr; }
    explicit operator bool() const { return ptr != nullptr; }
    void reset(Base* p = nullptr) { if (ptr) delete ptr; ptr = p; }

    // Safe cast using first-member + runtime tag
    template<typename Derived, typename TagEnum>
    Ptr<Derived> as(TagEnum expected) const {
        if (ptr && ptr->kind == expected) {
            return Ptr<Derived>(reinterpret_cast<Derived*>(ptr));
        }
        return Ptr<Derived>(nullptr);
    }
};
