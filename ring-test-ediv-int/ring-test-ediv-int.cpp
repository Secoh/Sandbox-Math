// ring-test-ediv-int.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>

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

