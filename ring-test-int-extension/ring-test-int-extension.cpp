// Thie purpose of this software is to debug certain functions of SKLib, and to provide example how to use them:
//     128-bit multiplication, division support, adding, subtraction for completeness, 32/64 bit also for completeness
//
// Written by Secoh, 2023 // https://github.com/Secoh/Sandbox-Math
//
// This is free and unencumbered software released into the public domain.
// For more information, please refer to: http://unlicense.org/
//
// Software is distributed on "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//
// In any jurisdiction where the Unlicense is not recognized, the following terms shall apply:
// Copyright [2020-2023] Secoh
// Licensed under the MIT License, see: https://opensource.org/license/mit/
//

#include <iostream>

#include <SKLib/sklib.hpp>
#include <SKLib/include/math.hpp>

// will use polynomial (x^2 - 2x + 4) and divide by (x - 1)
// when x > 4, quotent must be (x - 1), remainder 3

template<class T>
bool do_test(T A, uint64_t& hi_count)
{
    if (A < 5) return true;

    auto V = sklib::implementation::uint_extend_t<T>(A).sub(2).mul(A).add(4);
    if (V.hi()) hi_count++;

    auto U = V;  // for reporting

    T R = 0;
    V.div(A-1, &R);

    auto succ = (!V.hi() && V.lo() == A-1 && R == 3);
    if (!succ)
    {
        std::cout << "Error, int extension, probe=" << A << "\n"
                  << "Poly = (" << U.hi() << "," << U.lo() << ")\n"
                  << "Q " << V.lo() << " R " << R << "\n";
    }
    return succ;
}

int main()
{
    sklib::timer_stopwatch_type strobe(1000);
    sklib::random_size_integer_device<uint64_t, 64> uR64;
    sklib::random_size_integer_device<uint32_t, 32> uR32;

    uint64_t K = 0;
    uint64_t H64 = 0;
    uint64_t H32 = 0;

    while (true)
    {
        if (!do_test<uint64_t>(uR64(), H64)) return 1;
        if (!do_test<uint32_t>(uR32(), H32)) return 1;

        if (++K, strobe(true)) std::cout << K << " hi64=" << H64 << " hi32=" << H32 << "\r" << std::flush;
    }
}

