#pragma once

#include "MutableArraySequence.h"
#include "Bit.h"

class BitSequence : public MutableArraySequence<Bit> {
protected:
    // иначе GetSubSequence и Where вернули бы MutableArraySequence<Bit>, а не BitSequence
    ArraySequence<Bit>* CreateInstance(const DynamicArray<Bit>& subArray) const override;
    // иначе Copy через Clone из MutableArraySequence вернул бы MutableArraySequence<Bit>
    ArraySequence<Bit>* Clone() const override;

public:

    BitSequence();
    BitSequence(const Bit* data, int count);
    BitSequence(const DynamicArray<Bit>& array);
    BitSequence(const LinkedList<Bit>& list);

    BitSequence(const bool* values, int count);

    // 1100 And 1010 = 1000
    BitSequence* And(const BitSequence& other) const;
    // 1100 Or 1010 = 1110
    BitSequence* Or(const BitSequence& other) const;
    // 1100 Xor 1010 = 0110
    BitSequence* Xor(const BitSequence& other) const;
    // Not 1100 = 0011
    BitSequence* Not() const;

    // индекс проверит Get
    bool GetBit(int index) const;
    // индекс проверит Set
    void SetBit(int index, bool value);
};
