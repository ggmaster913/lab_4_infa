#ifndef IMMUTABLELISTSEQUENCE_H
#define IMMUTABLELISTSEQUENCE_H

#include "ListSequence.h"

template<typename T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    ListSequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

    ListSequence<T>* Instance() override {
        return this->Clone();
    }

    ListSequence<T>* CreateInstance(const LinkedList<T>& subArray) const override{
        return new ImmutableListSequence<T>(subArray);
    }

public:
    using ListSequence<T>::ListSequence; //перетаскиваем конструкторы из ArraySequence только теперь для IMmutableArraySequence 

};

#endif