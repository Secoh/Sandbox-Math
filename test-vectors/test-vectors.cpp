// test-vectors.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _USE_MATH_DEFINES
#define SKLIB_TARGET_TEST

#include <iostream>

#include "SKLib/sklib.hpp"
#include "SKLib/include/math.hpp"

int main(int argc, char* argv[])
{
    // need something that is not constant
    double two = (argc > 1 ? std::atof(argv[1]) : 2);

    // 2D

    constexpr auto A2 = sklib::Vect2d(3, 5);
    constexpr auto B2 = sklib::Vect2d(-2, 7);

    constexpr auto X2_01 = A2 + B2;
    constexpr auto X2_02 = A2 - B2;
    constexpr auto X2_03 = A2.X() * +B2;
    constexpr auto X2_04 = B2 * A2.Y();
    constexpr auto X2_05 = B2 / -0.5;
    constexpr auto X2_06 = -A2 * B2;

    constexpr auto X2_07 = A2 + ~B2;
    constexpr auto X2_08 = ~A2 - B2;
    constexpr auto X2_09 = (+~A2).X() * B2;
    constexpr auto X2_10 = ~B2 * A2.Y();

    constexpr auto X2_11 = ~A2 * B2;    // note: element-wise operations
    constexpr auto X2_12 = A2 * ~B2;
    constexpr auto X2_13 = ~A2 * ~B2;

    // 3D

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

    ab3a %= +~ab3b;
    //ab3a %= ~ab3b; // error! element-wise operation is requested, operator% is invalid for doubles

    std::cout << z1b << " " << (b1-ab3a).abs() << "\n";
}


