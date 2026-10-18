#ifndef BITSEQUENCE_H
#define BITSEQUENCE_H

#include "MutableArraySequence.h"
#include "Bit.h"
#include <stdexcept>

class BitSequence : public MutableArraySequence<Bit> {
public:
    using MutableArraySequence<Bit>::MutableArraySequence;

    BitSequence(const bool* values, int count){
        for (int i = 0; i < count; i++){
            this->Append(Bit(values[i]));
        }
    }            

    BitSequence* And(const BitSequence& other) const {
        if (this->GetLength() != other.GetLength()) throw std::invalid_argument("разная длина последовательностей");
        BitSequence* result = new BitSequence();
        for (int i = 0; i < other.GetLength(); i++) result->Append(this->Get(i) & other.Get(i));
        return result;
    }
    BitSequence* Or(const BitSequence& other) const{
        if (this->GetLength() != other.GetLength()) throw std::invalid_argument("разная длина последовательностей");
        BitSequence* result = new BitSequence();
        for (int i = 0; i < other.GetLength(); i++) result->Append(this->Get(i) | other.Get(i));
        return result;
    }
    BitSequence* Xor(const BitSequence& other) const{
        if (this->GetLength() != other.GetLength()) throw std::invalid_argument("разная длина последовательностей");
        BitSequence* result = new BitSequence();
        for (int i = 0; i < other.GetLength(); i++) result->Append(this->Get(i) ^ other.Get(i));
        return result;
    }
    BitSequence* Not() const {
        BitSequence* result = new BitSequence();
        int len = this->GetLength();
        for (int i = 0; i < len; i++) {
            result->Append(~this->Get(i));
        }
        return result;
    }

    bool GetBit(int index) const {
        return this->Get(index).GetValue();
    }
    void SetBit(int index, bool value) {
        if (index < 0 || index >= this->GetLength()) {
            throw std::out_of_range("неправильный индекс");
        }
        this->items->Set(index, Bit(value));

    }
};

#endif 