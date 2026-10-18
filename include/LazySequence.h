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

    // длину можно считать прямо в списке: generator объявлен раньше и уже проверил nullptr
    explicit LazySequence(const Sequence<T>* sequence) :
        generator(new ArrayGenerator<T>(sequence)), cache(0), materializedCount(0), length(Ordinal(sequence->GetLength())) {}

    LazySequence(T (*rule)(const Sequence<T>*), const Sequence<T>* initial) :
        generator(new RuleGenerator<T>(rule, initial)), cache(0), materializedCount(0), length(Ordinal::Omega()) {}

    // забирает генератор себе и удалит его в деструкторе, передавший его больше не трогает
    LazySequence(IGenerator<T>* generator, const Ordinal& length) :
        generator(generator), cache(0), materializedCount(0), length(length) {
        if (generator == nullptr) throw std::invalid_argument("нулевой указатель");
    }

    // кэш не копируем: новый генератор стоит в начале, и кэш с ним бы разошёлся
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
        if (Ordinal(index) >= length) throw std::out_of_range("индекс слишком большой"); //оборачиваем индекс в ординал чтобы можно было сравнить
        while (materializedCount <= index) {
            // растим кэш вдвое, а не на 1, иначе на миллионе эл будет n^2 копирований.
            // решает LazySequence, DynamicArray::Resize меняет размер ровно до запрошенного
            if (materializedCount == cache.GetSize()) {
                if (materializedCount == 0) cache.Resize(1);
                else cache.Resize(cache.GetSize() * 2);
            }
            cache.Set(materializedCount, generator->GetNext());
            materializedCount++;
        }
        return cache.Get(index);
    }

    // к ω и дальше обратиться нельзя: перечисление дойдёт туда только в бесконечности
    T Get(const Ordinal& index) {
        if (index.IsFinite()) return Get(index.GetFinitePart());
        throw std::out_of_range("невозможно получить бесконечный элемент");
    }

    LazySequence<T>& operator=(const LazySequence<T>& other) {
        if (this == &other) return *this;
        // сначала новый, потом удаляем старый: если клон кинет исключение, объект останется целым
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
        // у ω нет предыдущего, поэтому и последнего эл нет
        if (!length.IsFinite()) throw std::out_of_range("у бесконечной последовательности нет последнего элемента");
        return Get(length.GetFinitePart() - 1);
    }

    LazySequence<T>* Concat(const LazySequence<T>* other) const {
        if (other == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
        IGenerator<T>* combined = new ConcatGenerator<T>(generator->CloneFromStart(), other->generator->CloneFromStart());
        return new LazySequence<T>(combined, length + other->length); // порядок важен: ω + 1 != 1 + ω
    }

    LazySequence<T>* Append(const T& item) const {
        LazySequence<T> single(&item, 1); // &item - адрес одного элемента
        return Concat(&single); // this ++ [item]. single потом удалится, но Concat уже скопировал его генератор
    }

    LazySequence<T>* Prepend(const T& item) const {
        LazySequence<T> single(&item, 1);
        return single.Concat(this); // [item] ++ this, у бесконечной длина остаётся ω
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
        // начало ++ [item] ++ хвост. первые index эл посчитаются дважды: для начала и для пропуска в хвосте
        IGenerator<T>* first = new TakeGenerator<T>(generator->CloneFromStart(), index);
        IGenerator<T>* single = new ArrayGenerator<T>(DynamicArray<T>(&item, 1));
        IGenerator<T>* second = new SkipGenerator<T>(generator->CloneFromStart(), index);
        IGenerator<T>* tempGen = new ConcatGenerator<T>(new ConcatGenerator<T>(first, single), second);
        // индекс конечный -> вставка всегда до ω, поэтому длина как у Prepend: 1 + length
        return new LazySequence<T>(tempGen, Ordinal(1) + length);
    }

    template<typename T2>
    LazySequence<T2>* Map(T2 (*function)(const T&)) const {
        if (function == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
        IGenerator<T2>* tempGen = new MapGenerator<T, T2>(generator->CloneFromStart(), function);
        return new LazySequence<T2>(tempGen, length);
    }

    LazySequence<T>* Where(bool (*predicate)(const T&)) const {
        if (predicate == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
        // у бесконечной сколько подходящих заранее не узнать, считаем ω.
        // у конечной считаем точно по копии генератора, иначе GetLength и GetLast бы врали
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