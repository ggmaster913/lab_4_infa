#ifndef BIT_H
#define BIT_H

class Bit {
    bool value;
public:
    Bit() : value(0) {}
    Bit(bool value) : value(value) {}
    explicit Bit(int value) : value(value != 0) {}

    bool GetValue() const {
        return value;
    }

    void SetValue(bool value) {
        this->value = value;
    }

    Bit operator&(const Bit& other) const {
        return Bit(this->value && other.value);
    }

    Bit operator|(const Bit& other) const {
        return Bit(this->value || other.value);
    }

    Bit operator^(const Bit& other) const {
        return Bit(this->value != other.value);
    }

    Bit operator~() const {
        return Bit(!this->value);
    }

    bool operator==(const Bit& other) const {
        return this->value == other.value;
    }   

};

#endif