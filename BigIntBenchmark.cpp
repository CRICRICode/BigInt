#include "BigInt.h"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    using Clock = std::chrono::steady_clock;

    std::string MakeDecimal(std::size_t digitCount, unsigned int seed)
    {
        std::string text;
        text.reserve(digitCount);

        text.push_back(static_cast<char>('1' + (seed % 9)));
        for (std::size_t index = 1; index < digitCount; ++index)
        {
            const unsigned int digit = (seed + static_cast<unsigned int>(index * 7)) % 10;
            text.push_back(static_cast<char>('0' + digit));
        }

        return text;
    }

    template <typename Operation>
    void Measure(const char* name, std::size_t repetitions, Operation operation)
    {
        BigInt lastResult;

        const auto start = Clock::now();
        for (std::size_t iteration = 0; iteration < repetitions; ++iteration)
        {
            lastResult = operation();
        }
        const auto end = Clock::now();

        const std::chrono::duration<double, std::milli> elapsed = end - start;
        std::ostringstream resultText;
        resultText << lastResult;

        std::cout << std::left << std::setw(18) << name
                  << std::right << std::setw(8) << repetitions << " runs  "
                  << std::fixed << std::setprecision(3) << elapsed.count() << " ms"
                  << "  (result digits: " << resultText.str().size() << ")\n";
    }

    void BenchmarkArithmetic(std::size_t digitCount,
                             std::size_t addRepetitions,
                             std::size_t multiplyRepetitions,
                             std::size_t divideRepetitions)
    {
        const BigInt left(MakeDecimal(digitCount, 3));
        const BigInt right(MakeDecimal(digitCount, 7));
        const BigInt divisor(MakeDecimal(digitCount / 2 + 1, 5));

        std::cout << "\n--- " << digitCount << " decimal digits ---\n";
        Measure("addition", addRepetitions, [&] { return left + right; });
        Measure("multiplication", multiplyRepetitions, [&] { return left * right; });
        Measure("division", divideRepetitions, [&] { return left / divisor; });
    }
}

int main()
{
    std::cout << "BigInt benchmark (compile with optimizations, e.g. -O2)\n";

    BenchmarkArithmetic(16, 20'000, 2'000, 2'000);
    BenchmarkArithmetic(64, 5'000, 300, 500);
    BenchmarkArithmetic(256, 1'000, 30, 100);

    const BigInt base(2);
    std::cout << "\n--- exponentiation ---\n";
    Measure("pow(2, 512)", 20, [&] { return pow(base, 512); });
}
