// bezu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdio>

int ddiv(int a, int b, int& r)
{
    r = a % b;
    return a / b;
}

int bezu(int a, int b, int& va, int& vb)
{
    va=1; vb=0;
    int ua=0, ub=1, k=1;
    int *r[2] = { &a, &b };
    int *s[2] = { &va, &ua };
    int *t[2] = { &vb, &ub };

    while (true)
    {
        int q = ddiv(*r[1-k], *r[k], *r[1-k]);
        if (!*r[1-k]) break;

        *s[1-k] -= q * *s[k];
        *t[1-k] -= q * *t[k];
        k = 1-k;
    }

    if (k) { va=ua; vb=ub;}
    return *r[k];
}

int main()
{
    std::cout << "BEZU\n";

    int A=0, B=0, D=0, U=0, V=0;
    int K=0, rA=0, rB=0;

/*
    while (true)
    {
        std::cout << "A>";
        std::cin >> A;
        std::cout << "B>";
        std::cin >> B;
        D = bezu(A, B, U, V);
        std::cout << "D=" << D << ", vA=" << U << ", vB=" << V << ", vfy=" << A*U+B*V << "\n";
    }
*/

    srand((unsigned)time(nullptr));

    while (true)
    {
        A = (rand() & 0xFFF) + ((rand() & 0xFFF) << 12);
        B = (rand() & 0xFFF) + ((rand() & 0xFFF) << 12);

        if (!A || !B) continue;

        D = bezu(A, B, U, V);

        ddiv(A, D, rA);
        ddiv(B, D, rB);
        if (rA || rB || A*U+B*V != D)
        {
            std::cout << "Bad result\n"
                << "D=" << D << ", rA=" << rA << ", rB=" << rB << ", vA=" << U << ", vB=" << V << ", vfy=" << A*U+B*V << "\n";

            return 1;
        }

        if (++K % 10000 == 0) std::cout << K << "\r" << std::flush;
    }

//    return 0
}

