// ring-test-bezout-int.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

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
        auto iA = RND();
        auto iB = RND();

        sklib::signed_uint<uint64_t> exA{ uRND(), (iA >= 0) };
        sklib::signed_uint<uint64_t> exB{ uRND(), (iB >= 0) };

        if (iA && iB)
        {
            int64_t kA, kB;
            auto D = sklib::bezout(iA, iB, kA, kB);

            if (D <= 0 || std::abs(iA) % D || std::abs(iB) % D || kA*iA + kB*iB != D)
            {
                std::cout << "Error, bezout(i64) is incorrect\n"
                    << "iA=" << iA << "; iB=" << iB << "; D=" << D << "; kA=" << kA << "; kB=" << kB << "\n";
                return 1;
            }
        }

        if (exA && exB)
        {
            sklib::signed_uint<uint64_t> kA, kB;
            auto D = sklib::bezout(exA, exB, kA, kB);

            if (D <= 0 || exA % D || exB % D /*|| kA*exA + kB*exB != D*/)
            {
                std::cout << "Error, bezout(i65) is incorrect\n" ;
//                    << "iA=" << iA << "; iB=" << iB << "; D=" << D << "; kA=" << kA << "; kB=" << kB << "\n";
                return 1;
            }
        }

        if (++K, strobe(true)) std::cout << K << "\r" << std::flush;
    }

    //    return 0
}

