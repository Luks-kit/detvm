#include "constant_pool.hpp"
#include <cctype>

namespace detvm {

size_t ConstantPool::addInt(int32_t val) {
    entries.push_back({ConstType::INT, val});
    return entries.size() - 1;
}

size_t ConstantPool::addDouble(double val) {
    entries.push_back({ConstType::DOUBLE, val});
    return entries.size() - 1;
}

size_t ConstantPool::addString(const std::string& s) {
    if (auto it = string_to_index.find(s); it != string_to_index.end())
        return it->second;
    entries.push_back({ConstType::STRING, s});
    size_t idx = entries.size() - 1;
    string_to_index[s] = idx;
    return idx;
}

size_t ConstantPool::addChar(char c) {
    entries.push_back({ConstType::CHAR, c});
    return entries.size() -1;
}

bool ConstantPool::isInt(const std::string& s) {
     if (s.empty()) {
            return false;
        }
        size_t start_index = 0;
        if (s[0] == '-' || s[0] == '+') { // Handle optional sign
            start_index = 1;
        }
        for (size_t i = start_index; i < s.length(); ++i) {
            if (!isdigit(s[i])) {
                return false;
            }
        }
        return true;
}

bool ConstantPool::isFloat(const std::string& s){
    bool floating = false;
    size_t pos = 0;

    if (isdigit(s[pos])) {
        int start = pos;
        while (pos < s.size() && isdigit(s[pos])) pos++;
        if (pos < s.size() && s[pos] == '.') {
            floating = true;
            pos++;
            while (pos < s.size() && isdigit(s[pos])) pos++;
        }
    }
    return floating;
}

bool ConstantPool::isString(const std::string& s) {
    return s.size() >= 2 && s.front() == '"' && s.back() == '"';
}

std::string ConstantPool::stripQuotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}

size_t ConstantPool::add(const ConstantPoolEntry& entry) {
        // Try to find existing identical constant
        for (uint32_t i = 0; i < entries.size(); ++i) {
            if (entries[i] == entry)
                return i; // reuse existing constant
        }

        // Not found → add new entry
        entries.push_back(entry);
        return static_cast<uint32_t>(entries.size() - 1);
    }

} // namespace detvm

 