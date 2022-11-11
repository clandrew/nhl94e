// IndexedColorToShorts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TestData.h"

void IndexedColorToShorts(unsigned char i0, unsigned char i1, unsigned char i2, unsigned char i3, unsigned short* pS0, unsigned short* pS1)
{
    unsigned short short0 = 0;
    unsigned short short1 = 0;
    
    {
        bool b0 = i0 & 0x10;
        bool b1 = i1 & 0x10;
        bool b2 = i2 & 0x10;
        bool b3 = i3 & 0x10;

        if (b0)
        {
            short0 |= 1;
        }

        if (b1)
        {
            short0 |= 2;
        }

        if (b2)
        {
            short0 |= 4;
        }

        if (b3)
        {
            short0 |= 8;
        }
    }
    {
        bool b0 = i0 & 0x20;
        bool b1 = i1 & 0x20;
        bool b2 = i2 & 0x20;
        bool b3 = i3 & 0x20;

        if (b0)
        {
            short0 |= 0x10;
        }

        if (b1)
        {
            short0 |= 0x20;
        }

        if (b2)
        {
            short0 |= 0x40;
        }

        if (b3)
        {
            short0 |= 0x80;
        }
    }
    {
        bool b0 = i0 & 0x40;
        bool b1 = i1 & 0x40;
        bool b2 = i2 & 0x40;
        bool b3 = i3 & 0x40;

        if (b0)
        {
            short0 |= 0x100;
        }

        if (b1)
        {
            short0 |= 0x200;
        }

        if (b2)
        {
            short0 |= 0x400;
        }

        if (b3)
        {
            short0 |= 0x800;
        }
    }
    {
        bool b0 = i0 & 0x80;
        bool b1 = i1 & 0x80;
        bool b2 = i2 & 0x80;
        bool b3 = i3 & 0x80;

        if (b0)
        {
            short0 |= 0x1000;
        }

        if (b1)
        {
            short0 |= 0x2000;
        }

        if (b2)
        {
            short0 |= 0x4000;
        }

        if (b3)
        {
            short0 |= 0x8000;
        }
    }
    {
        bool b0 = i0 & 0x1;
        bool b1 = i1 & 0x1;
        bool b2 = i2 & 0x1;
        bool b3 = i3 & 0x1;

        if (b0)
        {
            short1 |= 0x0001;
        }

        if (b1)
        {
            short1 |= 0x0002;
        }

        if (b2)
        {
            short1 |= 0x0004;
        }

        if (b3)
        {
            short1 |= 0x0008;
        }
    }
    {
        bool b0 = i0 & 0x2;
        bool b1 = i1 & 0x2;
        bool b2 = i2 & 0x2;
        bool b3 = i3 & 0x2;

        if (b0)
        {
            short1 |= 0x0010;
        }

        if (b1)
        {
            short1 |= 0x0020;
        }

        if (b2)
        {
            short1 |= 0x0040;
        }

        if (b3)
        {
            short1 |= 0x0080;
        }
    }
    {
        bool b0 = i0 & 0x4;
        bool b1 = i1 & 0x4;
        bool b2 = i2 & 0x4;
        bool b3 = i3 & 0x4;

        if (b0)
        {
            short1 |= 0x0100;
        }

        if (b1)
        {
            short1 |= 0x0200;
        }

        if (b2)
        {
            short1 |= 0x0400;
        }

        if (b3)
        {
            short1 |= 0x0800;
        }
    }
    {
        bool b0 = i0 & 0x8;
        bool b1 = i1 & 0x8;
        bool b2 = i2 & 0x8;
        bool b3 = i3 & 0x8;

        if (b0)
        {
            short1 |= 0x1000;
        }

        if (b1)
        {
            short1 |= 0x2000;
        }

        if (b2)
        {
            short1 |= 0x4000;
        }

        if (b3)
        {
            short1 |= 0x8000;
        }
    }

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