#pragma once

#include "Sequence.h"
#include "DynamicArray.h"
#include "LinkedList.h"

template<typename T>
class ArraySequence : public Sequence<T>{
protected:
    DynamicArray<T>* items;

    virtual ArraySequence<T>* Clone() const = 0; // копия меня: тот же тип и те же данные, нужна иммутабл в Instance()
    virtual ArraySequence<T>* Instance() = 0; // мутабл вернёт this, иммутабл копию
    virtual ArraySequence<T>* CreateInstance(const DynamicArray<T>& subArray) const = 0; // объект моего вида но с другими данными, для GetSubSequence и Where

    // Internal меняют сам объект, а какой объект менять решает Instance()
    ArraySequence<T>* AppendInternal(const T& item);
    ArraySequence<T>* PrependInternal(const T& item);
    ArraySequence<T>* InsertAtInternal(const T& item, int index);
    ArraySequence<T>* ConcatInternal(const Sequence<T>* list);
public:
    ArraySequence();
    ArraySequence(const T* data, int count);
    ArraySequence(const ArraySequence<T>& other);
    ArraySequence(const DynamicArray<T>& array);
    ArraySequence(const LinkedList<T>& list);

    // a = b = c
    ArraySequence<T>& operator=(const ArraySequence<T>& other);

    ~ArraySequence() override;

    const T& GetFirst() const override;
    const T& GetLast() const override;
    int GetLength() const override;
    ArraySequence<T>* Copy() const override; // Clone уже умеет копировать с нужным типом, Copy просто открывает его наружу
    const T& Get(int index) const override;

    ArraySequence<T>* Append(const T& item) override;
    ArraySequence<T>* Prepend(const T& item) override;
    ArraySequence<T>* InsertAt(const T& item, int index) override;
    ArraySequence<T>* Concat(const Sequence<T>* list) override;

    ArraySequence<T>* GetSubSequence(int startIndex, int endIndex) const override;

    Sequence<T>* Where(bool (*predicate)(const T&)) const override;
};

#include "ArraySequence.tpp"
