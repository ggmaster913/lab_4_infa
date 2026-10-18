#ifndef LISTSEQUENCE_H
#define LISTSEQUENCE_H

#include "Sequence.h"
#include "LinkedList.h"
#include "DynamicArray.h"
#include <stdexcept>

template<typename T>
class ListSequence : public Sequence<T>{
protected:
    LinkedList<T>* items;

    virtual ListSequence<T>* Clone() const = 0;
    virtual ListSequence<T>* Instance() = 0;
    virtual ListSequence<T>* CreateInstance(const LinkedList<T>& subList) const = 0;

    ListSequence<T>* AppendInternal(const T& item){
        items->Append(item);
        return this;
    }

    ListSequence<T>* PrependInternal(const T& item){
        items->Prepend(item);
        return this;
    }

    ListSequence<T>* InsertAtInternal(const T& item, int index){
        items->InsertAt(item, index);
        return this;
    }

    ListSequence<T>* ConcatInternal(const Sequence<T>* list){
        if (list == nullptr) throw std::invalid_argument("неправильный аргумент");
        int otherSize = list->GetLength();
        for (int i = 0; i < otherSize; i++) {
            items->Append(list->Get(i));
        }
        return this;
    }
public:
    ListSequence(){
        items = new LinkedList<T>();
    }

    ListSequence(const T* data, int count){
        items = new LinkedList<T>(data, count);
    }

    ListSequence(const ListSequence<T>& other){
        items = new LinkedList<T>(*(other.items));
    }

    ListSequence(const LinkedList<T>& other){
        items = new LinkedList<T>(other);
    }

    ListSequence(const DynamicArray<T>& other){
        int count = other.GetSize();
        items = new LinkedList<T>();
        for(int i = 0; i < count; i++){
            items->Append(other.Get(i));
        }
    }

    ListSequence<T>& operator=(const ListSequence<T>& other){
        if (this == &other) return *this;

        *items = *(other.items);

        return *this;
    }

    ~ListSequence() override {
        delete items;
    }

    const T& GetFirst() const override {
        return items->GetFirst();
    }

    const T& GetLast() const override {
        return items->GetLast();
    }

    int GetLength() const override {
        return items->GetLength();
    }

    const T& Get(int index) const override {
        return items->Get(index);
    }

    ListSequence<T>* Append(const T& item) override {
        return this->Instance()->AppendInternal(item);
    }

    ListSequence<T>* Prepend(const T& item) override {
        return this->Instance()->PrependInternal(item);
    }

    ListSequence<T>* InsertAt(const T& item, int index) override {
        return this->Instance()->InsertAtInternal(item, index);
    }

    ListSequence<T>* Concat(const Sequence<T>* other) override{
        return this->Instance()->ConcatInternal(other);
    }

    ListSequence<T>* GetSubSequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || startIndex >= items->GetLength() ||
            endIndex < 0 || endIndex >= items->GetLength() || startIndex > endIndex) {
            throw std::out_of_range("неправильный индекс");
        }

        LinkedList<T>* subPtr = items->GetSubList(startIndex, endIndex);
        ListSequence<T>* result = this->CreateInstance(*subPtr);
        delete subPtr;
        return result;
    }

    Sequence<T>* Where(bool (*predicate)(const T&)) const override {
        LinkedList<T> temp;
        for (int i = 0; i < items->GetLength(); i++){
            if (predicate(items->Get(i))) temp.Append(items->Get(i));
        }
        return this->CreateInstance(temp);
    }

};

#endif