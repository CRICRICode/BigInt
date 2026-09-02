#pragma once

#include <string>
#include <vector>
#include <ostream>

class BigInt
{
public:
    BigInt();
    BigInt(int value);
    explicit BigInt(const std::string &text);

    BigInt &operator+=(const BigInt &other);
    BigInt operator+(const BigInt &other) const;
    BigInt &operator++();
    BigInt operator++(int);

    BigInt &operator-=(const BigInt &other);
    BigInt operator-(const BigInt &other) const;
    BigInt operator-() const;
    BigInt &operator--();
    BigInt operator--(int);

    BigInt &operator*=(const BigInt &other);
    BigInt operator*(const BigInt &other) const;

    BigInt &operator/=(const BigInt &other);
    BigInt operator/(const BigInt &other) const;

    BigInt &operator%=(const BigInt &other);
    BigInt operator%(const BigInt &other) const;

    bool operator==(const BigInt &other) const;
    bool operator!=(const BigInt &other) const;

    bool operator>(const BigInt &other) const;
    bool operator>=(const BigInt &other) const;
    bool operator<(const BigInt &other) const;
    bool operator<=(const BigInt &other) const;

    void Print() const;
    friend std::ostream &operator<<(std::ostream &out, const BigInt &value);

private:
    std::vector<int> digits;
    bool negative;

    void Normalize();
    void SubtractMagnitude(const BigInt &other);
    bool IsZero() const;

    enum class MagnitudeComparison
    {
        LESSER,
        EQUAL,
        GREATER
    };
    MagnitudeComparison CompareMagnitude(const BigInt &other) const;
};

BigInt pow(const BigInt &base, int exponent);