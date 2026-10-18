#ifndef STREAM_H
#define STREAM_H

#include <stdexcept>
#include "Ordinal.h"
#include "LazySequence.h"
#include "lab2/Sequence.h"

// поток не владеет источником, поэтому источник должен жить дольше потока
template<typename T>
class ReadOnlyStream {
    int position;
    bool isOpen;
protected:
    // чем потоки отличаются, реализуют наследники (как Instance и Clone в ArraySequence)
    virtual bool HasElementAt(int index) const = 0;
    virtual T ReadAt(int index) = 0;
public: 
    ReadOnlyStream() : position(0), isOpen(false) {}
    virtual ~ReadOnlyStream() = default;

    void Open() {
        isOpen = true;
        position = 0;
    }
    void Close() {
        isOpen = false;
    }
    bool IsEndOfStream() const {
        return !HasElementAt(position);
    }
    T Read() {
        if (!isOpen) throw std::out_of_range("поток закрыт");
        if (IsEndOfStream()) throw std::out_of_range("достигнут конец потока");
        T item = ReadAt(position);
        position++;
        return item;
    }
    int GetPosition() const {
        return position;
    }
    bool IsCanSeek() const {
        return true;
    }
    int Seek(int index) {
        if (!isOpen) throw std::out_of_range("поток закрыт");
        if (index < 0 || !HasElementAt(index)) throw std::out_of_range("позиция недостижима");
        position = index;
        return position; 
    }
    bool IsCanGoBack() const {
        return true;
    }
};

template<typename T>
class SequenceReadStream : public ReadOnlyStream<T> {
    const Sequence<T>* source;
protected:
    bool HasElementAt(int index) const override {
        return index < source->GetLength();
    }
    T ReadAt(int index) override {
        return source->Get(index);
    }
public: 
    explicit SequenceReadStream(const Sequence<T>* source) : source(source){
        if (source == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
    }
};

template<typename T> 
class LazyReadStream : public ReadOnlyStream<T> {
    LazySequence<T>* source;
protected:
    bool HasElementAt(int index) const override {
        return Ordinal(index) < source->GetLength(); // у бесконечной всегда true, конец потока не наступит
    }
    T ReadAt(int index) override {
        return source->Get(index);
    }
public:
    explicit LazyReadStream(LazySequence<T>* source) : source(source) {
        if (source == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
    }
};

template<typename T>
class WriteOnlyStream {
    int position;
    bool isOpen;
protected:
    virtual void WriteItem(const T& item) = 0;
public:
    WriteOnlyStream() : position(0), isOpen(false) {}
    virtual ~WriteOnlyStream() = default;

    void Open() {
        isOpen = true; // позицию не сбрасываем: записанное никуда не делось
    }
    void Close() {
        isOpen = false;
    }
    int GetPosition() const {
        return position;
    }
    int Write(const T& item) {
        if (!isOpen) throw std::out_of_range("поток закрыт");
        WriteItem(item);
        position++; // после WriteItem, чтоб при ошибке записи счётчик не сдвинулся
        return position;
    }
};

template<typename T>
class SequenceWriteStream : public WriteOnlyStream<T> {
    Sequence<T>* target;
protected:
    void WriteItem(const T& item) override {
        // у иммутабл Append вернёт новый объект. подменить target нельзя, он не наш,
        // поэтому удаляем копию и ругаемся, чтоб не было утечки
        Sequence<T>* result = target->Append(item);
        if (result != target) {
            delete result;
            throw std::invalid_argument("поток записи работает только с изменяемой последовательностью");
        }
    }
public:
    explicit SequenceWriteStream(Sequence<T>* target) : target(target) {
        if (target == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
    }
};

#endif 