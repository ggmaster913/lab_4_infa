#ifndef OPTION_H
#define OPTION_H

#include <stdexcept>

template<typename T>
class Option {
    bool hasValue;
    T value; // есть и у пустой, поэтому T нужен конструктор по умолч
public:
    Option() : hasValue(false), value() {}
    explicit Option(const T& value) : hasValue(true), value(value){}
    bool HasValue() const {
        return hasValue;
    }
    const T& GetValue() const{
        if (!hasValue) throw std::out_of_range("Элемента нет");
        return value;
    }
};

#endif
