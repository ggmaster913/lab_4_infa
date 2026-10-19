#ifndef ARRAYSEQUENCE_H
#define ARRAYSEQUENCE_H

#include "Sequence.h"
#include "DynamicArray.h"
#include "LinkedList.h"
#include <stdexcept>

template<typename T>
class ArraySequence : public Sequence<T>{
protected:
    DynamicArray<T>* items;

    virtual ArraySequence<T>* Clone() const = 0;
    virtual ArraySequence<T>* Instance() = 0; // мутабл вернёт this, иммутабл копию
    virtual ArraySequence<T>* CreateInstance(const DynamicArray<T>& subArray) const = 0;

    // Internal меняют сам объект, а какой объект менять решает Instance()
    ArraySequence<T>* AppendInternal(const T& item ) {
        items->Resize(items->GetSize() + 1);
        items->Set(items->GetSize() - 1, item);
        return this;
    }
    ArraySequence<T>* PrependInternal(const T& item ){
        int oldSize = items->GetSize();
        items->Resize(oldSize + 1);
        for (int i = oldSize; i > 0; i--){
            items->Set(i, items->Get(i - 1));
        }
        items->Set(0, item);
        return this;
    }

    ArraySequence<T>* InsertAtInternal(const T& item, int index){
        if (index < 0 || index >= items->GetSize()) throw std::out_of_range("неправильный индекс");
        int oldSize = items->GetSize();
        items->Resize(oldSize + 1);
        for (int i = oldSize; i > index; i--){
            items->Set(i, items->Get(i - 1));
        }
        items->Set(index, item);
        return this;
    }

    ArraySequence<T>* ConcatInternal(const Sequence<T>* list){
        if (list == nullptr) throw std::invalid_argument("неправильный аргумент");
        int oldSize = items->GetSize();
        int otherSize = list->GetLength();
        items->Resize(oldSize + otherSize);
        for (int i = 0; i < otherSize; i++){
            items->Set(oldSize + i, list->Get(i));
        }
        return this;
    }
public:
    ArraySequence(){
        items = new DynamicArray<T>(0);
    }

    ArraySequence(const T* data, int count){
        items = new DynamicArray<T>(data, count);
    }

    ArraySequence(const ArraySequence<T>& other) {
        items = new DynamicArray<T>(*(other.items));
    }

    ArraySequence(const DynamicArray<T>& array) {
    items = new DynamicArray<T>(array);
    }

    ArraySequence(const LinkedList<T>& list){
        int count = list.GetLength();
        items = new DynamicArray<T>(count);
        for (int i = 0; i < count; i++){
            items->Set(i, list.Get(i));
        }
    }

    ArraySequence<T>& operator=(const ArraySequence<T>& other){
        if (this == &other) return *this;

        *items = *(other.items);// уже готовый оператор= из дайнамикэррэй

        return *this;
    }

    ~ArraySequence() override{
        delete items;
    }

    const T& GetFirst() const override {
        return items->Get(0);
    } 

    const T& GetLast() const override {
        return items->Get(items->GetSize() - 1);
    }

    int GetLength() const override {
        return items->GetSize();
    }

    const T& Get(int index) const override {
        return items->Get(index);
    }

    ArraySequence<T>* Append(const T& item) override {
        return this->Instance()->AppendInternal(item);
    }

    ArraySequence<T>* Prepend(const T& item) override {
        return this->Instance()->PrependInternal(item);
    }
    
    ArraySequence<T>* InsertAt(const T& item, int index) override {
        return this->Instance()->InsertAtInternal(item, index);
    } 
    
    ArraySequence<T>* Concat(const Sequence<T>* list) override {
        return this->Instance()->ConcatInternal(list);
    }

    Sequence<T>* GetSubSequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || startIndex >= items->GetSize() ||
            endIndex < 0 || endIndex >= items->GetSize() || startIndex > endIndex) {
            throw std::out_of_range("неправильный индекс");
        }
        
        int subLength = endIndex - startIndex + 1;
        DynamicArray<T> sub(subLength);
        for (int i = 0; i < subLength; i++) {
            sub.Set(i, items->Get(startIndex + i));
        }

        return this->CreateInstance(sub);
    }

    Sequence<T>* Where(bool (*predicate)(const T&)) const override {
        int length = 0; // сначала считаем длину, чтоб не растить массив по одному
        for (int i = 0; i < items->GetSize(); i++) {
            if (predicate(items->Get(i))) length++;
        }
        DynamicArray<T> temp(length);
        int count = 0;
        for (int i = 0; i < items->GetSize(); i++){
            if (predicate(items->Get(i))) {
                temp.Set(count, items->Get(i));
                count++;
            }
        }
        return this->CreateInstance(temp);
    }


};




#endif