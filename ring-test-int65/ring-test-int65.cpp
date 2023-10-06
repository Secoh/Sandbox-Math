// Thie purpose of this software is to debug certain functions of SKLib, and to provide example how to use them:
//      Singed integer simulator (unsigned integer plus sign bit)
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
#include <cstdio>

#define SKLIB_TARGET_TEST
#include <SKLib/sklib.hpp>
#include <SKLib/include/math.hpp>

int i64_sign(int64_t V)
{
    if (!V) return 0;
    if (V<0) return -1;
    return 1;
}

int main()
{
    sklib::timer_stopwatch_type strobe(1000);
    sklib::random_size_integer_device<int32_t, 31> RND;

    srand((unsigned)time(nullptr));

    uint64_t K = 0;
    while (true)
    {
        int64_t A = RND();
        int64_t B = RND();
        sklib::signed_uint<uint64_t> exA{A};
        sklib::signed_uint<uint64_t> exB{B};

        auto S = exA + exB;
        auto D = exA - exB;
        auto P = exA * exB;

        auto S1 = A + B;
        auto D1 = A - B;
        auto P1 = A * B;

        auto sx = exA + exB.abs();
        auto sy = exA; sy += exB.abs();
        auto sz = A + std::abs(B);

        if (S.abs() != std::abs(S1) || S.sign() != i64_sign(S1))
        {
            std::cout << "Error, addition is incorrect\n"
                << "A=" << (i64_sign(A)<0 ? "-" : "") << std::abs(A)
                << "; B=" << (i64_sign(B)<0 ? "-" : "") << std::abs(B)
                << "; S=" << (S.sign()<0 ? "-" : "") << S.abs() << "\n";
            return 1;
        }

        if (D.abs() != std::abs(D1) || D.sign() != i64_sign(D1))
        {
            std::cout << "Error, subtraction is incorrect\n"
                << "A=" << (i64_sign(A)<0 ? "-" : "") << std::abs(A)
                << "; B=" << (i64_sign(B)<0 ? "-" : "") << std::abs(B)
                << "; D=" << (D.sign()<0 ? "-" : "") << D.abs() << "\n";
            return 1;
        }

        if (P.abs() != std::abs(P1) || P.sign() != i64_sign(P1))
        {
            std::cout << "Error, product is incorrect\n"
                << "A=" << (i64_sign(A)<0 ? "-" : "") << std::abs(A)
                << "; B=" << (i64_sign(B)<0 ? "-" : "") << std::abs(B)
                << "; P=" << (P.sign()<0 ? "-" : "") << P.abs() << "\n";
            return 1;
        }

        if ((sx-sz).abs() || (sz-sy).sign())
        {
            std::cout << "Error, conversion from integers is incorrect\n"
                << "A=" << (i64_sign(A)<0 ? "-" : "") << std::abs(A)
                << "; B=" << (i64_sign(B)<0 ? "-" : "") << std::abs(B) << "\n";
            return 1;
        }

        if (++K, strobe(true)) std::cout << K << "\r" << std::flush;
    }

    //    return 0
}

