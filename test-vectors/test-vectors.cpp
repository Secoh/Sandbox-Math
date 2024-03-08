// test-vectors.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _USE_MATH_DEFINES
#define SKLIB_TARGET_TEST
//#define SKLIB_MATH_VECTOR_DEBUG_CON

#include <iostream>

#include "SKLib/sklib.hpp"
#include "SKLib/include/math.hpp"

int main(int argc, char* argv[])
{
    double two = (argc > 1 ? std::atof(argv[1]) : 2);

    const sklib::Vect3d abc3 = std::array<double, 3>({ 1, 2, 3 });

    constexpr sklib::Vect3d abc({ 2, 3, 5 });
    const auto a1 = ~abc;

    constexpr auto a2 = abc + abc * 4;

    auto tmp_1 = abc;
    auto alt_a2 = (tmp_1 *= 4) += abc;

    std::cout << alt_a2.Y() << "\n";

    const auto a3 = abc / ~- ~abc;
    const auto a3x = abc * -~abc;
    const auto a4 = ~abc * abc;
    constexpr auto a5 = ~abc - 2 * ~abc + sklib::Vect3d({ 3, 3, 0 });

    const auto x1 = a2 ^ a5;
    const auto y1 = a2.norm();
    constexpr auto z1 = abc.Y();

    std::cout << x1 << " " << z1 << "\n";

    sklib::Vect3d ab3a(two, 3, 5);
    constexpr sklib::Vect3d ab3b(7, 1, -4);
    auto b1 = ab3a % ab3b;
    auto z1a = b1 * ab3a;
    auto z1b = b1 * ab3b;

    std::cout << z1b << "\n";
}


