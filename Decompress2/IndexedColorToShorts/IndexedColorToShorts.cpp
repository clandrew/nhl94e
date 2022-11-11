// IndexedColorToShorts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TestData.h"

void IndexedColorToShorts(unsigned char i0, unsigned char i1, unsigned char i2, unsigned char i3, unsigned short* pS0, unsigned short* pS1)
{
    unsigned short short0 = 0;
    unsigned short short1 = 0;

    if (i0 & 0x10) short0 |= 0x0001;
    if (i1 & 0x10) short0 |= 0x0002;
    if (i2 & 0x10) short0 |= 0x0004;
    if (i3 & 0x10) short0 |= 0x0008;

    if (i0 & 0x20) short0 |= 0x0010;
    if (i1 & 0x20) short0 |= 0x0020;
    if (i2 & 0x20) short0 |= 0x0040;
    if (i3 & 0x20) short0 |= 0x0080;

    if (i0 & 0x40) short0 |= 0x0100;
    if (i1 & 0x40) short0 |= 0x0200;
    if (i2 & 0x40) short0 |= 0x0400;
    if (i3 & 0x40) short0 |= 0x0800;

    if (i0 & 0x80) short0 |= 0x1000;
    if (i1 & 0x80) short0 |= 0x2000;
    if (i2 & 0x80) short0 |= 0x4000;
    if (i3 & 0x80) short0 |= 0x8000;

    if (i0 & 0x01) short1 |= 0x0001;
    if (i1 & 0x01) short1 |= 0x0002;
    if (i2 & 0x01) short1 |= 0x0004;
    if (i3 & 0x01) short1 |= 0x0008;

    if (i0 & 0x02) short1 |= 0x0010;
    if (i1 & 0x02) short1 |= 0x0020;
    if (i2 & 0x02) short1 |= 0x0040;
    if (i3 & 0x02) short1 |= 0x0080;

    if (i0 & 0x04) short1 |= 0x0100;
    if (i1 & 0x04) short1 |= 0x0200;
    if (i2 & 0x04) short1 |= 0x0400;
    if (i3 & 0x04) short1 |= 0x0800;

    if (i0 & 0x08) short1 |= 0x1000;
    if (i1 & 0x08) short1 |= 0x2000;
    if (i2 & 0x08) short1 |= 0x4000;
    if (i3 & 0x08) short1 |= 0x8000;

    *pS0 = short0;
    *pS1 = short1;
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