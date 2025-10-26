#pragma once
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <cstdint>


namespace detvm {

enum class ConstType : uint8_t {
    INT = 1,
    FLOAT = 2,
    DOUBLE = 3,
    CHAR = 4,
    STRING = 5,
};


struct ConstantPoolEntry {
    ConstType type;
    std::variant<int64_t, double, std::string> value;

    bool operator==(const ConstantPoolEntry& other) const noexcept {
        if (type != other.type) return false;
        switch (type) {
            case ConstType::INT:
                return std::get<int64_t>(value) == std::get<int64_t>(other.value);
            case ConstType::DOUBLE:
                return std::get<double>(value) == std::get<double>(other.value);
            case ConstType::STRING:
                return std::get<std::string>(value) == std::get<std::string>(other.value);
            default:
                return false;
        }
    }
};

struct ConstantPool {
    std::vector<ConstantPoolEntry> entries;
    std::unordered_map<std::string, size_t> string_to_index;

    // === Adding constants ===
    size_t addInt(int64_t val);
    size_t addDouble(double val);
    size_t addString(const std::string& s);

    size_t add(const ConstantPoolEntry& e);
    // === Helpers ===
    inline size_t size() const { return entries.size(); }
    bool isInt(const std::string& s);
    bool isFloat(const std::string& s);
    bool isString(const std::string& s);
    std::string stripQuotes(const std::string& s);
};

} // namespace detvm
