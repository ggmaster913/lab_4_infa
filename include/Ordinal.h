#ifndef ORDINAL_H
#define ORDINAL_H

#include <stdexcept>

// ω*omegaCount + finitePart
class Ordinal {
private:
    int omegaCount;
    int finitePart;
public:
    Ordinal() : omegaCount(0), finitePart(0) {}
    explicit Ordinal(int finitePart) {
        if (finitePart < 0) throw std::invalid_argument("конечный хвост не может быть < 0");
        this->finitePart = finitePart;
        this->omegaCount = 0;
    }
    Ordinal(int omegaCount, int finitePart) {
        if (finitePart < 0 || omegaCount < 0) throw std::invalid_argument("конечный хвост или число бесконечных кусков не может быть < 0 ");
        this->finitePart = finitePart;
        this->omegaCount = omegaCount;
    }
    static Ordinal Omega() {
        return Ordinal(1, 0);
    }

    bool IsFinite() const {
        if (omegaCount) return false;
        return true;
    }
    int GetOmegaCount() const {
        return omegaCount;
    }
    int GetFinitePart() const {
        return finitePart;
    }

    Ordinal operator+(const Ordinal& other) const {
        // справа ω, наш хвост поглощается: 5 + ω = ω
        if (other.omegaCount > 0) return Ordinal(other.omegaCount + omegaCount, other.finitePart);
        // справа конечное, хвосты складываются: ω + 5
        return Ordinal(omegaCount, other.finitePart + finitePart);
    }

    bool operator==(const Ordinal& other) const {
        if (omegaCount == other.omegaCount && finitePart == other.finitePart) return true;
        return false;
    }

    bool operator!=(const Ordinal& other) const {
        if (omegaCount != other.omegaCount || finitePart != other.finitePart) return true;
        return false;
    }

    bool operator<(const Ordinal& other) const {
        if (omegaCount < other.omegaCount) return true;
        if (omegaCount == other.omegaCount) {
            if (finitePart < other.finitePart) return true;
            return false;
        }
        return false;
    }

    bool operator>(const Ordinal& other) const {
        if (omegaCount > other.omegaCount) return true;
        if (omegaCount == other.omegaCount) {
            if (finitePart > other.finitePart) return true;
            return false;
        }
        return false;
    }

    bool operator>=(const Ordinal& other) const {
        return !(*this < other);
    }

    bool operator<=(const Ordinal& other) const {
        return !(other < *this);
    }
};

#endif
