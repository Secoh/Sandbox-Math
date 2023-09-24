// ring-test-int65.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdio>

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

    srand((unsigned)time(nullptr));

    uint64_t K = 0;
    while (true)
    {
        int64_t A = (rand() & 0xFFF) + ((rand() & 0xFFF) << 12);
        if (rand() & 1) A = -A;
        int64_t B = (rand() & 0xFFF) + ((rand() & 0xFFF) << 12);
        if (rand() & 1) B = -B;

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

