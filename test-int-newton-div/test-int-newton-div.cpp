// test-int-newton-div.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#define SKLIB_TARGET_TEST
#include "SKLib/include/types.hpp"
#include "SKLib/include/utility.hpp"
#include "SKLib/include/bitwise.hpp"
#include "SKLib/include/timer.hpp"

// model with normal integers
bool mdiv(uint32_t A, uint32_t B, uint32_t& Q, uint32_t& R, uint64_t salt)
{
    constexpr unsigned base_bits_max = sklib::bits_width_v<uint32_t>;
    const unsigned base_bits_min = sklib::bits_rank(A);
    const unsigned base_bits = base_bits_min + salt % (base_bits_max-base_bits_min+1);

    const uint64_t base = (uint64_t)1 << base_bits;
    const unsigned B_rank = sklib::bits_rank(B);

    // 0. special cases

    if (!B) return (R=1), false;

    if (B_rank >= base_bits)
    {
        if (B_rank != base_bits || A < B)
        {
            Q = 0;
            R = A;
        }
        else
        {
            Q = 1;
            R = A - B;
        }

        return true;
    }

    // 1. divide base by B

    uint64_t x = (uint64_t)1 << (base_bits - B_rank);

    while (true)
    {
        auto x1 = ((x << (base_bits+1)) - B*x*x) >> base_bits;
        if (x1 < x) return (R=2), false;
        if (x1 == x) break;
        x = x1;
    }

    // 1.a. quotent correction

    if (base > B * (x+1)) x++;

    // 2. multiply by dividend, correct quotent again

    Q = (uint32_t)((x*A) >> base_bits);

    R = A - Q*B;
    if (R >= B) { Q++; R-=B; }

    return true;
}

int main()
{
    sklib::random_size_integer_device<uint32_t, 32> RND;
    sklib::timer_stopwatch_type strobe(1000);

    uint64_t K = 0;
    while (true)
    {
        auto A = RND();
        auto B = RND();
        if (!B) continue;
        uint32_t Q=0, R=0;
        if (!mdiv(A, B, Q, R, K) || R >= B || A != B*Q + R)
        {
            std::cout << "A=" << A << " B=" << B << " Q=" << Q << " R=" << R << " (K=" << K << ")\n";
            return -1;
        }

        if (K++, strobe(true)) std::cout << K << "\r" << std::flush;
    }

    // return 0;
}

