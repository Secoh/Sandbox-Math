// primes-32-encoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <fstream>

#include <sklib/include/timer.hpp>
#include <sklib/include/cmdpar.hpp>
#include <sklib/include/checksum.hpp>
#include <sklib/include/bitwise.hpp>
#include <sklib/include/math.hpp>

SKLIB_DECLARE_CMD_PARAMS(options_t)
{
    SKLIB_PLAIN_STRING(file_in);
    SKLIB_PLAIN_STRING(file_out);
    SKLIB_OPTION_SWITCH(e);
    SKLIB_OPTION_STRING(d);
    SKLIB_OPTION_STRING(t);
    SKLIB_OPTION_STRING(w);
}
Options;

sklib::crc_32_iso Checksum;

// 1,2,3 - 00, 01, 10
// 11: 4-10 - 000-110
// 11,111: 11-25 - 0000-1110
// 11,111,1111,0 + 8
// 11,111,1111,1 + 32

static constexpr int base1 = 1;
static constexpr int base2 = 4;
static constexpr int base3 = 11;
static constexpr int base4 = 26;

int main(int argn, char *argc[])
{
    Options(argn, argc);

    const auto ones1 = sklib::bits_pack<1, uint8_t>(1);
    const auto ones2 = sklib::bits_pack<2, uint8_t>(3);
    const auto ones3 = sklib::bits_pack<3, uint8_t>(7);
    const auto ones4 = sklib::bits_pack<4, uint8_t>(0xF);
    const auto zeros1 = sklib::bits_pack<1, uint8_t>(0);

    sklib::timer_stopwatch_type strobe(1000);
    uint32_t nrecords = 0;

    if (Options.e)
    {
        std::ifstream Input(Options.file_in);
        sklib::bits_file_type Packed(Options.file_out.value());

        uint32_t Idx_prev = 0;
        uint32_t P_prev = 0;
        std::string str;
        while (true)
        {
            if (!std::getline(Input, str))
            {
                std::cout << "File error - premature ending\n";
                return 1;
            }

            auto P = sklib::stoi<uint32_t>(str.c_str());

            if (!P && Idx_prev)
            {
                if (!std::getline(Input, str))
                {
                    std::cout << "File error - invalid ending\n";
                    return 1;
                }

                Packed.write(ones2);
                Packed.write(ones3);
                Packed.write(ones4);
                Packed.write(ones1);
                Packed.write(sklib::bits_pack<32, uint32_t>(0));

                Packed.write(sklib::bits_pack<32, uint32_t>(Checksum.get()));
                Packed.write_flush();

                if (sklib::stoi<uint32_t>(str.c_str()) != Checksum.get())
                {
                    std::cout << "Warning: input/control CRC mismatch\n";
                }

                std::cout << nrecords << "\nDone!\n";
                return 0;
            }

            Checksum.update_integer_lsb(P);
            if (P <= 5) continue;

            uint32_t Idx = sklib::prime_candidate_to_index(P);

            if (Idx_prev >= 0 ? (Idx <= Idx_prev) : (P != 2))
            {
                std::cout << "File error - incorrect input\n";
                return 1;
            }


            nrecords++;
            if (strobe(true)) std::cout << nrecords << "\r" << std::flush;

            if (P > 3)
            {
                auto d2 = Idx - Idx_prev;

                int r = ((D2 < NRef) ? Ref[D2] : 0);
                if (r)
                {
                    int tier = r & 0xF0;
                    uint8_t data = r & 0x0F;

                    if (tier == 0x10)
                    {
                        Packed.write(sklib::bits_pack<2, uint8_t>(data));
                    }
                    else if (tier == 0x20)
                    {
                        Packed.write(ones2);
                        Packed.write(sklib::bits_pack<2, uint8_t>(data));
                    }
                    else // if (tier == 0x30)
                    {
                        Packed.write(ones2);
                        Packed.write(ones2);
                        Packed.write(sklib::bits_pack<3, uint8_t>(data));
                    }
                }
                else if (D2 <= sklib::OCTET_MASK)
                {
                    Packed.write(ones2);
                    Packed.write(ones2);
                    Packed.write(ones3);
                    Packed.write(zeros1);
                    Packed.write(sklib::bits_pack<8, uint8_t>((uint8_t)D2));
                }
                else // 32 bit
                {
                    Packed.write(ones2);
                    Packed.write(ones2);
                    Packed.write(ones3);
                    Packed.write(ones1);
                    Packed.write(sklib::bits_pack<32, uint32_t>(P));
                }
            }

            P_prev = P;
        }
    }
    else if (Options.d || Options.t)
    {
        sklib::bits_file_type Packed(Options.file_in.value());
        std::ofstream Output(Options.file_out);

        if (Options.d) Output << "2\n3\n";

        Checksum.update_integer_lsb<uint32_t>(2);
        Checksum.update_integer_lsb<uint32_t>(3);

        uint32_t P = 3;

        auto R1 = sklib::bits_pack<1, uint8_t>(0);
        auto R2 = sklib::bits_pack<2, uint8_t>(0);
        auto R3 = sklib::bits_pack<3, uint8_t>(0);
        auto R8 = sklib::bits_pack<8, uint8_t>(0);
        auto R32 = sklib::bits_pack<32, uint32_t>(0);

        while (true)
        {
            if (!Packed.can_read(2))
            {
                std::cout << "File error - unexpected EOF (broken archive)\n";
                return 1;
            }

            Packed.read(R2);
            int X = Tier1[R2.data & 0x03];
            if (X)
            {
                P += X;
            }
            else
            {
                Packed.read(R2);
                X = Tier2[R2.data & 0x03];
                if (X)
                {
                    P += X;
                }
                else
                {
                    Packed.read(R3);
                    X = Tier3[R3.data & 0x07];
                    if (X)
                    {
                        P += X;
                    }
                    else
                    {
                        Packed.read(R1);
                        if (R1.data & 0x01)
                        {
                            Packed.read(R32);
                            P = R32.data;
                        }
                        else
                        {
                            Packed.read(R8);
                            P += 2 * (uint32_t)(R8.data);
                        }
                    }
                }
            }

            if (P)
            {
                Checksum.update_integer_lsb(P);

                if (Options.d) Output << P << "\n";

                nrecords++;
                if (strobe(true)) std::cout << nrecords << "\r" << std::flush;
            }
            else
            {
                if (!Packed.can_read(32))
                {
                    std::cout << "File error - unexpected EOF (missing CRC)\n";
                    return 1;
                }

                Packed.read(R32);

                if (Options.d) Output << 0 << "\n" << R32.data << "\n";

                if (Checksum.get() != R32.data)
                {
                    std::cout << "Warning: input/control CRC mismatch\n";
                }

                break;
            }
        }

        std::cout << nrecords << "\nDone!\n";
        return 0;
    }
}

