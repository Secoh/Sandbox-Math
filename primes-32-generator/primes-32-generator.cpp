// primes-32-generator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// This file is candidate for Sklib
// LGPL 2.0 for now.

#include <iostream>
#include <vector>
#include <fstream>

#include "SKLib/sklib.hpp"

//#include <sklib/include/timer.hpp>
//#include <sklib/include/cmdpar.hpp>
//#include <sklib/include/checksum.hpp>
//#include <sklib/include/bitwise.hpp>
//#include <sklib/include/math.hpp>

static constexpr uint32_t SearchBound = sklib::bits_mask_v<uint32_t>;
static constexpr uint32_t UpdateBound = sklib::bits_mask_v<uint16_t>;

SKLIB_DECLARE_CMD_PARAMS(options_t)
{
    SKLIB_PARAM_STRING(file);
    SKLIB_OPTION_STRING(gap);
    SKLIB_OPTION_UINT(max);
    SKLIB_OPTION_UINT(debug);  // Eratosphenes test: turn on margin, and, 0 - reference list is empty, N - has N entries
}
Options;

sklib::crc_32_iso Checksum;

// TODO: need reversal
//sk uint64_t provide_6n1(uint64_t idx)
//{
//    return 3*idx + 5 - (idx & 1);
//}

/* // lets go sklib!
static constexpr int erth_false      = 0x00;
static constexpr int erth_true       = 0x01;
static constexpr int erth_error_mask = 0x02;

// input is odd numbers starting with 5
// record is 2,3,5,7,11,etc
int eratosphenes(uint32_t value, std::vector<uint32_t>& record)
{
    uint32_t N = (uint32_t)record.size();
    uint32_t k = 2;
    if (k==N) return erth_true; // special precaution the initial state
    for (; k<N; k++)
    {
        auto prime = record[k];
        if (value % prime == 0) return erth_false;
        if (value / prime < prime) return erth_true;
    }
    return (erth_false | erth_error_mask);
}
*/

// counting starts from 3-5; index 0 is gap 2, 1->4, 2->6, etc
uint32_t gap_hist[sklib::OCTET_ADDRESS_SPAN] = {0};
uint32_t igap_hist[sklib::OCTET_ADDRESS_SPAN] = {0};

int main(int argn, char *argc[])
{
    if (!Options(argn, argc))
    {
        std::cout << "Creates list of all 32-bit primes and their 6n1 indexes (Eratosphenes).\n"
                     "Usage: primes-32-generator.exe -file output_file [-gap csv_file_gaps_histogram]\n"
                     "                            [-max alt_maximum (full 32-bit integer is default)]\n"
                     "                            [-debug [N]]\n"
                     "If -debug is set, Eratosphenes test is performed, the input Primes list has\n"
                     "maximum N entries, can be 0 for empty (default). 2, 3 are counted as entries.\n";
        return 1;
    }

    uint32_t MaxSearch = (Options.max.is_present() ? Options.max : SearchBound);

    std::ofstream fout(Options.file);

    std::vector<uint32_t> Record;
    if (!Options.debug || Options.debug.value() > 0) Record.push_back(2);
    if (!Options.debug || Options.debug.value() > 1) Record.push_back(3);
    Checksum.update_integer_lsb<uint32_t>(2);
    Checksum.update_integer_lsb<uint32_t>(3);
    fout << "2,0\n3,0\n";

    sklib::timer_stopwatch_type strobe(1000);
    uint32_t nrecords = 0;
    uint32_t hist_large = 0;
    uint32_t prev_prime = 3;
    uint32_t prev_idx = 0;

    uint32_t idx = 0;
    for (;;idx++)
    {
        auto value64 = sklib::prime_candidate<uint64_t>(idx);
        if (value64 > MaxSearch) break;

        // the test subject will never be larger than 32-bit integer
        auto value = (uint32_t)value64;
        auto result = sklib::eratosphenes(idx, Record, Options.debug.is_present());

        if (sklib::is_eratosphenes_status_error(result))   //result & erth_error_mask)
        {
            std::cout << "Eratosphenes error\n";
            return -1;
        }

        if (sklib::is_eratosphenes_status_prime(result))    //result & erth_true)
        {
            nrecords++;

            if (idx != sklib::prime_candidate_to_index(value))
            {
                std::cout << "Indexing error\n";
                return -1;
            }

            int hidx = (value - prev_prime)/2 - 1;
            if (hidx < sklib::OCTET_ADDRESS_SPAN) gap_hist[hidx]++; else hist_large++;
            prev_prime = value;

            hidx = idx - prev_idx - 1;
            prev_idx = idx;
            if (hidx >= 0 && hidx < sklib::OCTET_ADDRESS_SPAN) igap_hist[hidx]++;

            auto debug_limitations = (!Options.debug.is_present() || Options.debug.value() >= nrecords + 2);

            // ensure that the very last element is greater than 2^{N/2}
            // so the last division in Eratosphenes returns dividend less than 2^{N/2}
            if (Record.back() <= UpdateBound && debug_limitations) Record.push_back(value);

            fout << value << "," << idx << "\n";
            if (strobe(true)) std::cout << nrecords << " (" << Record.size() << ")\r" << std::flush;

            Checksum.update_integer_lsb(value);
        }
    }

    fout << "0\n" << Checksum.get() << "\n";

    if (Options.gap.is_present())
    {
        std::ofstream gout(Options.gap);
        for (int i=0; i<sklib::OCTET_ADDRESS_SPAN; i++)
        {
            gout << 2*(i+1) << "," << gap_hist[i] << "," << i << "," << igap_hist[i] << "\n";
        }
        gout << 2*(sklib::OCTET_ADDRESS_SPAN+1) << "," << hist_large << "\n";
    }

    std::cout << nrecords << "\nDone!\n";
}

