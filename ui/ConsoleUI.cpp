#include <iostream>
#include <stdexcept>
#include <random>
#include <chrono>
#include <limits>
#include <cstdlib>

#include "LazySequence.h"
#include "LazyAlgebra.h"
#include "Stream.h"
#include "lab2/MutableArraySequence.h"
#include "lab2/ImmutableArraySequence.h"
#include "lab2/MutableListSequence.h"

// --- слоты: меню владеет всеми последовательностями в них ---

const int SLOT_COUNT = 10;
LazySequence<long long>* slots[SLOT_COUNT] = {nullptr};

void FreeAllSlots() {
    for (int i = 0; i < SLOT_COUNT; i++) {
        delete slots[i];
        slots[i] = nullptr;
    }
}

// --- ввод и вывод ---

// читает целое число нужного типа, при неверном вводе спрашивает снова
template<typename T>
T ReadNumber(const char* prompt) {
    while (true) {
        std::cout << prompt;
        T value;
        if (std::cin >> value) return value;
        if (std::cin.eof()) {
            std::cout << "\nКонец ввода.\n";
            FreeAllSlots();
            std::exit(0);
        }
        std::cout << "Ошибка: нужно целое число.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

const char* YesNo(bool value) {
    return value ? "да" : "нет";
}

// печатает ординал вида ω·a + b
void PrintOrdinal(const Ordinal& value) {
    if (value.GetOmegaCount() == 0) {
        std::cout << value.GetFinitePart();
        return;
    }
    std::cout << "ω";
    if (value.GetOmegaCount() > 1) std::cout << "·" << value.GetOmegaCount();
    if (value.GetFinitePart() > 0) std::cout << "+" << value.GetFinitePart();
}

// длина, первые count элементов (у бесконечной дальше «...») и сколько посчитано в кэше
void PrintSequence(LazySequence<long long>* seq, int count) {
    Ordinal length = seq->GetLength();
    int shown = count;
    if (length.IsFinite() && length.GetFinitePart() < shown) shown = length.GetFinitePart();
    std::cout << "длина ";
    PrintOrdinal(length);
    std::cout << ": [";
    for (int i = 0; i < shown; i++) std::cout << (i > 0 ? ", " : "") << seq->Get(i);
    if (!(length == Ordinal(shown))) std::cout << (shown > 0 ? ", ..." : "...");
    std::cout << "]  (посчитано " << seq->GetMaterializedCount() << ")\n";
}

long long MillisecondsSince(std::chrono::steady_clock::time_point begin) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count();
}

// --- правила и функции для операций ---

long long ArithmeticRule(const Sequence<long long>* last) { return 2 * last->Get(1) - last->Get(0); }
long long FibonacciRule(const Sequence<long long>* last) { return last->Get(0) + last->Get(1); }
long long DoubleRule(const Sequence<long long>* last) { return 2 * last->Get(0); }
long long Square(const long long& x) { return x * x; }
bool IsEven(const long long& x) { return x % 2 == 0; }
long long Sum(const long long& accumulator, const long long& x) { return accumulator + x; }

// --- работа со слотами ---

void ShowSlots() {
    bool empty = true;
    for (int i = 0; i < SLOT_COUNT; i++) {
        if (slots[i] == nullptr) continue;
        empty = false;
        std::cout << "  [" << i << "] ";
        PrintSequence(slots[i], 8);
    }
    if (empty) std::cout << "  слотов нет, создайте последовательность (пункт 2)\n";
}

LazySequence<long long>* ChooseSlot(const char* prompt) {
    int index = ReadNumber<int>(prompt);
    if (index < 0 || index >= SLOT_COUNT || slots[index] == nullptr) throw std::out_of_range("такого слота нет");
    return slots[index];
}

// кладёт новую последовательность в свободный слот; если мест нет — удаляет её
void Store(LazySequence<long long>* seq) {
    for (int i = 0; i < SLOT_COUNT; i++) {
        if (slots[i] == nullptr) {
            slots[i] = seq;
            std::cout << "Результат в слоте [" << i << "]: ";
            PrintSequence(seq, 8);
            return;
        }
    }
    delete seq;
    throw std::out_of_range("нет свободных слотов, удалите какой-нибудь (пункт 9)");
}

LazySequence<long long>* RandomSequence(int length, long long low, long long high, std::mt19937_64& random) {
    std::uniform_int_distribution<long long> distribution(low, high);
    DynamicArray<long long> items(length);
    for (int i = 0; i < length; i++) items.Set(i, distribution(random));
    return new LazySequence<long long>(new ArrayGenerator<long long>(items), Ordinal(length));
}

// --- пункты меню ---

void MenuOrdinals() {
    std::cout << "Ординал ω·a + b задаётся двумя числами a и b.\n";
    int a = ReadNumber<int>("x: a = ");
    int b = ReadNumber<int>("x: b = ");
    Ordinal x(a, b);
    a = ReadNumber<int>("y: a = ");
    b = ReadNumber<int>("y: b = ");
    Ordinal y(a, b);

    std::cout << "x + y = "; PrintOrdinal(x + y);
    std::cout << ",  y + x = "; PrintOrdinal(y + x);
    std::cout << "\nx < y: " << YesNo(x < y) << ",  x == y: " << YesNo(x == y) << ",  x > y: " << YesNo(x > y) << "\n";
    std::cout << "Для сравнения: 1 + ω = "; PrintOrdinal(Ordinal(1) + Ordinal::Omega());
    std::cout << ",  ω + 1 = "; PrintOrdinal(Ordinal::Omega() + Ordinal(1));
    std::cout << ",  (ω+5) + (ω+3) = "; PrintOrdinal(Ordinal(1, 5) + Ordinal(1, 3));
    std::cout << "\n";
}

void MenuCreate() {
    std::cout << "1. ввести с клавиатуры   2. случайные числа   3. арифметическая прогрессия (бесконечная)\n"
                 "4. Фибоначчи (бесконечная)   5. степени двойки (бесконечная)   6. готовая {7, 8, 9}   7. пустая\n";
    int choice = ReadNumber<int>("Выбор: ");

    if (choice == 1 || choice == 2) {
        int count = ReadNumber<int>("Количество (0..100000): ");
        if (count < 0 || count > 100000) throw std::invalid_argument("количество должно быть от 0 до 100000");
        if (choice == 2) {
            long long low = ReadNumber<long long>("Минимум: ");
            long long high = ReadNumber<long long>("Максимум: ");
            if (low > high) throw std::invalid_argument("минимум больше максимума");
            std::mt19937_64 random(std::random_device{}());
            Store(RandomSequence(count, low, high, random));
            return;
        }
        DynamicArray<long long> items(count);
        for (int i = 0; i < count; i++) items.Set(i, ReadNumber<long long>("  элемент: "));
        Store(new LazySequence<long long>(new ArrayGenerator<long long>(items), Ordinal(count)));
    } else if (choice >= 3 && choice <= 5) {
        long long start[2];
        int startCount = 2;
        long long (*rule)(const Sequence<long long>*) = ArithmeticRule;
        if (choice == 3) {
            start[0] = ReadNumber<long long>("Первый член: ");
            start[1] = start[0] + ReadNumber<long long>("Шаг: ");
        } else if (choice == 4) {
            start[0] = ReadNumber<long long>("Первый член: ");
            start[1] = ReadNumber<long long>("Второй член: ");
            rule = FibonacciRule;
        } else {
            start[0] = ReadNumber<long long>("Первое число: ");
            startCount = 1;
            rule = DoubleRule;
        }
        MutableArraySequence<long long> initial(start, startCount);
        Store(new LazySequence<long long>(rule, &initial));
    } else if (choice == 6) {
        long long raw[] = {7, 8, 9};
        Store(new LazySequence<long long>(raw, 3));
    } else if (choice == 7) {
        Store(new LazySequence<long long>());
    } else {
        std::cout << "Нет такого пункта.\n";
    }
}

void MenuShow() {
    LazySequence<long long>* seq = ChooseSlot("Слот: ");
    int count = ReadNumber<int>("Сколько элементов показать: ");
    if (count < 0) throw std::invalid_argument("количество не может быть отрицательным");
    PrintSequence(seq, count);
}

void MenuOperations() {
    LazySequence<long long>* seq = ChooseSlot("Слот: ");
    std::cout << "Исходная: ";
    PrintSequence(seq, 8);
    std::cout << "1. Get(i)   2. Get по ординалу   3. GetFirst и GetLast   4. Reduce (сумма)\n"
                 "5. Append   6. Prepend   7. InsertAt   8. Concat со вторым слотом\n"
                 "9. GetSubSequence   10. Map (x*x)   11. Where (чётные; у бесконечной без чётных зависнет)\n";
    int choice = ReadNumber<int>("Операция: ");

    if (choice == 1) {
        int index = ReadNumber<int>("Индекс: ");
        long long value = seq->Get(index);
        std::cout << "Get(" << index << ") = " << value << "\n";
    } else if (choice == 2) {
        int a = ReadNumber<int>("Индекс ω·a + b, a = ");
        int b = ReadNumber<int>("b = ");
        Ordinal index(a, b);
        long long value = seq->Get(index);
        std::cout << "Get("; PrintOrdinal(index); std::cout << ") = " << value << "\n";
    } else if (choice == 3) {
        std::cout << "GetFirst() = " << seq->GetFirst() << "\n";
        std::cout << "GetLast() = " << seq->GetLast() << "\n";
    } else if (choice == 4) {
        std::cout << "Сумма = " << seq->Reduce(Sum, 0LL) << "\n";
    } else if (choice == 5) {
        Store(seq->Append(ReadNumber<long long>("Элемент: ")));
    } else if (choice == 6) {
        Store(seq->Prepend(ReadNumber<long long>("Элемент: ")));
    } else if (choice == 7) {
        long long item = ReadNumber<long long>("Элемент: ");
        int index = ReadNumber<int>("Индекс: ");
        Store(seq->InsertAt(item, index));
    } else if (choice == 8) {
        Store(seq->Concat(ChooseSlot("Второй слот: ")));
    } else if (choice == 9) {
        int startIndex = ReadNumber<int>("Начальный индекс: ");
        int endIndex = ReadNumber<int>("Конечный индекс: ");
        Store(seq->GetSubSequence(startIndex, endIndex));
    } else if (choice == 10) {
        Store(seq->Map(Square));
    } else if (choice == 11) {
        Store(seq->Where(IsEven));
    } else {
        std::cout << "Нет такого пункта.\n";
    }
}

void MenuLaws() {
    LazySequence<long long>* a = ChooseSlot("Слот A: ");
    LazySequence<long long>* b = ChooseSlot("Слот B: ");
    LazySequence<long long>* c = ChooseSlot("Слот C: ");
    int count = ReadNumber<int>("Сколько первых элементов сравнивать: ");

    std::cout << "(A ++ B) ++ C == A ++ (B ++ C):       " << YesNo(CheckAssociativity(a, b, c, count)) << "\n";
    std::cout << "{} ++ A == A ++ {} == A:              " << YesNo(CheckIdentity(a, count)) << "\n";
    std::cout << "A ++ B == B ++ A:                     " << YesNo(CheckCommutativity(a, b, count)) << "\n";
    std::cout << "длина(A ++ B) == длина(A) + длина(B): " << YesNo(CheckLengthAddition(a, b)) << "\n";

    LazySequence<long long>* ab = a->Concat(b);
    LazySequence<long long>* ba = b->Concat(a);
    std::cout << "длина A ++ B = "; PrintOrdinal(ab->GetLength());
    std::cout << ",  длина B ++ A = "; PrintOrdinal(ba->GetLength());
    std::cout << "\n";
    delete ab;
    delete ba;
}

void MenuStreams() {
    std::cout << "1. прочитать из слота   2. записать с клавиатуры в список   3. записать в неизменяемую\n";
    int choice = ReadNumber<int>("Выбор: ");

    if (choice == 1) {
        LazyReadStream<long long> stream(ChooseSlot("Слот: "));
        stream.Open();
        int count = ReadNumber<int>("Сколько прочитать: ");
        for (int i = 0; i < count && !stream.IsEndOfStream(); i++) {
            long long value = stream.Read();
            std::cout << "  Read() = " << value << ", позиция " << stream.GetPosition() << "\n";
        }
        if (stream.IsEndOfStream()) std::cout << "  конец потока\n";
        int position = ReadNumber<int>("Seek на позицию (-1 — пропустить): ");
        if (position >= 0) {
            stream.Seek(position);
            long long value = stream.Read();
            std::cout << "  после Seek(" << position << ") Read() = " << value << "\n";
        }
    } else if (choice == 2) {
        MutableListSequence<long long> target;
        SequenceWriteStream<long long> stream(&target);
        stream.Open();
        int count = ReadNumber<int>("Сколько записать: ");
        for (int i = 0; i < count; i++) {
            int position = stream.Write(ReadNumber<long long>("  элемент: "));
            std::cout << "  записано, позиция " << position << "\n";
        }
        std::cout << "Список: [";
        for (int i = 0; i < target.GetLength(); i++) std::cout << (i > 0 ? ", " : "") << target.Get(i);
        std::cout << "]\n";
    } else if (choice == 3) {
        ImmutableArraySequence<long long> target;
        SequenceWriteStream<long long> stream(&target);
        stream.Open();
        std::cout << "Write(42) в ImmutableArraySequence...\n";
        stream.Write(42);
    } else {
        std::cout << "Нет такого пункта.\n";
    }
}

void MenuAuto() {
    int runs = ReadNumber<int>("Число прогонов: ");
    int maxLength = ReadNumber<int>("Наибольшая длина последовательности: ");
    long long low = ReadNumber<long long>("Минимум значений: ");
    long long high = ReadNumber<long long>("Максимум значений: ");
    if (runs <= 0 || maxLength < 0 || low > high) throw std::invalid_argument("некорректные параметры");

    std::mt19937_64 random(std::random_device{}());
    std::uniform_int_distribution<int> lengthDistribution(0, maxLength);
    int count = 3 * maxLength;
    for (int run = 1; run <= runs; run++) {
        LazySequence<long long>* a = RandomSequence(lengthDistribution(random), low, high, random);
        LazySequence<long long>* b = RandomSequence(lengthDistribution(random), low, high, random);
        LazySequence<long long>* c = RandomSequence(lengthDistribution(random), low, high, random);
        std::cout << "Прогон " << run << " (длины " << a->GetLength().GetFinitePart() << ", "
                  << b->GetLength().GetFinitePart() << ", " << c->GetLength().GetFinitePart() << "): "
                  << "ассоциативность " << YesNo(CheckAssociativity(a, b, c, count))
                  << ", нейтральный " << YesNo(CheckIdentity(a, count))
                  << ", длины " << YesNo(CheckLengthAddition(a, b))
                  << ", коммутативность " << YesNo(CheckCommutativity(a, b, count)) << "\n";
        delete a;
        delete b;
        delete c;
    }

    long long naturalStart[] = {0, 1};
    long long fibonacciStart[] = {1, 1};
    MutableArraySequence<long long> naturalInitial(naturalStart, 2);
    MutableArraySequence<long long> fibonacciInitial(fibonacciStart, 2);
    LazySequence<long long> naturals(ArithmeticRule, &naturalInitial);
    LazySequence<long long> fibonacci(FibonacciRule, &fibonacciInitial);
    LazySequence<long long>* withTail = naturals.Append(42);
    std::cout << "Бесконечные (натуральные, Фибоначчи, натуральные + 42): "
              << "ассоциативность " << YesNo(CheckAssociativity(&naturals, &fibonacci, withTail, 50))
              << ", нейтральный " << YesNo(CheckIdentity(&fibonacci, 50))
              << ", длины " << YesNo(CheckLengthAddition(&naturals, withTail))
              << ", коммутативность " << YesNo(CheckCommutativity(&naturals, &fibonacci, 50)) << "\n";
    delete withTail;
}

void MenuStress() {
    int n = ReadNumber<int>("N (1..3000000, например 1000000): ");
    if (n <= 0 || n > 3000000) throw std::invalid_argument("N должно быть от 1 до 3000000");
    long long start[] = {0, 1};
    MutableArraySequence<long long> initial(start, 2);

    LazySequence<long long> naturals(ArithmeticRule, &initial);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    long long last = naturals.Get(n - 1);
    std::cout << "Get(N-1) = " << last << ", " << MillisecondsSince(begin) << " мс\n";

    begin = std::chrono::steady_clock::now();
    LazySequence<long long>* squares = naturals.Map(Square);
    LazySequence<long long>* prefix = squares->GetSubSequence(0, n - 1);
    long long total = prefix->Reduce(Sum, 0LL);
    delete prefix;
    delete squares;
    std::cout << "Map + GetSubSequence + Reduce: сумма квадратов = " << total << ", " << MillisecondsSince(begin) << " мс\n";

    LazySequence<long long> fresh(ArithmeticRule, &initial);
    LazyReadStream<long long> reader(&fresh);
    reader.Open();
    begin = std::chrono::steady_clock::now();
    long long sum = 0;
    for (int i = 0; i < n; i++) sum += reader.Read();
    std::cout << "N раз Read: сумма = " << sum << ", " << MillisecondsSince(begin) << " мс\n";

    MutableListSequence<long long> target;
    SequenceWriteStream<long long> writer(&target);
    writer.Open();
    begin = std::chrono::steady_clock::now();
    for (int i = 0; i < n; i++) writer.Write(i);
    std::cout << "N раз Write в список: длина = " << target.GetLength() << ", " << MillisecondsSince(begin) << " мс\n";
}

void MenuDelete() {
    int index = ReadNumber<int>("Слот: ");
    if (index < 0 || index >= SLOT_COUNT || slots[index] == nullptr) throw std::out_of_range("такого слота нет");
    delete slots[index];
    slots[index] = nullptr;
    std::cout << "Слот [" << index << "] удалён.\n";
}

int main() {
    while (true) {
        std::cout << "\n=== ЛР-4: ленивые списки и ординалы ===\n";
        ShowSlots();
        std::cout << "1. Ординалы\n"
                     "2. Создать последовательность\n"
                     "3. Показать последовательность\n"
                     "4. Операции над последовательностью\n"
                     "5. Законы моноида\n"
                     "6. Потоки\n"
                     "7. Автоматический прогон законов\n"
                     "8. Предельные значения (миллионы элементов)\n"
                     "9. Удалить последовательность\n"
                     "0. Выход\n";
        int choice = ReadNumber<int>("Выбор: ");
        if (choice == 0) break;
        try {
            switch (choice) {
                case 1: MenuOrdinals(); break;
                case 2: MenuCreate(); break;
                case 3: MenuShow(); break;
                case 4: MenuOperations(); break;
                case 5: MenuLaws(); break;
                case 6: MenuStreams(); break;
                case 7: MenuAuto(); break;
                case 8: MenuStress(); break;
                case 9: MenuDelete(); break;
                default: std::cout << "Нет такого пункта.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }
    FreeAllSlots();
    return 0;
}
