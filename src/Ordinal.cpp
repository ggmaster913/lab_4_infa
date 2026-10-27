#include "Ordinal.h"
#include <stdexcept>

Ordinal::Ordinal() : omegaCount(0), finitePart(0) {}

Ordinal::Ordinal(int finitePart) {
    if (finitePart < 0) throw std::invalid_argument("конечный хвост не может быть < 0");
    this->finitePart = finitePart;
    this->omegaCount = 0;
}

Ordinal::Ordinal(int omegaCount, int finitePart) {
    if (finitePart < 0 || omegaCount < 0) throw std::invalid_argument("конечный хвост или число бесконечных кусков не может быть < 0 ");
    this->finitePart = finitePart;
    this->omegaCount = omegaCount;
}

Ordinal Ordinal::Omega() {
    return Ordinal(1, 0);
}

bool Ordinal::IsFinite() const {
    if (omegaCount) return false;
    return true;
}

int Ordinal::GetOmegaCount() const {
    return omegaCount;
}

int Ordinal::GetFinitePart() const {
    return finitePart;
}

Ordinal Ordinal::operator+(const Ordinal& other) const {
    // справа ω, наш хвост поглощается: 5 + ω = ω
    if (other.omegaCount > 0) return Ordinal(other.omegaCount + omegaCount, other.finitePart);
    // справа конечное, хвосты складываются: ω + 5
    return Ordinal(omegaCount, other.finitePart + finitePart);
}

bool Ordinal::operator==(const Ordinal& other) const {
    if (omegaCount == other.omegaCount && finitePart == other.finitePart) return true;
    return false;
}

bool Ordinal::operator!=(const Ordinal& other) const {
    if (omegaCount != other.omegaCount || finitePart != other.finitePart) return true;
    return false;
}

bool Ordinal::operator<(const Ordinal& other) const {
    if (omegaCount < other.omegaCount) return true;
    if (omegaCount == other.omegaCount) {
        if (finitePart < other.finitePart) return true;
        return false;
    }
    return false;
}

bool Ordinal::operator>(const Ordinal& other) const {
    if (omegaCount > other.omegaCount) return true;
    if (omegaCount == other.omegaCount) {
        if (finitePart > other.finitePart) return true;
        return false;
    }
    return false;
}

bool Ordinal::operator>=(const Ordinal& other) const {
    return !(*this < other);
}

bool Ordinal::operator<=(const Ordinal& other) const {
    return !(other < *this);
}
