// ring-test-ediv-int.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
//#include <cstdio>
#include <random>

#include <SKLib/sklib.hpp>
#include <SKLib/include/math.hpp>

int main()
{
    sklib::timer_stopwatch_type strobe(1000);

    // https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> distrib;

    uint64_t K = 0;
    while (true)
    {
        uint64_t A = distrib(gen);
        uint64_t B = distrib(gen);

        int64_t iA = (A >> 1);
        if (A & 1) iA = -iA;

        int siB = 3 + ((B >> 1) & 0x03);
        int64_t iB = (B >> siB);
        if (B & 1) iB = -iB;
        
        uint64_t sB = 4 + ((B >> 2) & 0x03);
        sklib::signed_uint<uint64_t> exA{ A, bool(B&1) };
        sklib::signed_uint<uint64_t> exB{ (B>>sB), bool(B&2) };

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

