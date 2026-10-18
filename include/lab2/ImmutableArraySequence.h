#ifndef IMMUTABLEARRAYSEQUENCE_H
#define IMMUTABLEARRAYSEQUENCE_H

#include "ArraySequence.h"

template<typename T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    ArraySequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }

    ArraySequence<T>* Instance() override {
        return this->Clone();
    }

    ArraySequence<T>* CreateInstance(const DynamicArray<T>& subArray) const override{
        return new ImmutableArraySequence<T>(subArray);
    }

public:
    using ArraySequence<T>::ArraySequence; //перетаскиваем конструкторы из ArraySequence только теперь для IMmutableArraySequence 

};

#endif