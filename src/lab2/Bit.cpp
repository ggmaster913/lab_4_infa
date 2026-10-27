#include "Bit.h"

Bit::Bit() : value(0) {}

Bit::Bit(bool value) : value(value) {}

Bit::Bit(int value) : value(value != 0) {}

bool Bit::GetValue() const {
    return value;
}

void Bit::SetValue(bool value) {
    this->value = value;
}

Bit Bit::operator&(const Bit& other) const {
    return Bit(this->value && other.value);
}

Bit Bit::operator|(const Bit& other) const {
    return Bit(this->value || other.value);
}

Bit Bit::operator^(const Bit& other) const {
    return Bit(this->value != other.value);
}

Bit Bit::operator~() const {
    return Bit(!this->value);
}

bool Bit::operator==(const Bit& other) const {
    return this->value == other.value;
}
