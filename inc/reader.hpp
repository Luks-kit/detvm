#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

class Reader {
public:
    Reader(const std::vector<uint8_t>& data) : data_(data), pos_(0) {}

    template <typename T>
    T read() {
        if (pos_ + sizeof(T) > data_.size())
            throw std::runtime_error("Unexpected EOF while reading");
        T value;
        std::memcpy(&value, data_.data() + pos_, sizeof(T));
        pos_ += sizeof(T);
        return value;
    }

    std::string readString(std::size_t len) {
        if (pos_ + len > data_.size())
            throw std::runtime_error("Unexpected EOF while reading string");
        std::string s(reinterpret_cast<const char*>(data_.data() + pos_), len);
        pos_ += len;
        return s;
    }

    void expect(const char* magic, std::size_t len) {
        std::string s = readString(len);
        if (s != std::string(magic, len))
            throw std::runtime_error("Invalid file magic: expected " + std::string(magic) + " but got" + s + "\n");
    }

    bool eof() const { return pos_ >= data_.size(); }

private:
    const std::vector<uint8_t>& data_;
    std::size_t pos_;
};
