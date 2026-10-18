#include <gtest/gtest.h>
#include <stdexcept>

#include "Ordinal.h"
#include "LazySequence.h"
#include "LazyAlgebra.h"
#include "lab2/MutableArraySequence.h"

// функции-помощники
static int ArithmeticRule(const Sequence<int>* last) { return 2 * last->Get(1) - last->Get(0); }
static int FibonacciRule(const Sequence<int>* last) { return last->Get(0) + last->Get(1); }
static int Square(const int& x) { return x * x; }
static bool IsEven(const int& x) { return x % 2 == 0; }
static int Sum(const int& acc, const int& x) { return acc + x; }
static double Half(const int& x) { return x / 2.0; }
static int TribonacciRule(const Sequence<int>* last) { return last->Get(0) + last->Get(1) + last->Get(2); }

// --- ordinal ---

// создание ординала по умолчанию и проверка на ноль
TEST(OrdinalTest, DefaultIsZero) {
    Ordinal ord;
    EXPECT_EQ(ord.GetOmegaCount(), 0);
    EXPECT_EQ(ord.GetFinitePart(), 0);
    EXPECT_TRUE(ord.IsFinite());
}

// создание через конструкторы с параметрами и получение омеги
TEST(OrdinalTest, ConstructorsAndOmega) {
    Ordinal finiteOrd(5);
    EXPECT_EQ(finiteOrd.GetOmegaCount(), 0);
    EXPECT_EQ(finiteOrd.GetFinitePart(), 5);
    EXPECT_TRUE(finiteOrd.IsFinite());

    Ordinal transfiniteOrd(2, 3);
    EXPECT_EQ(transfiniteOrd.GetOmegaCount(), 2);
    EXPECT_EQ(transfiniteOrd.GetFinitePart(), 3);
    EXPECT_FALSE(transfiniteOrd.IsFinite());

    Ordinal omega = Ordinal::Omega();
    EXPECT_EQ(omega.GetOmegaCount(), 1);
    EXPECT_EQ(omega.GetFinitePart(), 0);
    EXPECT_FALSE(omega.IsFinite());
}

// передача отрицательных чисел в конструкторы
TEST(OrdinalTest, NegativeThrows) {
    EXPECT_THROW(Ordinal(-1), std::invalid_argument);
    EXPECT_THROW(Ordinal(-1, 0), std::invalid_argument);
    EXPECT_THROW(Ordinal(0, -1), std::invalid_argument);
    EXPECT_THROW(Ordinal(-2, -5), std::invalid_argument);
}

// сложение конечных ординалов и добавление конечного хвоста к омеге
TEST(OrdinalTest, AdditionFinite) {
    Ordinal a(2);
    Ordinal b(3);
    Ordinal sumFinite = a + b;
    EXPECT_TRUE(sumFinite == Ordinal(5));

    Ordinal omega = Ordinal::Omega();
    Ordinal tail(5);
    Ordinal omegaPlusFive = omega + tail;
    EXPECT_EQ(omegaPlusFive.GetOmegaCount(), 1);
    EXPECT_EQ(omegaPlusFive.GetFinitePart(), 5);
    EXPECT_TRUE(omegaPlusFive == Ordinal(1, 5));
}

// поглощение левого хвоста при сложении с омегой справа
TEST(OrdinalTest, AdditionAbsorption) {
    Ordinal finitePart(5);
    Ordinal omega = Ordinal::Omega();

    // 5 + omega == omega
    Ordinal absorbed = finitePart + omega;
    EXPECT_TRUE(absorbed == Ordinal::Omega());

    // 1 + omega == omega
    EXPECT_TRUE(Ordinal(1) + Ordinal::Omega() == Ordinal::Omega());

    // omega + 1 != omega
    EXPECT_TRUE(Ordinal::Omega() + Ordinal(1) != Ordinal::Omega());

    // (omega + 5) + (omega + 3) == 2*omega + 3
    Ordinal first(1, 5);
    Ordinal second(1, 3);
    Ordinal sum = first + second;
    EXPECT_EQ(sum.GetOmegaCount(), 2);
    EXPECT_EQ(sum.GetFinitePart(), 3);
    EXPECT_TRUE(sum == Ordinal(2, 3));
}

// проверка всех шести операторов сравнения ординалов
TEST(OrdinalTest, Comparison) {
    Ordinal bigFinite(1000000);
    Ordinal omega = Ordinal::Omega();
    Ordinal omegaPlusOne(1, 1);
    Ordinal twoOmega(2, 0);

    EXPECT_TRUE(bigFinite < omega);
    EXPECT_TRUE(omega < omegaPlusOne);
    EXPECT_TRUE(omegaPlusOne < twoOmega);

    EXPECT_TRUE(twoOmega > omegaPlusOne);
    EXPECT_TRUE(omegaPlusOne > omega);
    EXPECT_TRUE(omega > bigFinite);

    EXPECT_TRUE(omega <= omega);
    EXPECT_TRUE(omega <= omegaPlusOne);

    EXPECT_TRUE(omega >= omega);
    EXPECT_TRUE(twoOmega >= omegaPlusOne);

    EXPECT_TRUE(omega == Ordinal(1, 0));
    EXPECT_TRUE(omega != omegaPlusOne);
}

// ассоциативность сложения ординалов на переборе значений
TEST(OrdinalTest, AdditionIsAssociative) {
    for (int pOmega = 0; pOmega <= 2; pOmega++) {
        for (int pFin = 0; pFin <= 2; pFin++) {
            Ordinal p(pOmega, pFin);
            for (int qOmega = 0; qOmega <= 2; qOmega++) {
                for (int qFin = 0; qFin <= 2; qFin++) {
                    Ordinal q(qOmega, qFin);
                    for (int rOmega = 0; rOmega <= 2; rOmega++) {
                        for (int rFin = 0; rFin <= 2; rFin++) {
                            Ordinal r(rOmega, rFin);
                            EXPECT_TRUE((p + q) + r == p + (q + r));
                        }
                    }
                }
            }
        }
    }
}

// --- lazy sequence creation and element access ---

// создание пустой последовательности и попытка взять крайние элементы
TEST(LazySequenceTest, EmptySequence) {
    LazySequence<int> seq;
    EXPECT_TRUE(seq.GetLength() == Ordinal());
    EXPECT_EQ(seq.GetMaterializedCount(), 0);
    EXPECT_THROW(seq.GetFirst(), std::out_of_range);
    EXPECT_THROW(seq.GetLast(), std::out_of_range);
    EXPECT_THROW(seq.Get(0), std::out_of_range);
}

// создание из сырого массива и проверка чтения по индексам
TEST(LazySequenceTest, FromArray) {
    int raw[] = {7, 8, 9};
    LazySequence<int> seq(raw, 3);

    EXPECT_TRUE(seq.GetLength() == Ordinal(3));
    EXPECT_EQ(seq.Get(0), 7);
    EXPECT_EQ(seq.Get(1), 8);
    EXPECT_EQ(seq.Get(2), 9);

    EXPECT_THROW(seq.Get(3), std::out_of_range);
    EXPECT_THROW(seq.Get(-1), std::out_of_range);
}

// создание из mutable array sequence и проверка на nullptr
TEST(LazySequenceTest, FromSequence) {
    int raw[] = {7, 8, 9};
    MutableArraySequence<int> mutSeq(raw, 3);
    LazySequence<int> seq(&mutSeq);

    EXPECT_TRUE(seq.GetLength() == Ordinal(3));
    EXPECT_EQ(seq.Get(0), 7);
    EXPECT_EQ(seq.Get(2), 9);

    const Sequence<int>* nullSeq = nullptr;
    EXPECT_THROW(LazySequence<int> badSeq(nullSeq), std::invalid_argument);
}

// генерация ряда фибоначчи по рекуррентному правилу
TEST(LazySequenceTest, RuleFibonacci) {
    int start[] = {1, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> fib(FibonacciRule, &initial);

    EXPECT_TRUE(fib.GetLength() == Ordinal::Omega());
    EXPECT_FALSE(fib.GetLength().IsFinite());

    int expected[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(fib.Get(i), expected[i]);
    }
}

// ленивое вычисление и работа кэша мемоизации
TEST(LazySequenceTest, Memoization) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    EXPECT_EQ(naturals.GetMaterializedCount(), 0);

    EXPECT_EQ(naturals.Get(5), 5);
    EXPECT_EQ(naturals.GetMaterializedCount(), 6);

    EXPECT_EQ(naturals.Get(2), 2);
    EXPECT_EQ(naturals.GetMaterializedCount(), 6);
}

// проверка всех ошибок при создании последовательности
TEST(LazySequenceTest, ConstructorErrors) {
    int start[] = {1, 1};
    MutableArraySequence<int> initial(start, 2);
    MutableArraySequence<int> emptyInitial;

    EXPECT_THROW(LazySequence<int>(nullptr, &initial), std::invalid_argument);
    EXPECT_THROW(LazySequence<int>(FibonacciRule, nullptr), std::invalid_argument);
    EXPECT_THROW(LazySequence<int>(FibonacciRule, &emptyInitial), std::invalid_argument);

    IGenerator<int>* nullGen = nullptr;
    EXPECT_THROW(LazySequence<int>(nullGen, Ordinal(5)), std::invalid_argument);

    int raw[] = {1, 2};
    EXPECT_THROW(LazySequence<int>(raw, -1), std::invalid_argument);
}

// обращение к элементам через ординал и виртуальная граница бесконечности
TEST(LazySequenceTest, GetByOrdinal) {
    int raw[] = {7, 8, 9};
    LazySequence<int> seq(raw, 3);
    EXPECT_EQ(seq.Get(Ordinal(2)), 9);
    EXPECT_THROW(seq.Get(Ordinal(3)), std::out_of_range);

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    EXPECT_EQ(naturals.Get(Ordinal(4)), 4);
    EXPECT_THROW(naturals.Get(Ordinal::Omega()), std::out_of_range);
}

// независимость копии и корректность операторов присваивания
TEST(LazySequenceTest, CopyIsIndependent) {
    int raw[] = {7, 8, 9};
    LazySequence<int>* orig = new LazySequence<int>(raw, 3);
    EXPECT_EQ(orig->Get(1), 8);

    LazySequence<int> copy(*orig);
    delete orig;

    EXPECT_EQ(copy.Get(0), 7);
    EXPECT_EQ(copy.Get(2), 9);

    LazySequence<int> a(raw, 3);
    LazySequence<int> b;
    b = a;
    EXPECT_EQ(b.Get(1), 8);

    // самоприсваивание
    a = a;
    EXPECT_EQ(a.Get(2), 9);
    EXPECT_EQ(a.GetMaterializedCount(), 3);
}

// получение первого и последнего элементов в конечной и бесконечной последовательностях
TEST(LazySequenceTest, GetFirstGetLast) {
    int raw[] = {7, 8, 9};
    LazySequence<int> finiteSeq(raw, 3);
    EXPECT_EQ(finiteSeq.GetFirst(), 7);
    EXPECT_EQ(finiteSeq.GetLast(), 9);

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> infiniteSeq(ArithmeticRule, &initial);
    EXPECT_EQ(infiniteSeq.GetFirst(), 0);
    EXPECT_THROW(infiniteSeq.GetLast(), std::out_of_range);
}

// --- lazy sequence operations ---

// добавление элемента в конец бесконечной последовательности дает омега плюс один
TEST(LazySequenceTest, AppendToInfiniteGivesOmegaPlusOne) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int>* extended = naturals.Append(42);

    EXPECT_TRUE(extended->GetLength() == Ordinal::Omega() + Ordinal(1));
    EXPECT_FALSE(extended->GetLength().IsFinite());
    EXPECT_EQ(extended->Get(5), 5);
    EXPECT_THROW(extended->Get(Ordinal::Omega() + Ordinal(1)), std::out_of_range);
    EXPECT_THROW(extended->GetLast(), std::out_of_range);

    delete extended;
}

// добавление элемента в начало бесконечной последовательности оставляет длину омега
TEST(LazySequenceTest, PrependToInfiniteKeepsOmega) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int>* prepended = naturals.Prepend(42);

    EXPECT_TRUE(prepended->GetLength() == Ordinal::Omega());
    EXPECT_EQ(prepended->Get(0), 42);
    EXPECT_EQ(prepended->Get(1), 0);
    EXPECT_EQ(prepended->Get(2), 1);

    delete prepended;
}

// конкатенация конечных и бесконечных последовательностей
TEST(LazySequenceTest, Concat) {
    int raw1[] = {7, 8, 9};
    int raw2[] = {1, 2};
    LazySequence<int> seq1(raw1, 3);
    LazySequence<int> seq2(raw2, 2);

    LazySequence<int>* combined = seq1.Concat(&seq2);
    EXPECT_TRUE(combined->GetLength() == Ordinal(5));
    EXPECT_EQ(combined->Get(0), 7);
    EXPECT_EQ(combined->Get(2), 9);
    EXPECT_EQ(combined->Get(3), 1);
    EXPECT_EQ(combined->Get(4), 2);
    delete combined;

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int>* twoOmegas = naturals.Concat(&naturals);
    EXPECT_TRUE(twoOmegas->GetLength() == Ordinal(2, 0));
    EXPECT_EQ(twoOmegas->Get(3), 3);
    delete twoOmegas;
}

// получение подпоследовательности и проверка выхода за границы
TEST(LazySequenceTest, GetSubSequence) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int>* sub = naturals.GetSubSequence(10, 14);
    EXPECT_TRUE(sub->GetLength() == Ordinal(5));
    EXPECT_EQ(sub->Get(0), 10);
    EXPECT_EQ(sub->Get(4), 14);
    EXPECT_EQ(sub->GetLast(), 14);
    delete sub;

    int raw[] = {7, 8, 9};
    LazySequence<int> seq(raw, 3);
    EXPECT_THROW(seq.GetSubSequence(1, 3), std::out_of_range);
    EXPECT_THROW(seq.GetSubSequence(-1, 2), std::out_of_range);
    EXPECT_THROW(seq.GetSubSequence(2, 1), std::out_of_range);
}

// вставка элемента по индексу для конечной и бесконечной последовательностей
TEST(LazySequenceTest, InsertAt) {
    int raw[] = {7, 8, 9};
    LazySequence<int> seq(raw, 3);

    LazySequence<int>* inserted = seq.InsertAt(5, 1);
    EXPECT_TRUE(inserted->GetLength() == Ordinal(4));
    EXPECT_EQ(inserted->Get(0), 7);
    EXPECT_EQ(inserted->Get(1), 5);
    EXPECT_EQ(inserted->Get(2), 8);
    EXPECT_EQ(inserted->Get(3), 9);
    delete inserted;

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int>* infInserted = naturals.InsertAt(100, 2);
    EXPECT_TRUE(infInserted->GetLength() == Ordinal::Omega());
    EXPECT_EQ(infInserted->Get(0), 0);
    EXPECT_EQ(infInserted->Get(1), 1);
    EXPECT_EQ(infInserted->Get(2), 100);
    EXPECT_EQ(infInserted->Get(3), 2);
    delete infInserted;

    EXPECT_THROW(seq.InsertAt(99, 3), std::out_of_range);
    EXPECT_THROW(seq.InsertAt(99, -1), std::out_of_range);
}

// вставка целой последовательности: конечной в конечную, в бесконечную и бесконечной в конечную
TEST(LazySequenceTest, InsertSequence) {
    int raw[] = {7, 8, 9};
    int rawInserted[] = {1, 2};
    LazySequence<int> seq(raw, 3);
    LazySequence<int> inserted(rawInserted, 2);

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int>* finite = seq.InsertAt(&inserted, 1);
    EXPECT_TRUE(finite->GetLength() == Ordinal(5));
    int expected[] = {7, 1, 2, 8, 9};
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(finite->Get(i), expected[i]);
    }
    delete finite;

    // 2 + 2 + ω = ω
    LazySequence<int>* intoInfinite = naturals.InsertAt(&inserted, 2);
    EXPECT_TRUE(intoInfinite->GetLength() == Ordinal::Omega());
    EXPECT_EQ(intoInfinite->Get(1), 1);
    EXPECT_EQ(intoInfinite->Get(2), 1);
    EXPECT_EQ(intoInfinite->Get(3), 2);
    EXPECT_EQ(intoInfinite->Get(4), 2);
    delete intoInfinite;

    // 1 + ω + 2 = ω + 2, хвост {8, 9} перечислением не достать
    LazySequence<int>* infiniteInside = seq.InsertAt(&naturals, 1);
    EXPECT_TRUE(infiniteInside->GetLength() == Ordinal(1, 2));
    EXPECT_EQ(infiniteInside->Get(0), 7);
    EXPECT_EQ(infiniteInside->Get(1), 0);
    EXPECT_EQ(infiniteInside->Get(5), 4);
    delete infiniteInside;

    EXPECT_THROW(seq.InsertAt(&inserted, 3), std::out_of_range);
    const LazySequence<int>* nullSeq = nullptr;
    EXPECT_THROW(seq.InsertAt(nullSeq, 1), std::invalid_argument);
}

// поэлементное преобразование через map с изменением типа данных
TEST(LazySequenceTest, Map) {
    int raw[] = {3, 4, 5};
    LazySequence<int> seq(raw, 3);

    LazySequence<int>* squared = seq.Map(Square);
    EXPECT_TRUE(squared->GetLength() == Ordinal(3));
    EXPECT_EQ(squared->Get(0), 9);
    EXPECT_EQ(squared->Get(1), 16);
    EXPECT_EQ(squared->Get(2), 25);
    delete squared;

    LazySequence<double>* halves = seq.Map(Half);
    EXPECT_TRUE(halves->GetLength() == Ordinal(3));
    EXPECT_DOUBLE_EQ(halves->Get(0), 1.5);
    EXPECT_DOUBLE_EQ(halves->Get(1), 2.0);
    EXPECT_DOUBLE_EQ(halves->Get(2), 2.5);
    delete halves;
}

// фильтрация элементов через where
TEST(LazySequenceTest, Where) {
    int raw[] = {1, 2, 3, 4, 5, 6};
    LazySequence<int> seq(raw, 6);

    LazySequence<int>* evens = seq.Where(IsEven);
    EXPECT_TRUE(evens->GetLength() == Ordinal(3));
    EXPECT_EQ(evens->Get(0), 2);
    EXPECT_EQ(evens->Get(1), 4);
    EXPECT_EQ(evens->Get(2), 6);
    delete evens;

    int oddsRaw[] = {1, 3, 5};
    LazySequence<int> oddsSeq(oddsRaw, 3);
    LazySequence<int>* noEvens = oddsSeq.Where(IsEven);
    EXPECT_TRUE(noEvens->GetLength() == Ordinal(0));
    delete noEvens;

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int>* infEvens = naturals.Where(IsEven);
    EXPECT_TRUE(infEvens->GetLength() == Ordinal::Omega());
    EXPECT_EQ(infEvens->Get(0), 0);
    EXPECT_EQ(infEvens->Get(1), 2);
    delete infEvens;
}

// свертка конечной последовательности через reduce
TEST(LazySequenceTest, Reduce) {
    int raw[] = {1, 2, 3, 4, 5};
    LazySequence<int> seq(raw, 5);

    int sum = seq.Reduce(Sum, 0);
    EXPECT_EQ(sum, 15);

    LazySequence<int> emptySeq;
    int emptySum = emptySeq.Reduce(Sum, 42);
    EXPECT_EQ(emptySum, 42);

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);
    EXPECT_THROW(naturals.Reduce(Sum, 0), std::out_of_range);
}

// удаление исходной последовательности после операции не ломает результат
TEST(LazySequenceTest, SourceDeletedAfterOperation) {
    int raw[] = {7, 8, 9};
    LazySequence<int>* orig = new LazySequence<int>(raw, 3);

    LazySequence<int>* appended = orig->Append(10);
    delete orig;

    EXPECT_TRUE(appended->GetLength() == Ordinal(4));
    EXPECT_EQ(appended->Get(0), 7);
    EXPECT_EQ(appended->Get(3), 10);

    delete appended;
}

// --- lazy algebra ---

// базовое сравнение последовательностей и проверка неверных аргументов
TEST(AlgebraTest, AreEqualBasics) {
    int raw1[] = {1, 2, 3};
    int raw2[] = {1, 2, 3};
    int raw3[] = {1, 2, 4};
    int raw4[] = {1, 2};

    LazySequence<int> seq1(raw1, 3);
    LazySequence<int> seq2(raw2, 3);
    LazySequence<int> seq3(raw3, 3);
    LazySequence<int> seq4(raw4, 2);

    EXPECT_TRUE(AreEqual(&seq1, &seq2, 10));
    EXPECT_FALSE(AreEqual(&seq1, &seq3, 10));
    EXPECT_FALSE(AreEqual(&seq4, &seq1, 10));
    EXPECT_TRUE(AreEqual(&seq1, &seq2, 0));

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);
    LazySequence<int>* extended = naturals.Append(42);

    EXPECT_FALSE(AreEqual(&naturals, extended, 10));
    delete extended;

    EXPECT_THROW(AreEqual<int>(nullptr, &seq1, 5), std::invalid_argument);
    EXPECT_THROW(AreEqual<int>(&seq1, nullptr, 5), std::invalid_argument);
    EXPECT_THROW(AreEqual<int>(&seq1, &seq2, -1), std::invalid_argument);
}

// ассоциативность конкатенации на конечных последовательностях
TEST(AlgebraTest, AssociativityFinite) {
    int rawA[] = {1, 2};
    int rawB[] = {3};
    int rawC[] = {4, 5};

    LazySequence<int> a(rawA, 2);
    LazySequence<int> b(rawB, 1);
    LazySequence<int> c(rawC, 2);

    EXPECT_TRUE(CheckAssociativity(&a, &b, &c, 10));
}

// ассоциативность конкатенации с участием бесконечных последовательностей
TEST(AlgebraTest, AssociativityInfinite) {
    int startNat[] = {0, 1};
    MutableArraySequence<int> initNat(startNat, 2);
    LazySequence<int> naturals(ArithmeticRule, &initNat);

    int startFib[] = {1, 1};
    MutableArraySequence<int> initFib(startFib, 2);
    LazySequence<int> fib(FibonacciRule, &initFib);

    int raw7[] = {7};
    int raw8[] = {8};
    LazySequence<int> s7(raw7, 1);
    LazySequence<int> s8(raw8, 1);

    EXPECT_TRUE(CheckAssociativity(&naturals, &s7, &s8, 15));
    EXPECT_TRUE(CheckAssociativity(&naturals, &fib, &s7, 15));
}

// проверка нейтрального элемента для конечной, бесконечной и пустой последовательности
TEST(AlgebraTest, IdentityElement) {
    int raw[] = {1, 2, 3};
    LazySequence<int> finiteSeq(raw, 3);

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<int> emptySeq;

    EXPECT_TRUE(CheckIdentity(&finiteSeq, 10));
    EXPECT_TRUE(CheckIdentity(&naturals, 10));
    EXPECT_TRUE(CheckIdentity(&emptySeq, 10));
}

// некоммутативность конкатенации на нетривиальных последовательностях
TEST(AlgebraTest, NotCommutative) {
    int raw12[] = {1, 2};
    int raw3[] = {3};
    int raw7[] = {7};

    LazySequence<int> s12(raw12, 2);
    LazySequence<int> s3(raw3, 1);
    LazySequence<int> s7(raw7, 1);
    LazySequence<int> emptySeq;

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    EXPECT_FALSE(CheckCommutativity(&s12, &s3, 10));
    EXPECT_FALSE(CheckCommutativity(&naturals, &s7, 10));
    EXPECT_TRUE(CheckCommutativity(&emptySeq, &s12, 10));
}

// согласованность длины конкатенации со сложением ординалов
TEST(AlgebraTest, LengthAddition) {
    int raw12[] = {1, 2};
    int raw3[] = {3};
    int raw7[] = {7};

    LazySequence<int> s12(raw12, 2);
    LazySequence<int> s3(raw3, 1);
    LazySequence<int> s7(raw7, 1);

    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    EXPECT_TRUE(CheckLengthAddition(&s12, &s3));
    EXPECT_TRUE(CheckLengthAddition(&naturals, &s7));
    EXPECT_TRUE(CheckLengthAddition(&s7, &naturals));
    EXPECT_TRUE(CheckLengthAddition(&naturals, &naturals));

    LazySequence<int>* natPlus7 = naturals.Concat(&s7);
    LazySequence<int>* sevenPlusNat = s7.Concat(&naturals);
    LazySequence<int>* natPlusNat = naturals.Concat(&naturals);

    EXPECT_TRUE(natPlus7->GetLength() == Ordinal::Omega() + Ordinal(1));
    EXPECT_TRUE(sevenPlusNat->GetLength() == Ordinal::Omega());
    EXPECT_TRUE(natPlusNat->GetLength() == Ordinal(2, 0));

    delete natPlus7;
    delete sevenPlusNat;
    delete natPlusNat;
}

// --- option and generators ---

// пустая и заполненная коробка, ноль внутри не путается с пустотой
TEST(OptionTest, EmptyAndFull) {
    Option<int> empty;
    Option<int> full(7);
    Option<int> zero(0);

    EXPECT_FALSE(empty.HasValue());
    EXPECT_THROW(empty.GetValue(), std::out_of_range);
    EXPECT_TRUE(full.HasValue());
    EXPECT_EQ(full.GetValue(), 7);
    EXPECT_TRUE(zero.HasValue());
    EXPECT_EQ(zero.GetValue(), 0);
}

// TryGetNext в конце генератора и копия генератора с начала
TEST(GeneratorTest, TryGetNextAndCloneFromStart) {
    int raw[] = {7, 8, 9};
    IGenerator<int>* gen = new ArrayGenerator<int>(DynamicArray<int>(raw, 3));
    EXPECT_EQ(gen->GetNext(), 7);
    EXPECT_EQ(gen->GetNext(), 8);

    IGenerator<int>* copy = gen->CloneFromStart();
    EXPECT_EQ(copy->GetNext(), 7);

    EXPECT_EQ(gen->TryGetNext().GetValue(), 9);
    EXPECT_FALSE(gen->TryGetNext().HasValue());
    EXPECT_FALSE(gen->HasNext());
    EXPECT_THROW(gen->GetNext(), std::out_of_range);

    delete copy;
    delete gen;
}

// сдвиг окна у правила с тремя начальными членами
TEST(GeneratorTest, RuleGeneratorWindow) {
    int start[] = {0, 0, 1};
    MutableArraySequence<int> initial(start, 3);
    RuleGenerator<int> gen(TribonacciRule, &initial);

    int expected[] = {0, 0, 1, 1, 2, 4, 7, 13, 24, 44};
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(gen.GetNext(), expected[i]);
    }

    IGenerator<int>* copy = gen.CloneFromStart();
    EXPECT_EQ(copy->GetNext(), 0);
    EXPECT_TRUE(copy->HasNext());
    delete copy;
}
