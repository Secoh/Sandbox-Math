// prime-get-64.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// This file is candidate for Sklib
// LGPL 2.0 for now.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <random>

#include "SKLib/sklib.hpp"

static constexpr unsigned max_bit_count = sklib::bits_width_v<uint64_t>;
static constexpr unsigned def_count = 16;

SKLIB_DECLARE_CMD_PARAMS(options_t)
{
    SKLIB_PARAM_STRING(pack);   // compressed primes-32 list
    SKLIB_OPTION_STRING(log);   // output text file where primes-64 will be written
//    SKLIB_OPTION_SWITCH(o);     // use -o to overwrite existing file
    SKLIB_OPTION_UINT(bits, max_bit_count);  // width of the primes to be searched
    SKLIB_OPTION_UINT(count, def_count);     // count of output numbers
}
Options;

int main(int argn, char* argc[])
{
    if (!Options(argn, argc))
    {
        std::cout << "Brute-force search of random 64-bit primes using 32-bit primes table.\n"
            "Usage: primes-get-64.exe -pack primes [-log output_file]\n" /* " [-o]\n" */
            "                         [-bits bit_length_of_output] [-count how_many]\n"
            "Default bit length is " << max_bit_count << ", default count is " << def_count << ".\n";
            return 1;
    }

    sklib::bits_file_type Packed(Options.pack, std::ios_base::in);
    if (!Packed.file_stream().is_open())
    {
        std::cout << "Cannot read file " << Options.pack << "\n";
        return -1;
    }

    std::vector<uint32_t> Primes;
    uint32_t CRC;
    auto R = sklib::primes_decode(Packed, Primes, CRC,
                                  [](void*, uint32_t nrec) { std::cout << nrec << "\r" << std::flush; });

    if (!sklib::is_primes_decoder_status_good(R))
    {
        std::cout << sklib::primes_decoder_status_msg(R);
        return -1;
    }

    std::cout << Primes.size() << " primes loaded\n";

    uint64_t gen_min = sklib::bits_data_high_1<uint64_t>(Options.bits);
    uint64_t gen_max = sklib::bits_data_mask<uint64_t>(Options.bits);

    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<uint64_t> distrib(gen_min, gen_max);

    std::ofstream fout;
    if (Options.log.is_present()) fout.open(Options.log);

    for (unsigned k=0; k<Options.count; k++)
    {
        uint64_t idx = sklib::prime_candidate_to_index_ex(distrib(gen));

        while (true)
        {
            auto res = sklib::eratosphenes(idx++, Primes, true);
            if (sklib::is_eratosphenes_status_error(res))
            {
                std::cout << "Eratosphenes error\n";
                return -1;
            }

            if (!sklib::is_eratosphenes_status_prime(res)) continue;

            std::cout << sklib::prime_candidate<uint64_t>(idx) << "\n";
            if (fout.is_open()) fout << sklib::prime_candidate<uint64_t>(idx) << "\n";
            break;
        }
    }

    return 0;
}

