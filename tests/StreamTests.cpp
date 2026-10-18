#include <gtest/gtest.h>
#include <stdexcept>

#include "Stream.h"
#include "lab2/MutableArraySequence.h"
#include "lab2/ImmutableArraySequence.h"
#include "lab2/MutableListSequence.h"
#include "lab2/ImmutableListSequence.h"

// функции-помощники
static int ArithmeticRule(const Sequence<int>* last) { return 2 * last->Get(1) - last->Get(0); }
static bool IsEven(const int& x) { return x % 2 == 0; }

// --- streams ---

// базовое чтение из обычной последовательности и проверка конца потока
TEST(StreamTest, SequenceReadBasics) {
    int raw[] = {5, 6, 7};
    MutableArraySequence<int> seq(raw, 3);
    SequenceReadStream<int> stream(&seq);

    stream.Open();
    EXPECT_FALSE(stream.IsEndOfStream());
    EXPECT_EQ(stream.GetPosition(), 0);

    EXPECT_EQ(stream.Read(), 5);
    EXPECT_EQ(stream.Read(), 6);
    EXPECT_EQ(stream.Read(), 7);

    EXPECT_EQ(stream.GetPosition(), 3);
    EXPECT_TRUE(stream.IsEndOfStream());
}

// ошибки чтения и позиционирования в закрытом потоке и за границами
TEST(StreamTest, ReadErrors) {
    int raw[] = {5, 6, 7};
    MutableArraySequence<int> seq(raw, 3);
    SequenceReadStream<int> stream(&seq);

    // до открытия
    EXPECT_THROW(stream.Read(), std::out_of_range);
    EXPECT_THROW(stream.Seek(0), std::out_of_range);

    stream.Open();
    EXPECT_THROW(stream.Seek(-1), std::out_of_range);
    EXPECT_THROW(stream.Seek(3), std::out_of_range);

    stream.Read();
    stream.Read();
    stream.Read();
    // после достижения конца
    EXPECT_THROW(stream.Read(), std::out_of_range);

    stream.Close();
    // после закрытия
    EXPECT_THROW(stream.Read(), std::out_of_range);
    EXPECT_THROW(stream.Seek(0), std::out_of_range);
}

// перемещение закладки через seek и повторное открытие потока
TEST(StreamTest, SeekAndReopen) {
    int raw[] = {5, 6, 7};
    MutableArraySequence<int> seq(raw, 3);
    SequenceReadStream<int> stream(&seq);

    EXPECT_TRUE(stream.IsCanSeek());
    EXPECT_TRUE(stream.IsCanGoBack());

    stream.Open();
    EXPECT_EQ(stream.Seek(1), 1);
    EXPECT_EQ(stream.Read(), 6);
    EXPECT_EQ(stream.GetPosition(), 2);

    // повторное открытие сбрасывает закладку в начало
    stream.Open();
    EXPECT_EQ(stream.GetPosition(), 0);
    EXPECT_EQ(stream.Read(), 5);
}

// чтение и прыжки по бесконечной ленивой последовательности
TEST(StreamTest, LazyReadInfinite) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);
    LazyReadStream<int> stream(&naturals);

    stream.Open();
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(stream.Read(), i);
    }
    EXPECT_FALSE(stream.IsEndOfStream());

    EXPECT_EQ(stream.Seek(1000), 1000);
    EXPECT_EQ(stream.Read(), 1000);
    EXPECT_FALSE(stream.IsEndOfStream());
}

// чтение из конечной ленивой последовательности после append
TEST(StreamTest, LazyReadFinite) {
    int raw[] = {7, 8, 9};
    LazySequence<int> baseSeq(raw, 3);
    LazySequence<int>* seq = baseSeq.Append(10);

    LazyReadStream<int> stream(seq);
    stream.Open();

    EXPECT_EQ(stream.Read(), 7);
    EXPECT_EQ(stream.Read(), 8);
    EXPECT_EQ(stream.Read(), 9);
    EXPECT_EQ(stream.Read(), 10);
    EXPECT_TRUE(stream.IsEndOfStream());

    delete seq;
}

// проверка на nullptr во всех конструкторах потоков
TEST(StreamTest, NullptrSources) {
    EXPECT_THROW(SequenceReadStream<int>(nullptr), std::invalid_argument);
    EXPECT_THROW(LazyReadStream<int>(nullptr), std::invalid_argument);
    EXPECT_THROW(SequenceWriteStream<int>(nullptr), std::invalid_argument);
}

// запись элементов в изменяемую последовательность
TEST(StreamTest, WriteToMutable) {
    MutableArraySequence<int> target;
    SequenceWriteStream<int> stream(&target);

    EXPECT_THROW(stream.Write(10), std::out_of_range);

    stream.Open();
    EXPECT_EQ(stream.Write(10), 1);
    EXPECT_EQ(stream.Write(20), 2);
    EXPECT_EQ(stream.Write(30), 3);

    EXPECT_EQ(target.GetLength(), 3);
    EXPECT_EQ(target.Get(0), 10);
    EXPECT_EQ(target.Get(1), 20);
    EXPECT_EQ(target.Get(2), 30);

    stream.Close();
    EXPECT_THROW(stream.Write(40), std::out_of_range);

    stream.Open();
    EXPECT_EQ(stream.Write(40), 4);
    EXPECT_EQ(target.GetLength(), 4);
    EXPECT_EQ(target.Get(3), 40);
}

// отклонение попытки записи в неизменяемые последовательности
TEST(StreamTest, WriteToImmutableRejected) {
    ImmutableArraySequence<int> immArr;
    SequenceWriteStream<int> streamArr(&immArr);
    streamArr.Open();

    EXPECT_THROW(streamArr.Write(100), std::invalid_argument);
    EXPECT_EQ(immArr.GetLength(), 0);
    EXPECT_EQ(streamArr.GetPosition(), 0);

    ImmutableListSequence<int> immList;
    SequenceWriteStream<int> streamList(&immList);
    streamList.Open();

    EXPECT_THROW(streamList.Write(100), std::invalid_argument);
    EXPECT_EQ(immList.GetLength(), 0);
    EXPECT_EQ(streamList.GetPosition(), 0);
}

// перекачка четных чисел из потока чтения в поток записи
TEST(StreamTest, ReadThenWrite) {
    int start[] = {0, 1};
    MutableArraySequence<int> initial(start, 2);
    LazySequence<int> naturals(ArithmeticRule, &initial);
    LazySequence<int>* evens = naturals.Where(IsEven);

    MutableListSequence<int> target;
    LazyReadStream<int> readStream(evens);
    SequenceWriteStream<int> writeStream(&target);

    readStream.Open();
    writeStream.Open();

    for (int i = 0; i < 10; ++i) {
        writeStream.Write(readStream.Read());
    }

    EXPECT_EQ(target.GetLength(), 10);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(target.Get(i), i * 2);
    }

    delete evens;
}
