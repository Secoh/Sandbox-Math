// Thie purpose of this software is to debug certain functions of SKLib, and to provide example how to use them:
//      Euclidean Divide for regular signed integers, and for signed integer software emulator
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
#include <random>

#define SKLIB_TARGET_TEST
#include <SKLib/sklib.hpp>
#include <SKLib/include/math.hpp>

int main()
{
    sklib::timer_stopwatch_type strobe(1000);
    sklib::random_size_integer_device<int64_t, 63> RND;
    sklib::random_size_integer_device<uint64_t, 64> uRND;

    uint64_t K = 0;
    while (true)
    {
        int64_t iA = RND();
        int64_t iB = RND();

        sklib::signed_uint<uint64_t> exA{ uRND(), (iA >= 0) };
        sklib::signed_uint<uint64_t> exB{ uRND(), (iB >= 0) };

        if (iB)
        {
            int64_t R=0;
            auto Q = sklib::edivrem(iA, iB, &R);

            if (R < 0 || R >= std::abs(iB) || Q*iB+R != iA)
            {
                std::cout << "Error, edivrem(i64) is incorrect\n"
                    << "iA=" << iA << "; iB=" << iB << "; Q=" << Q << "; R=" << R << "\n";
                return 1;
            }
        }

        if (exB.abs())
        {
            sklib::signed_uint<uint64_t> R = 0;
            auto Q = sklib::edivrem(exA, exB, &R);

            if (R.sign() < 0 || R.abs() >= exB.abs() || (Q*exB+R - exA).abs())
            {
                std::cout << "Error, edivrem(i65) is incorrect\n"
                    << "A=" << (exA.sign()<0 ? "-" : "") << exA.abs()
                    << "; B=" << (exB.sign()<0 ? "-" : "") << exB.abs()
                    << "; Q=" << (Q.sign()<0 ? "-" : "") << Q.abs()
                    << "; R=" << (R.sign()<0 ? "-" : "") << R.abs() << "\n";
                return 1;
            }
        }

        if (++K, strobe(true)) std::cout << K << "\r" << std::flush;
    }

    //    return 0
}

