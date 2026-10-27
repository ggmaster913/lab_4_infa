#pragma once

template<typename T>
class Option {
    bool hasValue;
    T value; // есть и у пустой, поэтому T нужен конструктор по умолч
public:
    Option();
    explicit Option(const T& value);
    bool HasValue() const;
    const T& GetValue() const;
};

#include "Option.tpp"
