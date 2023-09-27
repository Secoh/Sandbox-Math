// random-test-range.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <SKLib/sklib.hpp>
#include <SKLib/include/math.hpp>

template<class T> void update(T& xmin, T& xmax, T val)
{
    if (xmin > val) xmin = val;
    if (xmax < val) xmax = val;
}

int main()
{
    sklib::timer_stopwatch_type strobe(1000);

    sklib::random_size_integer_device<uint64_t, 64> R64;
    sklib::random_size_integer_device<int64_t, 63> R63;

    sklib::random_size_integer_device<uint32_t, 32> R32;
    sklib::random_size_integer_device<int32_t, 31> R31;

    sklib::random_size_integer_device<uint16_t, 16> R16;
    sklib::random_size_integer_device<int16_t, 15> R15;

    uint64_t maxR64 = 0;
    uint64_t minR64 = 100;
    int64_t maxR63 = 0;
    int64_t minR63 = 0;

    uint32_t maxR32 = 0;
    uint32_t minR32 = 100;
    int32_t maxR31 = 0;
    int32_t minR31 = 0;

    uint16_t maxR16 = 0;
    uint16_t minR16 = 100;
    int16_t maxR15 = 0;
    int16_t minR15 = 0;

    uint64_t K = 0;
    while (true)
    {
        auto A64 = R64();
        auto A32 = R32();
        auto A16 = R16();

        auto A63 = R63();
        auto A31 = R31();
        auto A15 = R15();

        update(minR64, maxR64, A64);
        update(minR63, maxR63, A63);

        update(minR32, maxR32, A32);
        update(minR31, maxR31, A31);

        update(minR16, maxR16, A16);
        update(minR15, maxR15, A15);

        if (++K, strobe(true))
        {
            std::cout << maxR64 << " " << minR64 << " "
                      << maxR63 << " " << minR63 << " "
                      << maxR32 << " " << minR32 << " "
                      << maxR31 << " " << minR31 << " "
                      << maxR16 << " " << minR16 << " "
                      << maxR15 << " " << minR15 << "\r" << std::flush;
        }
    }

    // return 0;
}

