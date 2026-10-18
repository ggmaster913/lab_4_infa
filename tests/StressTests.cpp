#include <gtest/gtest.h>
#include <stdexcept>

#include "Stream.h"
#include "lab2/MutableArraySequence.h"
#include "lab2/MutableListSequence.h"

// функции-помощники
static int ArithmeticRule(const Sequence<int>* last) { return 2 * last->Get(1) - last->Get(0); }
static long long SquareLong(const int& x) { return static_cast<long long>(x) * x; }
static long long SumLong(const long long& acc, const long long& x) { return acc + x; }

// --- stress tests (millions of elements) ---

// получение миллионного элемента напрямую и проверка размера кэша
TEST(StressTest, MillionGet) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    EXPECT_EQ(naturals.Get(999999), 999999);
    EXPECT_EQ(naturals.GetMaterializedCount(), 1000000);
}

// цепочка map, подпоследовательности и reduce на миллионе элементов
TEST(StressTest, MillionMapReduce) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazySequence<long long>* squares = naturals.Map(SquareLong);
    LazySequence<long long>* firstMillion = squares->GetSubSequence(0, 999999);

    long long total = firstMillion->Reduce(SumLong, 0LL);
    EXPECT_EQ(total, 333332833333500000LL);

    delete firstMillion;
    delete squares;
}

// чтение миллиона элементов подряд через поток чтения
TEST(StressTest, MillionStreamRead) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);

    LazyReadStream<int> stream(&naturals);
    stream.Open();

    long long sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += stream.Read();
    }

    EXPECT_EQ(sum, 499999500000LL);
    EXPECT_EQ(stream.GetPosition(), 1000000);
}

// запись миллиона элементов подряд через поток записи в список
TEST(StressTest, MillionStreamWrite) {
    MutableListSequence<int> target;
    SequenceWriteStream<int> stream(&target);
    stream.Open();

    for (int i = 0; i < 1000000; ++i) {
        stream.Write(i);
    }

    EXPECT_EQ(stream.GetPosition(), 1000000);
    EXPECT_EQ(target.GetLength(), 1000000);
    EXPECT_EQ(target.GetFirst(), 0);
    EXPECT_EQ(target.GetLast(), 999999);
}
