#ifndef OPERATIONGENERATORS_H
#define OPERATIONGENERATORS_H

#include <stdexcept>
#include "Generator.h"

template<typename T>
class ConcatGenerator : public IGenerator<T> {
    IGenerator<T>* first;
    IGenerator<T>* second;
public: 
    ConcatGenerator(IGenerator<T>* first, IGenerator<T>* second) : first(first), second(second) {
        if (first == nullptr || second == nullptr) {
            delete this->first;
            delete this->second;
            throw std::invalid_argument("часть конкатенации не может быть nullptr");
        }
    }
    ~ConcatGenerator() {
        delete first;
        delete second;
    }

    // без этого копирование по умолч скопировало бы указатели, 
    // было бы двойное удаление. генераторы копируются через CloneFromStart()
    ConcatGenerator(const ConcatGenerator<T>& other) = delete;
    ConcatGenerator<T>& operator=(const ConcatGenerator<T>& other) = delete;

    // если first бесконечный, до second очередь не дойдет 
    // так и должно быть (по методичке)
    bool HasNext() const override {
        return first->HasNext() || second->HasNext();
    }
    T GetNext() override {
        if (first->HasNext()) return first->GetNext();
        return second->GetNext();
    }
    IGenerator<T>* CloneFromStart() const override {
        return new ConcatGenerator<T>(first->CloneFromStart(), second->CloneFromStart());
    }
};

template<typename T>
class SkipGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    int skipCount;
    mutable bool skipped;
    void SkipIfNeeded() const {
        if (skipped == true) return;
        for (int i = 0; i < skipCount; i++) {
            if (source->HasNext()) source->GetNext();
        }
        skipped = true;
    }
public:
    SkipGenerator(IGenerator <T>* source, int skipCount) : source(source), skipCount(skipCount), skipped(false) {
        if (source == nullptr) throw std::invalid_argument("указатель не может быть пустым");
        if (skipCount < 0) {
            delete source;
            throw std::invalid_argument("skipCount не может быть меньше нуля");
        }
    }
    ~SkipGenerator() {
        delete source;
    }
    SkipGenerator(const SkipGenerator<T>& other) = delete;
    SkipGenerator<T>& operator=(const SkipGenerator<T>& other) = delete;

    bool HasNext() const override{
        SkipIfNeeded();
        return source->HasNext();
    }
    T GetNext() override{
        SkipIfNeeded();
        return source->GetNext();
    }
    IGenerator<T>* CloneFromStart() const override {
        return new SkipGenerator<T>(source->CloneFromStart(), skipCount);
    }
};

template<typename T>
class TakeGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    int takeCount;
    int given;
public:
    TakeGenerator(IGenerator<T>* source, int takeCount) : source(source), takeCount(takeCount), given(0) {
        if (source == nullptr) throw std::invalid_argument("указатель не может быть равен нулю");
        if (takeCount < 0) {
            delete source;
            throw std::invalid_argument("takeCount не может быть меньше нуля");
        }
    }
    ~TakeGenerator() {
        delete source;
    }
    TakeGenerator(const TakeGenerator<T>& other) = delete;
    TakeGenerator<T>& operator=(const TakeGenerator<T>& other) = delete;

    bool HasNext() const override {
        return given < takeCount && source->HasNext();
    }
    T GetNext() override {
        if (!this->HasNext()) throw std::out_of_range("нету следущего элемента");
        given++;
        return source->GetNext();
    }
    IGenerator<T>* CloneFromStart() const override {
        return new TakeGenerator<T>(source->CloneFromStart(), takeCount);
    }
};

template<typename T, typename T2>
class MapGenerator : public IGenerator<T2> {
    IGenerator<T>* source;
    T2 (*function)(const T&);
public: 
    MapGenerator(IGenerator<T>* source, T2 (*function)(const T&)) : source(source), function(function) {
        if (source == nullptr) throw std::invalid_argument("указатель не может быть равен nullptr"); 
        if (function == nullptr) {
            delete source;
            throw std::invalid_argument("указатель на функцию не может быть nullptr");  
        }
    }    
    ~MapGenerator() {
        delete source;
    }
    MapGenerator(const MapGenerator<T , T2>& other) = delete;
    MapGenerator<T, T2>& operator=(const MapGenerator<T, T2>& other) = delete;

    bool HasNext() const override {
        return source->HasNext();
    }
    T2 GetNext() override {
        return function(source->GetNext());
    }
    IGenerator<T2>* CloneFromStart() const override {
        return new MapGenerator<T, T2>(source->CloneFromStart(), function);
    }
};

// если источник бесконечный, а подходящих больше нет(например число меньшие 5), то FindNext() не закончится никогда. методичка это допускает 
template<typename T>
class WhereGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    bool (*predicate)(const T&);
    mutable Option<T> buffer;
    void FindNext() const {
        if (buffer.HasValue()) return;
        while (source->HasNext()) {
            T item = source->GetNext();
            if (predicate(item)) {
                buffer = Option<T>(item);
                return;
            }
        }
    }
public:
    WhereGenerator(IGenerator<T>* source, bool (*predicate)(const T&)) : source(source), predicate(predicate), buffer() {
        if (source == nullptr) throw std::invalid_argument("указатель не может быть равен nullptr"); 
        if (predicate == nullptr) {
            delete source;
            throw std::invalid_argument("указатель на функцию не может быть nullptr");  
        }
    }
    ~WhereGenerator() {
        delete source;
    }
    WhereGenerator(const WhereGenerator<T>& other) = delete;
    WhereGenerator<T>& operator=(const WhereGenerator<T>& other) = delete;

    bool HasNext() const override {
        FindNext();
        return buffer.HasValue();
    }
    T GetNext() override {
        FindNext();
        if (!buffer.HasValue()) throw std::out_of_range("элементы кончились");
        T result = buffer.GetValue();
        buffer = Option<T>();
        return result;
    }
    IGenerator<T>* CloneFromStart() const override {
        return new WhereGenerator<T>(source->CloneFromStart(), predicate);
    }
};

#endif