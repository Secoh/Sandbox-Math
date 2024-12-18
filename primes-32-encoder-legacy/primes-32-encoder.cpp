// primes-32-encoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
// OUTDATED

// This file is candidate for Sklib
// MIT for now.

#include <iostream>
#include <fstream>

#include <sklib/sklib.hpp>

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

constexpr int NRef = 16;
uint8_t Ref[NRef] = {0};

uint8_t Tier1[4] = {0};
uint8_t Tier2[4] = {0};
uint8_t Tier3[8] = {0};

int main(int argn, char *argc[])
{
    Options(argn, argc);

    Ref[3] = 0x10;
    Ref[6] = 0x11;
    Ref[5] = 0x12;
    Ref[1] = 0x20;
    Ref[2] = 0x21;
    Ref[9] = 0x22;
    Ref[4] = 0x30;
    Ref[7] = 0x31;
    Ref[8] = 0x32;
    Ref[10] = 0x33;
    Ref[11] = 0x34;
    Ref[12] = 0x35;
    Ref[15] = 0x36;

    Tier1[0] = 6; // included x2
    Tier1[1] = 12;
    Tier1[2] = 10;
    Tier2[0] = 2;
    Tier2[1] = 4;
    Tier2[2] = 18;
    Tier3[0] = 8;
    Tier3[1] = 14;
    Tier3[2] = 16;
    Tier3[3] = 20;
    Tier3[4] = 22;
    Tier3[5] = 24;
    Tier3[6] = 30;

    const auto ones1 = sklib::bits_pack<1, uint8_t>(1);
    const auto ones2 = sklib::bits_pack<2, uint8_t>(3);
    const auto ones3 = sklib::bits_pack<3, uint8_t>(7);
    const auto zeros1 = sklib::bits_pack<1, uint8_t>(0);

    sklib::timer_stopwatch_type strobe(1000);
    uint32_t nrecords = 0;

    if (Options.e)
    {
        std::ifstream Input(Options.file_in);
        sklib::bits_file_type Packed(Options.file_out.value());

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

            if (!P && P_prev)
            {
                if (!std::getline(Input, str))
                {
                    std::cout << "File error - invalid ending\n";
                    return 1;
                }

                Packed.write(ones2);
                Packed.write(ones2);
                Packed.write(ones3);
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

            if (P_prev ? (P <= P_prev) : (P != 2))
            {
                std::cout << "File error - incorrect input\n";
                return 1;
            }

            Checksum.update_integer_lsb(P);

            nrecords++;
            if (strobe(true)) std::cout << nrecords << "\r" << std::flush;

            if (P > 3)
            {
                auto D2 = (P - P_prev) / 2;

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

