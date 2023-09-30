// Thie purpose of this software is to debug certain functions of SKLib, and to provide example how to use them:
//      Modular Arithmetics, integers modulo prime
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

int main()
{
    sklib::timer_stopwatch_type strobe(1000);
    sklib::random_size_integer_device<uint32_t, 32> uR32;
//    sklib::random_size_integer_device<uint64_t, 64> uR64;

    static constexpr uint32_t P32 = 4294967231UL;
//    static constexpr uint16_t P16 = 64901U;

    uint64_t K = 0;
    while (true)
    {
        auto A32 = uR32();
        if (!A32) continue;

        sklib::modp<uint32_t> F(P32, A32);
        if (!F())
        {
            std::cout << "Zero input: A=" << A32 << " F=" << F()
                      << " mod " << P32 << "\n";
            continue;
        }

        auto G = F.reciprocal();

        auto M = F * G;

        if (!M.is_valid() || M() != 1)
        {
            std::cout << "Error, Modular Reciprocal 32 is incorrect\n"
                      << "A=" << A32 << " F=" << F() << " G=" << G()
                      << " M=" << M() << " mod " << P32 << "\n";
            return 1;
        }

        if (++K, strobe(true)) std::cout << K << "\r" << std::flush;
    }
}

