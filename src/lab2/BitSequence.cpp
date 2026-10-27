#include "BitSequence.h"
#include <stdexcept>

ArraySequence<Bit>* BitSequence::CreateInstance(const DynamicArray<Bit>& subArray) const {
    return new BitSequence(subArray);
}

ArraySequence<Bit>* BitSequence::Clone() const {
    return new BitSequence(*this);
}

BitSequence::BitSequence() : MutableArraySequence<Bit>() {} // указываем какой конструктор базы использовать

BitSequence::BitSequence(const Bit* data, int count) : MutableArraySequence<Bit>(data, count) {}

BitSequence::BitSequence(const DynamicArray<Bit>& array) : MutableArraySequence<Bit>(array) {}

BitSequence::BitSequence(const LinkedList<Bit>& list) : MutableArraySequence<Bit>(list) {}

// базу не указали, значит вызовется MutableArraySequence() и items будет пусто
BitSequence::BitSequence(const bool* values, int count){
    if (count < 0 || (count > 0 && values == nullptr)) throw std::invalid_argument("неправильные данные для битовой последовательности");
    for (int i = 0; i < count; i++){
        this->Append(Bit(values[i]));
    }
}

BitSequence* BitSequence::And(const BitSequence& other) const {
    if (this->GetLength() != other.GetLength()) throw std::invalid_argument("разная длина последовательностей");
    BitSequence* result = new BitSequence();
    for (int i = 0; i < other.GetLength(); i++) result->Append(this->Get(i) & other.Get(i));
    return result;
}

BitSequence* BitSequence::Or(const BitSequence& other) const{
    if (this->GetLength() != other.GetLength()) throw std::invalid_argument("разная длина последовательностей");
    BitSequence* result = new BitSequence();
    for (int i = 0; i < other.GetLength(); i++) result->Append(this->Get(i) | other.Get(i));
    return result;
}

BitSequence* BitSequence::Xor(const BitSequence& other) const{
    if (this->GetLength() != other.GetLength()) throw std::invalid_argument("разная длина последовательностей");
    BitSequence* result = new BitSequence();
    for (int i = 0; i < other.GetLength(); i++) result->Append(this->Get(i) ^ other.Get(i));
    return result;
}

BitSequence* BitSequence::Not() const {
    BitSequence* result = new BitSequence();
    int len = this->GetLength();
    for (int i = 0; i < len; i++) {
        result->Append(~this->Get(i)); // Get даёт const Bit&, поэтому operator~ обязан быть const
    }
    return result;
}

bool BitSequence::GetBit(int index) const {
    return this->Get(index).GetValue();
}

void BitSequence::SetBit(int index, bool value) {
    this->items->Set(index, Bit(value)); // items protected в ArraySequence, наследнику доступен
}
