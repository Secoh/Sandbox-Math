// primes-32-generator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// This file is candidate for Sklib
// LGPL 2.0 for now.

#include <iostream>
#include <vector>
#include <fstream>

#include <sklib/include/timer.hpp>
#include <sklib/include/cmdpar.hpp>
#include <sklib/include/checksum.hpp>
#include <sklib/include/bitwise.hpp>

static constexpr uint32_t SearchBound = sklib::supplement::bits_data_mask<uint32_t>();
static constexpr uint32_t UpdateBound = (uint32_t)(SearchBound / 2);

SKLIB_DECLARE_CMD_PARAMS(options_t)
{
    SKLIB_OPTION_SWITCH(z);
    SKLIB_OPTION_STRING(file);
    SKLIB_OPTION_STRING(gap);
    SKLIB_OPTION_UINT(max);
}
Options;

sklib::crc_32_iso Checksum;

// TODO: need reversal
uint64_t provide_6n1(uint64_t idx)
{
    return 3*idx + 4 + (~idx & 1);
}

static constexpr int erth_false      = 0x00;
static constexpr int erth_true       = 0x01;
static constexpr int erth_error_mask = 0x02;

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

// counting starts from 3-5; index 0 is gap 2, 1->4, 2->6, etc
uint32_t gap_hist[sklib::OCTET_ADDRESS_SPAN] = {0};

int main(int argn, char *argc[])
{
    Options(argn, argc);

    uint32_t MaxSearch = (Options.max.is_present() ? Options.max : SearchBound);

    std::ofstream ofile;
    std::ostream& fout = (Options.file.is_present() ? (ofile.open(Options.file), ofile) : std::cout);

    std::vector<uint32_t> Record;
    Record.push_back(2);
    Record.push_back(3);
    fout << "2\n3\n";

    sklib::timer_stopwatch_type strobe(1000);
    uint32_t nrecords = 0;
    uint32_t hist_large = 0;
    uint32_t prev_prime = 3;

    uint32_t idx = 0;
    for (;;idx++)
    {
        auto value64 = provide_6n1(idx);
        if (value64 > MaxSearch) break;

        auto value = (uint32_t)value64;
        auto result = eratosphenes(value, Record);

        if (result & erth_error_mask)
        {
            std::cout << "Internal error\n";
            return -1;
        }

        if (result & erth_true)
        {
            nrecords++;

            int hidx = (value - prev_prime)/2 - 1;
            if (hidx < sklib::OCTET_ADDRESS_SPAN) gap_hist[hidx]++; else hist_large++;
            prev_prime = value;

            // ensure that the very last element is greater than 2^{N/2}
            // so the last division in Eratosphenes returns dividend less than 2^{N/2}
            if (Record.back() <= UpdateBound) Record.push_back(value);

            fout << value << "\n";
            if (strobe(true)) std::cout << nrecords << "\r" << std::flush;

            Checksum.update_integer_lsb(value);
        }
    }

    fout << "0\n" << Checksum.get() << "\n";

    if (Options.gap.is_present())
    {
        std::ofstream gout(Options.gap);
        for (int i=0; i<sklib::OCTET_ADDRESS_SPAN; i++)
        {
            gout << 2*(i+1) << "," << gap_hist[i] << "\n";
        }
        gout << 2*(sklib::OCTET_ADDRESS_SPAN+1) << "," << hist_large << "\n";
    }

    std::cout << nrecords << "\nDone!\n";
}

