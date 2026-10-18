#ifndef LAZYSEQUENCE_H
#define LAZYSEQUENCE_H

#include <stdexcept>
#include "Generator.h"
#include "Ordinal.h"
#include "OperationGenerators.h"
#include "lab2/DynamicArray.h"
#include "lab2/Sequence.h"

template<typename T>
class LazySequence {
    IGenerator<T>* generator;
    DynamicArray<T> cache; //посчитанные эл
    int materializedCount; //сколько ячеек кэша реально заполнено
    Ordinal length;
public:
    LazySequence() : 
generator(new ArrayGenerator<T>(DynamicArray<T>(0))), cache(0), materializedCount(0), length(Ordinal()) {}
    LazySequence(const T* items, int count) : 
generator(new ArrayGenerator<T>(DynamicArray<T>(items, count))), cache(0), materializedCount(0), length(Ordinal(count)) {}
    
    explicit LazySequence(const Sequence<T>* sequence) : 
generator(new ArrayGenerator<T>(sequence)), cache(0), materializedCount(0), length(Ordinal(sequence->GetLength())) {}
    LazySequence(T (*rule)(const Sequence<T>*), const Sequence<T>* initial) : 
generator(new RuleGenerator<T>(rule, initial)), cache(0), materializedCount(0), length(Ordinal::Omega()) {}

    LazySequence(IGenerator<T>* generator, const Ordinal& length) : generator(generator), cache(0), materializedCount(0), length(length){
        if (generator == nullptr) throw std::invalid_argument("нулевой указатель");
    }

    LazySequence(const LazySequence<T>& other) : 
generator(other.generator->CloneFromStart()), cache(0), materializedCount(0), length(other.length) {}

    ~LazySequence() {
        delete generator;
    }

    Ordinal GetLength() const {
        return length;
    }

    int GetMaterializedCount() const {
        return materializedCount;
    }

    T Get(int index) {
        if (index < 0) throw std::out_of_range("Индекс не может быть меньше 0");
        if (Ordinal(index) >= length) throw std::out_of_range("индекс слишком большой"); //оборачиваем индеск в ординал чтобы можно было сравнить 
        while (materializedCount <= index) {
            if (materializedCount == cache.GetSize()) {
                if (materializedCount == 0)cache.Resize(1);
                else cache.Resize(cache.GetSize() * 2);
            }
            cache.Set(materializedCount, generator->GetNext());
            materializedCount++;
        }
        return cache.Get(index);
    }

    T Get(const Ordinal& index) {
        if (index.IsFinite()) return Get(index.GetFinitePart());
        throw std::out_of_range("невозможно получить бесконечный элемент");
    }

    

    LazySequence<T>& operator=(const LazySequence<T>& other) {
        if (this == &other) return *this;
        IGenerator<T>* newGenerator = other.generator->CloneFromStart();
        delete generator;
        generator = newGenerator;
        cache = DynamicArray<T>(0);
        materializedCount = 0;
        length = other.length;
        return *this;
    }

    T GetFirst() {
        if (length == Ordinal()) throw std::out_of_range("последовательность пустая");
        return Get(0);
    }

    T GetLast() {
        if (length == Ordinal()) throw std::out_of_range("последовательность пустая");
        if (!length.IsFinite()) throw std::out_of_range("у бесконечной последовательности нет последнего элемента");
        return Get(length.GetFinitePart() - 1);
    }

    LazySequence<T>* Concat(const LazySequence<T>* other) const {
        if (other == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
        IGenerator<T>* combined = new ConcatGenerator<T>(generator->CloneFromStart(), other->generator->CloneFromStart());
        return new LazySequence<T>(combined, length + other->length);
    }

    LazySequence<T>* Append(const T& item) const {
        LazySequence<T> single(&item, 1); // &item - адрес одного элемента
        return Concat(&single);
    }

    LazySequence<T>* Prepend(const T& item) const {
        LazySequence<T> single(&item, 1);
        return single.Concat(this);
    }

    LazySequence<T>* GetSubSequence(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex < startIndex || Ordinal(endIndex) >= length) throw std::out_of_range("некорректные индексы");
        int count = endIndex - startIndex + 1;
        // {0, 1, 2, 3, 4, 5, ...}, st = 2, end = 4, count = 3 -> {2, 3, 4, 5, ...} -> {2, 3, 4}; 
        IGenerator<T>* tempGen = new TakeGenerator<T>(new SkipGenerator<T>(generator->CloneFromStart(), startIndex), count);
        return new LazySequence<T>(tempGen, Ordinal(count));

    }

    LazySequence<T>* InsertAt(const T& item, int index) const {
        if (index < 0 || Ordinal(index) >= length) throw std::out_of_range("некорректный индекс");
        IGenerator<T>* first = new TakeGenerator<T>(generator->CloneFromStart(), index);
        IGenerator<T>* single = new ArrayGenerator<T>(DynamicArray<T>(&item, 1));
        IGenerator<T>* second = new SkipGenerator<T>(generator->CloneFromStart(), index);
        IGenerator<T>* tempGen = new ConcatGenerator<T>(new ConcatGenerator<T>(first, single), second);
        return new LazySequence<T>(tempGen, Ordinal(1) + length);
    }

    template<typename T2>
    LazySequence<T2>* Map(T2 (*function)(const T&)) const {
        if (function == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
        IGenerator<T2>* tempGen = new MapGenerator<T, T2>(generator->CloneFromStart(), function);
        return new LazySequence<T2>(tempGen, length);
    }

    // доразобраться 
    LazySequence<T>* Where(bool (*predicate)(const T&)) const {
        if (predicate == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
        Ordinal resultLength;
        if (!length.IsFinite()) {
            resultLength = Ordinal::Omega();
        }
        else {
            int count = 0;
            IGenerator<T>* counter = generator->CloneFromStart();
            while (counter->HasNext()) {
                if (predicate(counter->GetNext())) {
                    count++;
                }
            }
            delete counter; 
            resultLength = Ordinal(count);
        }
    IGenerator<T>* whereGen = new WhereGenerator<T>(generator->CloneFromStart(), predicate);
    return new LazySequence<T>(whereGen, resultLength);
    }

    template<typename T2>
    T2 Reduce(T2 (*function)(const T2&, const T&), const T2& initial) {
        if (function == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
        if (!length.IsFinite()) throw std::out_of_range("нельзя свернуть бесконечную последовательность");
        T2 accumulator = initial;
        for (int i = 0; i < length.GetFinitePart(); i++) {
            accumulator = function(accumulator, Get(i));
        }
        return accumulator;
    }
};  

#endif 