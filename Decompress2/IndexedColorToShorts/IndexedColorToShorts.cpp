// IndexedColorToShorts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

struct TestCase
{
    unsigned char IndexedColor[4];
    unsigned short Shorts[2];
} testCases[] =
{
    {0, 0, 0, 0, 0, 0}
};

void IndexedColorToShorts(unsigned char i0, unsigned char i1, unsigned char i2, unsigned char i3, unsigned short* pS0, unsigned short* pS1)
{
    *pS0 = 0;
    *pS1 = 0;
}

int main()
{
    for (int i = 0; i < _countof(testCases); ++i)
    {
        TestCase const& testCase = testCases[i];

        unsigned short shorts[2];
        IndexedColorToShorts(
            testCase.IndexedColor[0],
            testCase.IndexedColor[1],
            testCase.IndexedColor[2],
            testCase.IndexedColor[3],
            &shorts[0],
            &shorts[1]);

        if (shorts[0] != testCase.Shorts[0])
        {
            __debugbreak();
        }

        if (shorts[1] != testCase.Shorts[1])
        {
            __debugbreak();
        }
    }
}