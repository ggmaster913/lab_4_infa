#pragma once

class Bit {
    bool value;
public:
    Bit();
    // защита от неявных преобразований (типа s.Append(7);  может молча перевести в бит)
    explicit Bit(bool value);
    explicit Bit(int value);

    bool GetValue() const;
    void SetValue(bool value);

    // И: 1 только если оба 1
    Bit operator&(const Bit& other) const;

    // ИЛИ: 1 если хоть один 1
    Bit operator|(const Bit& other) const;

    // исключающее ИЛИ: 1 если биты разные
    Bit operator^(const Bit& other) const;

    // НЕ: переворачивает бит
    Bit operator~() const;

    // равны ли  два бита
    bool operator==(const Bit& other) const;
};
