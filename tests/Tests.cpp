#include <gtest/gtest.h>
#include <stdexcept>

#include "lab2/DynamicArray.h"
#include "lab2/LinkedList.h"
#include "lab2/MutableArraySequence.h"
#include "lab2/ImmutableArraySequence.h"
#include "lab2/MutableListSequence.h"
#include "lab2/BitSequence.h"
#include "lab2/SequenceStats.h"
#include "lab2/ICollection.h"

// функции-помощники
bool isEven(const int& x) { return x % 2 == 0; }
int square(const int& x) { return x * x; }
int sumHelper(const int& acc, const int& x) { return acc + x; }

// --- dynamic array ---

// создание массива и проверка на кривой размер
TEST(DynamicArrayTest, ConstructorsAndNegativeSize) {
    DynamicArray<int> arr1(3);
    EXPECT_EQ(arr1.GetSize(), 3);

    int raw[] = {10, 20, 30};
    DynamicArray<int> arr2(raw, 3);
    EXPECT_EQ(arr2.GetSize(), 3);
    EXPECT_EQ(arr2.Get(0), 10);
    EXPECT_EQ(arr2.Get(2), 30);

    EXPECT_THROW(DynamicArray<int>(-5), std::invalid_argument);
    EXPECT_THROW(DynamicArray<int>(nullptr, 3), std::invalid_argument);
}

// чтение, запись и вылет за пределы массива
TEST(DynamicArrayTest, GetSetAndOutOfRange) {
    DynamicArray<int> arr(2);
    arr.Set(0, 100);
    arr.Set(1, 200);

    EXPECT_EQ(arr.Get(0), 100);
    EXPECT_EQ(arr.Get(1), 200);

    EXPECT_THROW(arr.Get(-1), std::out_of_range);
    EXPECT_THROW(arr.Get(2), std::out_of_range);
    EXPECT_THROW(arr.Set(2, 300), std::out_of_range);
}

// изменение размера в плюс и в минус
TEST(DynamicArrayTest, ResizeGrowthAndShrink) {
    int raw[] = {1, 2, 3};
    DynamicArray<int> arr(raw, 3);

    arr.Resize(5);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(2), 3);

    arr.Resize(2);
    EXPECT_EQ(arr.GetSize(), 2);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
    EXPECT_THROW(arr.Get(2), std::out_of_range);
    EXPECT_THROW(arr.Resize(-1), std::invalid_argument);
}

// доступ через квадратные скобки и неверные индексы
TEST(DynamicArrayTest, IndexOperatorReadWrite) {
    DynamicArray<int> arr(3);
    arr[0] = 5;
    arr[1] = 10;
    arr[2] = 15;

    EXPECT_EQ(arr[0], 5);
    EXPECT_EQ(arr[1], 10);
    EXPECT_EQ(arr[2], 15);

    EXPECT_THROW(arr[-1], std::out_of_range);
    EXPECT_THROW(arr[3], std::out_of_range);
}

// работа через интерфейс icollection
TEST(DynamicArrayTest, ICollectionPolymorphism) {
    int raw[] = {7, 8, 9};
    DynamicArray<int> arr(raw, 3);
    ICollection<int>* col = &arr;

    EXPECT_EQ(col->GetCount(), 3);
    EXPECT_EQ(col->Get(1), 8);
}

// --- linked list ---

// вставка в начало и в конец
TEST(LinkedListTest, AppendAndPrependOrder) {
    LinkedList<int> list;
    list.Append(20);
    list.Append(30);
    list.Prepend(10);

    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.Get(0), 10);
    EXPECT_EQ(list.Get(1), 20);
    EXPECT_EQ(list.Get(2), 30);
}

// вставка по индексу и проверка границ
TEST(LinkedListTest, InsertAtMiddleAndBounds) {
    int raw[] = {1, 3};
    LinkedList<int> list(raw, 2);

    list.InsertAt(2, 1);
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.Get(1), 2);

    EXPECT_THROW(list.InsertAt(99, 3), std::out_of_range);
    EXPECT_THROW(list.InsertAt(99, -1), std::out_of_range);
}

// попытка взять элементы из пустого списка
TEST(LinkedListTest, EmptyListThrows) {
    LinkedList<int> list;
    EXPECT_THROW(list.GetFirst(), std::out_of_range);
    EXPECT_THROW(list.GetLast(), std::out_of_range);
    EXPECT_THROW(list.Get(0), std::out_of_range);
}

// получение подсписка и невалидные диапазоны
TEST(LinkedListTest, GetSubListRangeAndErrors) {
    int raw[] = {10, 20, 30, 40};
    LinkedList<int> list(raw, 4);

    LinkedList<int>* sub = list.GetSubList(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    EXPECT_EQ(sub->Get(0), 20);
    EXPECT_EQ(sub->Get(1), 30);
    delete sub;

    EXPECT_THROW(list.GetSubList(-1, 2), std::out_of_range);
    EXPECT_THROW(list.GetSubList(2, 1), std::out_of_range);
    EXPECT_THROW(list.GetSubList(0, 5), std::out_of_range);
}

// склейка списков и проверка на nullptr
TEST(LinkedListTest, ConcatAndNullptr) {
    int raw1[] = {1, 2};
    int raw2[] = {3, 4};
    LinkedList<int> list1(raw1, 2);
    LinkedList<int> list2(raw2, 2);

    LinkedList<int>* res = list1.Concat(&list2);
    EXPECT_EQ(res->GetLength(), 4);
    EXPECT_EQ(res->Get(0), 1);
    EXPECT_EQ(res->Get(3), 4);
    delete res;

    EXPECT_THROW(list1.Concat(nullptr), std::invalid_argument);
}

// обращение по индексу через скобки в списке
TEST(LinkedListTest, IndexOperatorReadWrite) {
    int raw[] = {10, 20};
    LinkedList<int> list(raw, 2);

    list[0] = 99;
    EXPECT_EQ(list.Get(0), 99);
    EXPECT_EQ(list[1], 20);

    EXPECT_THROW(list[-1], std::out_of_range);
    EXPECT_THROW(list[2], std::out_of_range);
}

// копирование списка и независимость данных
TEST(LinkedListTest, CopyAndAssignmentIndependence) {
    int raw[] = {1, 2};
    LinkedList<int> original(raw, 2);

    LinkedList<int> copy1 = original;
    LinkedList<int> copy2;
    copy2 = original;

    original.Append(3);
    EXPECT_EQ(original.GetLength(), 3);
    EXPECT_EQ(copy1.GetLength(), 2);
    EXPECT_EQ(copy2.GetLength(), 2);
}

// --- sequence ---

// методы на мутабельной последовательности меняют сам объект
TEST(SequenceTest, MutableOperationsModifyInPlace) {
    int raw[] = {2};
    MutableArraySequence<int> seq(raw, 1);

    Sequence<int>* resAppend = seq.Append(4);
    EXPECT_EQ(resAppend, &seq);
    EXPECT_EQ(seq.GetLength(), 2);
    EXPECT_EQ(seq.Get(1), 4);

    Sequence<int>* resPrepend = seq.Prepend(1);
    EXPECT_EQ(resPrepend, &seq);
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.Get(0), 1);

    Sequence<int>* resInsert = seq.InsertAt(3, 2);
    EXPECT_EQ(resInsert, &seq);
    EXPECT_EQ(seq.GetLength(), 4);
    EXPECT_EQ(seq.Get(2), 3);
}

// методы на иммутабельной последовательности возвращают новый объект
TEST(SequenceTest, ImmutableOperationsDoNotModifyOriginal) {
    int raw[] = {2};
    ImmutableArraySequence<int> original(raw, 1);

    Sequence<int>* resAppend = original.Append(3);
    EXPECT_NE(resAppend, &original);
    EXPECT_EQ(original.GetLength(), 1);
    EXPECT_EQ(resAppend->GetLength(), 2);

    Sequence<int>* resPrepend = original.Prepend(1);
    EXPECT_NE(resPrepend, &original);
    EXPECT_EQ(original.GetLength(), 1);
    EXPECT_EQ(resPrepend->Get(0), 1);

    Sequence<int>* resInsert = original.InsertAt(99, 0);
    EXPECT_NE(resInsert, &original);
    EXPECT_EQ(original.GetLength(), 1);
    EXPECT_EQ(resInsert->Get(0), 99);

    delete resAppend;
    delete resPrepend;
    delete resInsert;
}

// объединение массива со списком
TEST(SequenceTest, ConcatArrayWithList) {
    int rawArr[] = {1, 2};
    int rawList[] = {3, 4};
    MutableArraySequence<int> seqArr(rawArr, 2);
    MutableListSequence<int> seqList(rawList, 2);

    Sequence<int>* res = seqArr.Concat(&seqList);
    EXPECT_EQ(res->GetLength(), 4);
    EXPECT_EQ(res->Get(0), 1);
    EXPECT_EQ(res->Get(3), 4);
}

// выделение подпоследовательности и неверные границы
TEST(SequenceTest, GetSubSequenceRangeAndErrors) {
    int raw[] = {10, 20, 30, 40};
    MutableListSequence<int> seq(raw, 4);

    Sequence<int>* sub = seq.GetSubSequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    EXPECT_EQ(sub->Get(0), 20);
    EXPECT_EQ(sub->Get(1), 30);
    delete sub;

    EXPECT_THROW(seq.GetSubSequence(-1, 2), std::out_of_range);
    EXPECT_THROW(seq.GetSubSequence(2, 1), std::out_of_range);
    EXPECT_THROW(seq.GetSubSequence(0, 4), std::out_of_range);
}

// фильтрация через where для заполненной и пустой последовательности
TEST(SequenceTest, WhereNonEmptyAndEmpty) {
    int raw[] = {1, 2, 3, 4, 5, 6};
    MutableArraySequence<int> seq(raw, 6);

    Sequence<int>* filtered = seq.Where(isEven);
    EXPECT_EQ(filtered->GetLength(), 3);
    EXPECT_EQ(filtered->Get(0), 2);
    EXPECT_EQ(filtered->Get(2), 6);
    delete filtered;

    MutableArraySequence<int> emptySeq;
    Sequence<int>* emptyFiltered = emptySeq.Where(isEven);
    EXPECT_EQ(emptyFiltered->GetLength(), 0);
    delete emptyFiltered;
}

// поэлементное преобразование через map
TEST(SequenceTest, MapValuesTransformation) {
    int raw[] = {1, 2, 3};
    MutableListSequence<int> seq(raw, 3);

    Sequence<int>* mapped = seq.Map(square);
    EXPECT_EQ(mapped->GetLength(), 3);
    EXPECT_EQ(mapped->Get(0), 1);
    EXPECT_EQ(mapped->Get(1), 4);
    EXPECT_EQ(mapped->Get(2), 9);
    delete mapped;
}

// сворачивание последовательности через reduce
TEST(SequenceTest, ReduceNormalAndEmpty) {
    int raw[] = {1, 2, 3, 4};
    MutableArraySequence<int> seq(raw, 4);

    int sum = seq.Reduce<int>(sumHelper, 0);
    EXPECT_EQ(sum, 10);

    MutableArraySequence<int> emptySeq;
    int emptySum = emptySeq.Reduce<int>(sumHelper, 42);
    EXPECT_EQ(emptySum, 42);
}

// --- bitsequence ---

// сборка битовой последовательности и чтение битов
TEST(BitSequenceTest, ConstructorAndGetBit) {
    bool raw[] = {true, false, true};
    BitSequence bits(raw, 3);

    EXPECT_EQ(bits.GetLength(), 3);
    EXPECT_TRUE(bits.GetBit(0));
    EXPECT_FALSE(bits.GetBit(1));
    EXPECT_TRUE(bits.GetBit(2));
}

// побитовые операции and, or, xor и несовпадение длин
TEST(BitSequenceTest, LogicOperationsAndLengthMismatch) {
    bool raw1[] = {true, false, true};
    bool raw2[] = {false, false, true};
    BitSequence b1(raw1, 3);
    BitSequence b2(raw2, 3);

    BitSequence* andRes = b1.And(b2);
    EXPECT_FALSE(andRes->GetBit(0));
    EXPECT_TRUE(andRes->GetBit(2));
    delete andRes;

    BitSequence* orRes = b1.Or(b2);
    EXPECT_TRUE(orRes->GetBit(0));
    EXPECT_FALSE(orRes->GetBit(1));
    delete orRes;

    BitSequence* xorRes = b1.Xor(b2);
    EXPECT_TRUE(xorRes->GetBit(0));
    EXPECT_FALSE(xorRes->GetBit(2));
    delete xorRes;

    bool rawDiff[] = {true};
    BitSequence bDiff(rawDiff, 1);
    EXPECT_THROW(b1.And(bDiff), std::invalid_argument);
    EXPECT_THROW(b1.Or(bDiff), std::invalid_argument);
    EXPECT_THROW(b1.Xor(bDiff), std::invalid_argument);
}

// побитовое отрицание not
TEST(BitSequenceTest, NotOperation) {
    bool raw[] = {true, false};
    BitSequence bits(raw, 2);

    BitSequence* notRes = bits.Not();
    EXPECT_FALSE(notRes->GetBit(0));
    EXPECT_TRUE(notRes->GetBit(1));
    delete notRes;
}

// установка бита и проверка диапазона
TEST(BitSequenceTest, SetBitAndBounds) {
    bool raw[] = {false, false};
    BitSequence bits(raw, 2);

    bits.SetBit(1, true);
    EXPECT_FALSE(bits.GetBit(0));
    EXPECT_TRUE(bits.GetBit(1));

    EXPECT_THROW(bits.SetBit(-1, true), std::out_of_range);
    EXPECT_THROW(bits.SetBit(2, true), std::out_of_range);
}

// --- sequence stats ---

// подсчет min, max, avg и обработка пустой последовательности
TEST(SequenceStatsTest, ComputeMinMaxAvgCalculations) {
    int raw1[] = {-10, -5, -20};
    MutableArraySequence<int> seq1(raw1, 3);
    MinMaxAvg<int> stats1 = ComputeMinMaxAvg(&seq1);
    EXPECT_EQ(stats1.min, -20);
    EXPECT_EQ(stats1.max, -5);
    EXPECT_DOUBLE_EQ(stats1.avg, -35.0 / 3.0);

    int raw2[] = {42};
    MutableArraySequence<int> seq2(raw2, 1);
    MinMaxAvg<int> stats2 = ComputeMinMaxAvg(&seq2);
    EXPECT_EQ(stats2.min, 42);
    EXPECT_EQ(stats2.max, 42);
    EXPECT_DOUBLE_EQ(stats2.avg, 42.0);

    MutableArraySequence<int> emptySeq;
    EXPECT_THROW(ComputeMinMaxAvg(&emptySeq), std::invalid_argument);
}