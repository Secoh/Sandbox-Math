// Thie purpose of this software is to debug certain functions of SKLib, and to provide example how to use them:
//      Extended Euclid algorithm, Bezout coefficients, for numerical Euclidean domains
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

            // verify kA*exA + kB*exB == D
            auto tmpA = sklib::implementation::uint_extend_t<uint64_t>(kA.abs()).mul(exA.abs());
            auto tmpB = sklib::implementation::uint_extend_t<uint64_t>(kB.abs()).mul(exB.abs());
            auto diff = sklib::implementation::uint_extend_t<uint64_t>(D.abs());
            if (sklib::bool_xor((kA.sign()>0), (exA.sign()>0))) diff.add(tmpA); else diff.sub(tmpA);
            if (sklib::bool_xor((kB.sign()>0), (exB.sign()>0))) diff.add(tmpB); else diff.sub(tmpB);

            if (D <= 0 || exA % D || exB % D || diff)
            {
                std::cout << "Error, bezout(i65) is incorrect\n";
//                    << "iA=" << iA << "; iB=" << iB << "; D=" << D << "; kA=" << kA << "; kB=" << kB << "\n";
                return 1;
            }
        }

        if (++K, strobe(true)) std::cout << K << "\r" << std::flush;
    }

    //    return 0
}

