#pragma once

#include "Generator.h"

template<typename T>
class ConcatGenerator : public IGenerator<T> {
    IGenerator<T>* first;
    IGenerator<T>* second;
public:
    ConcatGenerator(IGenerator<T>* first, IGenerator<T>* second);
    ~ConcatGenerator();

    // без этого копирование по умолч скопировало бы указатели,
    // было бы двойное удаление. генераторы копируются через CloneFromStart()
    ConcatGenerator(const ConcatGenerator<T>& other) = delete;
    ConcatGenerator<T>& operator=(const ConcatGenerator<T>& other) = delete;

    // если first бесконечный, до second очередь не дойдет
    // так и должно быть (по методичке)
    bool HasNext() const override;
    T GetNext() override; // <-
    IGenerator<T>* CloneFromStart() const override;
};

template<typename T> // как и тэйк генератор для гетсабсиквенс
class SkipGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    int skipCount;
    // mutable тк пропуск делается в const HasNext()
    mutable bool skipped;
    // пропуск при первом обращении, чтоб было лениво
    void SkipIfNeeded() const;
public:
    SkipGenerator(IGenerator<T>* source, int skipCount);
    ~SkipGenerator();
    SkipGenerator(const SkipGenerator<T>& other) = delete;
    SkipGenerator<T>& operator=(const SkipGenerator<T>& other) = delete;

    bool HasNext() const override;
    T GetNext() override;
    IGenerator<T>* CloneFromStart() const override;
};

template<typename T>
class TakeGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    int takeCount;
    int given;
public:
    TakeGenerator(IGenerator<T>* source, int takeCount);
    ~TakeGenerator();
    TakeGenerator(const TakeGenerator<T>& other) = delete;
    TakeGenerator<T>& operator=(const TakeGenerator<T>& other) = delete;

    bool HasNext() const override;
    T GetNext() override;
    IGenerator<T>* CloneFromStart() const override;
};

template<typename T, typename T2>
class MapGenerator : public IGenerator<T2> {
    IGenerator<T>* source;
    T2 (*function)(const T&);
public:
    MapGenerator(IGenerator<T>* source, T2 (*function)(const T&));
    ~MapGenerator();
    MapGenerator(const MapGenerator<T, T2>& other) = delete;
    MapGenerator<T, T2>& operator=(const MapGenerator<T, T2>& other) = delete;

    bool HasNext() const override;
    T2 GetNext() override;
    IGenerator<T2>* CloneFromStart() const override;
};

// если источник бесконечный, а подходящих больше нет(например число меньшие 5), то FindNext() не закончится никогда. методичка это допускает
template<typename T>
class WhereGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    bool (*predicate)(const T&);
    // найденный, но ещё не отданный эл (mutable тк ищем в const HasNext())
    mutable Option<T> buffer;
    void FindNext() const;
public:
    WhereGenerator(IGenerator<T>* source, bool (*predicate)(const T&));
    ~WhereGenerator();
    WhereGenerator(const WhereGenerator<T>& other) = delete;
    WhereGenerator<T>& operator=(const WhereGenerator<T>& other) = delete;

    bool HasNext() const override;
    T GetNext() override;
    IGenerator<T>* CloneFromStart() const override;
};

#include "OperationGenerators.tpp"
