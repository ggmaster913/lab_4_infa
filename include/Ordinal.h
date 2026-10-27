#pragma once

// ω*omegaCount + finitePart
class Ordinal {
private:
    int omegaCount;
    int finitePart;
public:
    Ordinal();
    explicit Ordinal(int finitePart);
    Ordinal(int omegaCount, int finitePart);
    static Ordinal Omega();

    bool IsFinite() const;
    int GetOmegaCount() const;
    int GetFinitePart() const;

    Ordinal operator+(const Ordinal& other) const;

    bool operator==(const Ordinal& other) const;
    bool operator!=(const Ordinal& other) const;
    bool operator<(const Ordinal& other) const;
    bool operator>(const Ordinal& other) const;
    bool operator>=(const Ordinal& other) const;
    bool operator<=(const Ordinal& other) const;
};
