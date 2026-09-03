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

    BigInt &operator<<=(int count);
    BigInt operator<<(int count) const;

    BigInt &operator&=(const BigInt &other);
    BigInt operator&(const BigInt &other) const;

    void Print() const;
    friend std::ostream &operator<<(std::ostream &out, const BigInt &value);

private:
    std::vector<int> digits;
    bool negative;

    void Normalize();
    void SubtractMagnitude(const BigInt &other);
    bool IsZero() const;

    std::vector<int> ToBits() const;
    std::vector<int> ToTwosComplement(std::size_t width) const;
    static BigInt FromTwosComplement(const std::vector<int> &bits);
    static void AddOneToBits(std::vector<int> &bits);
    static BigInt FromBits(const std::vector<int> &bits);

    enum class MagnitudeComparison
    {
        LESSER,
        EQUAL,
        GREATER
    };
    MagnitudeComparison CompareMagnitude(const BigInt &other) const;
};

BigInt pow(const BigInt &base, int exponent);
BigInt pow(const BigInt &base, BigInt exponent);
