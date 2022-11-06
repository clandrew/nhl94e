#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "Util.h"
#include "Decompress2.h"

namespace Fast
{
    void Fn_80C1B0();
    bool Fn_80C232();
    void Fn_80C2DC();

    unsigned short a = 0xFB30;
    unsigned short x = 0x0480;
    unsigned short y = 0xF8AE;
    bool n = false;
    bool z = false;
    bool c = false;
    unsigned char dbr = 0x9A;
    unsigned short currentProfileImageIndex = 0;
    std::string outputCpuLogFileName;
    std::string outputIndexedColorFileName;
    std::string goldIndexedColorFileName;

    Mem16 mem00{};
    unsigned short mem04 = 0;
    unsigned short mem06 = 0;
    unsigned short mem08 = 0;
    unsigned short mem0c = 0xF8AC;
    unsigned short mem0e = 0x7f;
    unsigned short mem10 = 0;
    unsigned short mem12 = 0x007F;
    unsigned short mem14 = 0;
    unsigned short mem16 = 0;
    unsigned short mem6a = 0;
    Mem16 LoadMem6b();
    void SaveMem6b(Mem16 const& v);
    unsigned short mem6c = 0;
    unsigned short mem6f = 0;
    unsigned short mem71 = 0;
    unsigned short mem73 = 0;
    unsigned short mem75 = 0;
    unsigned short mem77 = 0;
    unsigned short mem79 = 0;
    unsigned short mem7b = 0;
    unsigned short mem7d = 0;
    unsigned short mem91_HomeOrAway = 0;
    unsigned short mem0760 = 0;

    // Staging output.
    std::vector<unsigned char> mem7E0500_7E0700;

    unsigned short indirectHigh;
    unsigned short indirectLow;

    std::vector<unsigned char> cache7E0100;
    std::vector<unsigned char> cache7E0700;
    std::vector<unsigned char> cache7E0720;
    std::vector<unsigned char> cache7E0740;

    // Final output
    std::vector<unsigned char> cache7F0000;

    // Gold reference output
    std::vector<unsigned char> goldReferenceIndexedColor;

    // Loaded plainly
    std::vector<unsigned char> romFile;

    unsigned short loaded = 0;
    Mem16 loaded16{};
    unsigned char low = 0;
    bool willCarry = false;
    bool willSetNegative = false;

    Mem16 LoadMem6b()
    {
        Mem16 mem6b;
        mem6b.Low8 = mem6a >> 8;
        mem6b.High8 = mem6c & 0xFF;
        return mem6b;
    }

    void SaveMem6b(Mem16 const& v)
    {
        mem6a &= 0xFF;
        mem6a |= (v.Low8 << 8);
        mem6c &= 0xFF00;
        mem6c |= v.High8;
    }

    Mem16 Load16FromAddress(unsigned short bank, unsigned short offset)
    {
        Mem16 result{};

        if (offset < 0x8000 && bank == 0x96)
        {
            bank = 0x7E; // Shadowing
        }

        if (bank >= 0x80)
        {
            // ROM
            int local = offset - 0x8000;
            int bankMultiplier = bank - 0x80;

            int fileOffset = 0x8000 * bankMultiplier;
            result.Low8 = romFile[fileOffset + local];
            result.High8 = romFile[fileOffset + local + 1];
        }
        else if (bank == 0x7E)
        {
            if (offset == 0)
            {
                result.Data16 = mem00.Data16;
            }
            else
            {
                __debugbreak();
            }
        }
        else
        {
            __debugbreak();
        }

        return result;
    }

    void LoadNextFrom0CInc()
    {
        // This runs in 8 bit mode.
        loaded16 = Load16FromAddress(dbr, mem0c);
        a &= 0xFF00;
        a |= loaded16.Low8;
        mem0c++;
    }

    void LoadNextFrom0500()
    {
        // 16bit A, 8bit index

        x = mem7E0500_7E0700[y];

        loaded16.Data16 = x;
        if (indirectHigh == 0x7E && indirectLow >= 0x100)
        {
            cache7E0100[indirectLow - 0x100] = loaded16.Low8;
        }
        else if (indirectHigh == 0x7F)
        {
            cache7F0000[indirectLow] = loaded16.Low8;
        }
        else
        {
            __debugbreak();
        }
        indirectLow++;
        mem08 = x;
    }

    void LoadNextFrom0600()
    {
        mem6c = a;
        y = mem6c >> 8;
        x = mem7E0500_7E0700[0x100 + y];
    }

    void LoadNextFrom0CMaskAndShift(unsigned char nextX, int shifts)
    {
        x = nextX;

        mem6a = 0;

        loaded16 = Load16FromAddress(dbr, mem0c);
        a = loaded16.Data16;

        a &= 0xFF;

        for (int i = 0; i < shifts; ++i)
        {

            a *= 2;
        }

        loaded16 = LoadMem6b();
        a |= loaded16.Data16;

        loaded16.Data16 = a;
        SaveMem6b(loaded16);

        a = mem6c;
    }

    void ShiftThenLoad100ThenCompare(int shifts, int subtractDataAddress, int nextY)
    {
        for (int i = 0; i < shifts; ++i)
        {
            // $80/BF8F 4A          LSR A                   A:F192 X:0006 Y:00F1 P:envmxdizc

            a >>= 1;
        }

        bool shadowBank7E = false;
        if (subtractDataAddress < 0x8000)
        {
            if (dbr >= 0x80 && dbr <= 0xBF)
            {
                shadowBank7E = true;
            }
        }

        bool resolvedAddress = false;
        if (shadowBank7E)
        {
            if (subtractDataAddress >= 0x720)
            {
                int local = subtractDataAddress - 0x720;
                if (local >= (int)cache7E0720.size())
                {
                    __debugbreak(); // notimpl
                }

                loaded16.Low8 = cache7E0720[local];
                loaded16.High8 = cache7E0720[local + 1];
                resolvedAddress = true;
            }
        }

        if (!resolvedAddress)
        {
            __debugbreak(); // notimpl
        }

        a -= loaded16.Data16;

        y = a;

        a = cache7E0100[y];

        y = nextY;

        // $80/BFA2 C5 73       CMP $73    [$00:0073]   A:0008 X:0006 Y:0001 P:envmxdizc

        z = a == (mem73 & 0xFF); // we are in 8bit mode
    }

    void ShiftRotateDecrement(int xDecAmt, int yDecAmt)
    {
        c = a >= 0x8000;
        a *= 2;

        RotateLeft(&mem6f, &c);

        for (int i = 0; i < xDecAmt; ++i)
        {
            --x;
            z = x == 0;
        }

        for (int i = 0; i < yDecAmt; ++i)
        {
            --y;
            z = y == 0;
        }
    }

    void Monstrosity0()
    {
        // Use 8bit X and Y

        x &= 0xFF;
        y &= 0xFF;
        mem0c += 5;

        loaded16 = Load16FromAddress(dbr, mem0c);
        mem73 = loaded16.Data16;
        mem0c++;

        loaded16 = Load16FromAddress(dbr, mem0c);
        a = loaded16.Data16;
        mem0c += 2;

        a = ExchangeShortHighAndLow(a);
        mem6c = a;

        y = 8;
        mem77 = 0;
        mem75 = 0;
        mem14 = 0x10;
        a = 0x10;
        x = 0xFE;

    label_MonstrosityStart:
        x = IncLow8(x);
        x = IncLow8(x);
        mem14--;

        mem75 *= 2;
        a = mem75 - mem77;

        loaded16.Data16 = a;
        cache7E0720[x] = loaded16.Low8;
        cache7E0720[x + 1] = loaded16.High8;

        Fn_80C1B0();

        // 8bit index
        loaded16.Data16 = a;
        cache7E0700[x] = loaded16.Low8;

        a += mem77;
        mem77 = a;
        mem75 += mem6f;
        if (mem6f == 0)
        {
            cache7E0740[x] = 0;
            cache7E0740[x + 1] = 0;
            goto label_MonstrosityStart;
        }

        mem00.Data16 = mem75;

        for (int i = 0; i < mem14; ++i)
        {
            c = mem00.Data16 >= 0x8000;
            mem00.Data16 *= 2;
        }

        cache7E0740[x] = mem00.Low8;
        cache7E0740[x + 1] = mem00.High8;

        if (!c)
        {
            goto label_MonstrosityStart;
        }


        mem79 = x >> 1;

        for (int i = 0; i < 0x40; i += 2)
        {
            mem7E0500_7E0700[i] = 0;
            mem7E0500_7E0700[i + 1] = 0;

            mem7E0500_7E0700[0x40 + i] = 0;
            mem7E0500_7E0700[0x40 + i + 1] = 0;

            mem7E0500_7E0700[0x80 + i] = 0;
            mem7E0500_7E0700[0x80 + i + 1] = 0;

            mem7E0500_7E0700[0xC0 + i] = 0;
            mem7E0500_7E0700[0xC0 + i + 1] = 0;

        }

        indirectHigh = 0x7E;
        indirectLow = 0x100;

        x = 0xFF;

        while (mem77 != 0)
        {
            Fn_80C1B0();
            ++a;
            while (a != 0)
            {
                ++x;
                x &= 0xFF;
                if (mem7E0500_7E0700[x] < 0x80)
                {
                    --a;
                }
            }

            mem7E0500_7E0700[x]--;

            // This is running in 8 bit index mode.
            loaded16.Data16 = x;
            cache7E0100[indirectLow - 0x100] = loaded16.Low8;
            indirectLow += 1;

            mem77--;
        }
        mem71 = y * 2;
        x = 0;
        y = 0;
        mem7b = 0;


        mem00.Data16 = x;
        while (mem7b * 2 != 0x10)
        {
            mem77 = cache7E0700[mem7b * 2];
            mem7d = romFile[0x3C7B + mem7b];

            x = mem00.Data16;

            for (int i = 0; i < mem77; ++i)
            {
                mem00.Data16 = mem7b * 2;

                // This is running in 8 bit accumulator and index mode.
                loaded16.Data16 = mem00.Data16;
                loaded16.Low8 = cache7E0100[y];
                a = loaded16.Data16;

                mem00.High8 = cache7E0100[y];

                ++y;

                if (a == (mem73 & 0xFF))
                {
                    a = mem7b + 1;

                    mem73 &= 0x00FF; // Keep the first, lower byte
                    mem73 |= (a << 8); // Replace the upper byte, second byte

                    mem00.Data16 = 0x12;
                }

                mem04 = y;
                for (int j = 0; j < mem7d - 1; ++j)
                {
                    mem7E0500_7E0700[x] = mem00.High8;
                    mem7E0500_7E0700[0x100 + x] = mem00.Low8;
                    ++x;
                }
            }

            mem7b++;
            mem00.Data16 = x;
        }

        x = mem00.Low8;
        a = 0x10;

        while (x != 0)
        {
            // 8bit acc
            loaded16.Data16 = a;
            mem7E0500_7E0700[0x100 + x] = loaded16.Low8;

            ++x;
            x &= 0x00FF;
        }

        x = mem79 * 2;

        if (x == 0x10)
        {
            mem0760 = 0xBFC5;
        }
        else if (x == 0x12)
        {
            mem0760 = 0xBFC8;
        }
        else
        {
            __debugbreak(); // notimpl
        }

        indirectHigh = mem12;
        indirectLow = mem10;
    }

    struct CaseTableRow
    {
        int NextCaseIndices[9];
        int FirstMultipliers[9];
        int SecondMultipliers[9];
    };

    CaseTableRow s_caseTable[] =
        { 
            /*              NextCaseIndex               FirstMultiplier                      SecondMultiplier           */
            /* Case 0 */ { {8, 7, 6, 5, 4, 3, 2, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0},        {0, 0, 0, 0, 0, 0, 0, 0, 0},  },
            /* Case 1 */ { {3, 4, 5, 6, 7, 8, 1, 0, 0}, {4, 8, 16, 32, 64, 128, 256, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0},  },
            /* Case 2 */ { {4, 5, 6, 7, 8, 1, 2, 0, 0}, {4, 8, 16, 32, 64, 128, 128, 0, 0}, {0, 0, 0, 0, 0, 1, 2, 0, 0},  },
            /* Case 3 */ { {5, 6, 7, 8, 1, 2, 3, 0, 0}, {4, 8, 16, 32, 64, 64, 64, 0, 0},   {0, 0, 0, 0, 1, 2, 4, 0, 0},  },
            /* Case 4 */ { {6, 7, 8, 1, 2, 3, 4, 0, 0}, {4, 8, 16, 32, 32, 32, 32, 0, 0},   {0, 0, 0, 1, 2, 4, 8, 0, 0},  },
            /* Case 5 */ { {7, 8, 1, 2, 3, 4, 5, 0, 0}, {4, 8, 16, 16, 16, 16, 16, 0, 0},   {0, 0, 1, 2, 4, 8, 16, 0, 0},  },
            /* Case 6 */ { {8, 1, 2, 3, 4, 5, 6, 0, 0}, {4, 8, 8, 8, 8, 8, 8, 0, 0},        {0, 1, 2, 4, 8, 16, 32, 0, 0},  },
            /* Case 7 */ { {1, 2, 3, 4, 5, 6, 7, 0, 0}, {4, 4, 4, 4, 4, 4, 4, 0, 0},        {1, 2, 4, 8, 16, 32, 64, 0, 0},  },
            /* Case 8 */ { {2, 3, 4, 5, 6, 7, 8, 0, 0}, {2, 2, 2, 2, 2, 2, 2, 0, 0},        {2, 4, 8, 16, 32, 64, 128, 0, 0},  },
        };

    struct CaseTable8Entry
    {
        int Cond;
        int Lower;
        int IterCount;
    } caseTable8Entries[] = {
        {2, 0, 2},      // x==2   
        {4, 7, 4},      // x==4   
        {6, 6, 4},      // x==6    
        {8, 5, 5},      // x==8    
        {10, 4, 7},     // x==10    
        {12, 3, 7},     // x==12   
        {14, 2, 8},     // x==14   
        {16, 1, 9},     // x==16    
    };

    void Monstrosity1()
    {
        bool continueDecompression = true;
        unsigned char decompressedValue = 0;
        bool shiftHigh = false;
        unsigned short currentCaseIndex = 0;
        unsigned short nextCaseCond = 0;
        unsigned short nextCaseIndex = 0;
        unsigned short mainIndex = 0;
        unsigned short exitValue = 0;

        a = mem6c;
        x = mem71;
        nextCaseCond = mem71;
        LoadNextFrom0600();
        nextCaseIndex = s_caseTable[0].NextCaseIndices[nextCaseCond / 2 - 1];

        while (1)
        {
            currentCaseIndex = nextCaseIndex;
            nextCaseCond = x;
            nextCaseIndex = s_caseTable[currentCaseIndex].NextCaseIndices[nextCaseCond / 2 - 1];
            exitValue = 0x12 - (currentCaseIndex * 2);

            if (nextCaseCond < 0x10)
            {
                unsigned short firstMultiplier = s_caseTable[currentCaseIndex].FirstMultipliers[nextCaseCond / 2 - 1];
                unsigned short secondMultiplier = s_caseTable[currentCaseIndex].SecondMultipliers[nextCaseCond / 2 - 1];

                a *= firstMultiplier;
                if (secondMultiplier != 0)
                {
                    LoadNextFrom0CInc();
                    a *= secondMultiplier;
                }
                LoadNextFrom0500();
                LoadNextFrom0600();
                continue;
            }

            if (nextCaseCond == 0x10)
            {
                // The jump760 case with what was formerly known as switchcase 8.
                LoadNextFrom0CMaskAndShift(exitValue, currentCaseIndex - 1);

                shiftHigh = false;
                if (mem0760 == 0xBFC8)
                {
                    loaded16.Low8 = cache7E0740[0x10];
                    loaded16.High8 = cache7E0740[0x11];
                    shiftHigh = a >= loaded16.Data16;
                }

                if (shiftHigh)
                {
                    ShiftThenLoad100ThenCompare(6, 0x732, 2);
                }
                else
                {
                    ShiftThenLoad100ThenCompare(7, 0x0730, 0x1);
                }

                // This is 8 bit acc.
                loaded16.Data16 = a;
                if (indirectHigh == 0x7E && indirectLow >= 0x100)
                {
                    cache7E0100[indirectLow - 0x100] = loaded16.Low8;
                }
                else if (indirectHigh == 0x7F)
                {
                    cache7F0000[indirectLow] = loaded16.Low8;
                }

                indirectLow += 1;

                loaded16.Data16 = mem08;
                loaded16.Low8 = a & 0xFF;
                mem08 = loaded16.Data16;
                mem0c++;

                loaded16 = LoadMem6b();
                a = loaded16.Data16;
                for (int iter = 0; iter < 8; iter++)
                {
                    bool foundMatch = false;
                    if (x == caseTable8Entries[iter].Cond)
                    {
                        for (int i = caseTable8Entries[iter].Lower; i < caseTable8Entries[iter].Lower + caseTable8Entries[iter].IterCount; ++i)
                        {
                            a *= 2;
                            if (i == 0 || i == 8)
                            {
                                LoadNextFrom0CInc();
                            }

                            y--;
                            if (y == 0)
                            {
                                LoadNextFrom0600();
                                nextCaseIndex = (i % 8) + 1;
                                foundMatch = true;
                                break;
                            }
                        }
                        if (foundMatch)
                        {
                            break;
                        }
                        __debugbreak(); //Should not hit
                    }
                }
            }
            else if (nextCaseCond == 0x12)
            {
                // Write output and check if done.
                x = exitValue;
                y = mem73 >> 8;
                Fn_80C2DC();
                mem6c = a;
                continueDecompression = Fn_80C232();
                if (!continueDecompression)
                {
                    return; // return from monstrosity
                }

                // Write the value 'mem08', mem6f times.
                assert(mem08 <= 0xFF);
                decompressedValue = static_cast<unsigned char>(mem08);
                for (int i = 0; i < mem6f; ++i)
                {
                    if (indirectHigh == 0x7E && indirectLow >= 0x100)
                    {
                        cache7E0100[indirectLow - 0x100] = decompressedValue;
                    }
                    else if (indirectHigh == 0x7F)
                    {
                        cache7F0000[indirectLow] = decompressedValue;
                    }
                    else
                    {
                        __debugbreak();
                    }
                    indirectLow += 1;
                }

                a = mem6c;

                nextCaseCond = x;
                LoadNextFrom0600();
                nextCaseIndex = s_caseTable[0].NextCaseIndices[nextCaseCond / 2 - 1];
                continue;
            }
        }
    }

    void Fn_80BBB3()
    {
        // This is a sizeable function, a.k.a. 'the monstrosity'.
        //
        // Preconditions:
        //     Mem0C contains the source ROM address.
        //     Mem10 contains the low short of the destination address. (E.g., 0x0000)
        //     Mem12 contains the bank of the destination address. (E.g., 0x7F)
        //
        // Postconditions:
        //     Decompressed staging data is written to the destination address.
        //     Mem0C is scrambled.
        //
        // Notes:
        //     A, X, Y are ignored and stomped on.

        Monstrosity0();

        Monstrosity1();
    }

    void Fn_80C1B0()
    {
        mem6f = 0;
        a = mem6c;

        c = a >= 0x8000;
        a *= 2;

        --y;
        if (y == 0)
        {
            LoadNextFrom0CInc();
            y = 0x8;
        }

        if (c)
        {
            ShiftRotateDecrement(0, 1);

            if (z)
            {
                LoadNextFrom0CInc();
                y = 0x8;
            }

            ShiftRotateDecrement(0, 1);

            if (z)
            {
                LoadNextFrom0CInc();
                y = 8;
            }

            mem6c = a;
            a = mem6f;
            return;
        }

        mem00.Data16 = x;
        mem04 = 0x2;

        c = false;
        while (!c)
        {
            c = a >= 0x8000;
            a *= 2;

            --y;
            if (y == 0)
            {
                LoadNextFrom0CInc();
                y = 0x8;
            }

            ++mem04;
        }


        for (int i = 0; i < mem04; ++i)
        {
            ShiftRotateDecrement(0, 1);

            if (z)
            {
                LoadNextFrom0CInc();
                y = 0x8;
            }
        }

        mem6c = a;
        mem04 *= 2;
        x = mem00.Data16;

        static unsigned short s_ROMValueTable_80C2B6[] = { 0, 0, 0, 0x4, 0xC, 0x1C, 0x3C, 0x7C, 0xFC };
        mem6f += s_ROMValueTable_80C2B6[mem04 / 2];
        a = mem6f; // Caller expects return value in a
    }

    bool Fn_80C232() // Returns whether we should continue decompression.
    {
        // Input: x, mem6c
        mem6f = 0;
        a = mem6c;

        willCarry = a >= 0x8000;
        a *= 2;
        c = willCarry;

        x -= 2;

        if (x == 0)
        {
            LoadNextFrom0CInc();
            x = 0x10;
        }

        if (c)
        {
            ShiftRotateDecrement(2, 0);

            if (z)
            {
                LoadNextFrom0CInc();
                x = 0x10;
            }

            ShiftRotateDecrement(2, 0);

            if (!z)
            {
                mem6c = a;
                a = mem6f;
                return mem6f != 0;
            }

            LoadNextFrom0CInc();

            x = 0x10;
            mem6c = a;
            return mem6f != 0;
        }

        y = 2;

        c = false;
        while (!c)
        {
            willCarry = a >= 0x8000;
            a *= 2;
            c = willCarry;

            x -= 2;
            if (x == 0)
            {
                LoadNextFrom0CInc();
                x = 0x10;
            }

            y++;
        }

        mem14 = y;

        for (int i = 0; i < y; ++i)
        {
            ShiftRotateDecrement(2, 0);

            if (z)
            {
                LoadNextFrom0CInc();
                x = 0x10;
            }
        }

        mem6c = a;

        static const unsigned short lookup[] = { 0x4, 0xC, 0x1C, 0x3C, 0x7C };
        int lookupIndex = (mem14 * 2 - 6) / 2;
        mem6f += lookup[lookupIndex];
        return mem6f != 0;
    }

    void Fn_80C2DC()
    {
        // Input: a, x and y
        // Output: a, mem0c, y
        for (int i = 0; i < y; ++i)
        {
            a *= 2;
            x -= 2;
            if (x == 0)
            {
                LoadNextFrom0CInc(); // Updates a and mem0c
                x = 0x10;
            }
        }
    }

    bool DoubleAndCheckCarry(unsigned short* pSourceDataOffset)
    {
        bool carry = *pSourceDataOffset >= 0x8000;
        *pSourceDataOffset *= 2;
        return carry;
    }

    struct IndexedColorResult
    {
        unsigned short Low;
        unsigned short High;
    };

    void GetIndexedColor(
        unsigned short resultComponent,
        unsigned short* pSourceDataOffset,
        IndexedColorResult* pResult)
    {
        if (DoubleAndCheckCarry(pSourceDataOffset))
        {
            pResult->High |= (resultComponent << 8);
        }

        if (DoubleAndCheckCarry(pSourceDataOffset))
        {
            pResult->High |= resultComponent;
        }

        if (DoubleAndCheckCarry(pSourceDataOffset))
        {
            pResult->Low |= (resultComponent << 8);
        }

        if (DoubleAndCheckCarry(pSourceDataOffset))
        {
            pResult->Low |= resultComponent;
        }
    }

    bool LoadSourceElement(unsigned short* pSourceDataOffset, unsigned short* pResultComponent)
    {
        // Two bytes are loaded at a time.
        while (true)
        {
            loaded16.Low8 = cache7F0000[*pSourceDataOffset + 1];
            loaded16.High8 = cache7F0000[*pSourceDataOffset];

            if (loaded16.Data16 != 0)
            {
                return true;
            }

            if (*pResultComponent < 16)
            {
                return false;
            }

            *pResultComponent >>= 4;
            *pSourceDataOffset += 2;
        }

        return true;
    }

    bool FormulateOutput(
        unsigned short iter,
        unsigned short* pResultComponent,
        unsigned short* pSourceDataOffset,
        IndexedColorResult* pResult)
    {
        while (true)
        {
            GetIndexedColor(*pResultComponent, pSourceDataOffset, pResult);

            if (*pResultComponent < 2)
                return false;

            *pResultComponent /= 2;

            if (*pResultComponent >= 0x8 && *pResultComponent < 0x10)
            {
                // resultComponent is [8..15]
                *pSourceDataOffset = (iter * 4) + 2;
                return true;
            }
        }
    }

    IndexedColorResult CalculateIndexedColorResult(int iter)
    {
        IndexedColorResult result{};

        unsigned short sourceDataOffset = iter * 4;
        unsigned short resultComponent = 0x80;
        while (true)
        {
            if (!LoadSourceElement(&sourceDataOffset, &resultComponent))
                break;

            sourceDataOffset = loaded16.Data16;
            if (!FormulateOutput(iter, &resultComponent, &sourceDataOffset, &result))
                break;
        }

        return result;
    }

    void WriteIndexed(unsigned short homeOrAway)
    {
        // Figure out the destination offset based on profile index and whether we're home or away.
        unsigned short localIndex = 0xA - (currentProfileImageIndex * 2);
        unsigned short destDataAddressLow = homeOrAway == 0 ? 0x5100 : 0x2D00;
        destDataAddressLow += Load16FromAddress(0x9D, 0xCCAE + localIndex).Data16;

        for (int iter = 0; iter < 0x240; ++iter)
        {
            IndexedColorResult result = CalculateIndexedColorResult(iter);

            int highOrder = (iter / 8) * 0x20;
            int lowOrder = (iter % 8) * 2;
            int r = highOrder + lowOrder;

            // Write four bytes of output.

            loaded16.Data16 = result.Low;
            cache7F0000[destDataAddressLow + r] = loaded16.Low8;
            cache7F0000[destDataAddressLow + r + 1] = loaded16.High8;

            r += 0x10;

            loaded16.Data16 = result.High;
            cache7F0000[destDataAddressLow + r] = loaded16.Low8;
            cache7F0000[destDataAddressLow + r + 1] = loaded16.High8;
        }
    }

    void CreateCaches()
    {
        cache7E0100.resize(0x100);
        mem7E0500_7E0700.resize(0x200);
        cache7E0700.resize(0x14);
        cache7E0720.resize(0x20);
        cache7E0740.resize(0x20);
        cache7F0000.resize(0xFFFF);
        goldReferenceIndexedColor.resize(0x600);
    }

    void InitializeCaches()
    {
        memset(cache7E0100.data(), 0, cache7E0100.size());
        memset(mem7E0500_7E0700.data(), 0, mem7E0500_7E0700.size());
        memset(cache7E0700.data(), 0, cache7E0700.size());
        memset(cache7E0720.data(), 0, cache7E0720.size());
        memset(cache7E0740.data(), 0, cache7E0740.size());
        memset(cache7F0000.data(), 0, cache7F0000.size());
        memset(goldReferenceIndexedColor.data(), 0, goldReferenceIndexedColor.size());
    }

    struct TeamInfo
    {
        std::string Name;
        int CompressedDataLocations[6];
    };
    TeamInfo s_teams[] = {
        { "Anaheim",    0x99AC36, 0x99BFD5, 0x98CD0A, 0x98D19D, 0x979931, 0x98BCF3 },
        { "Boston",     0x97F28A, 0x998000, 0x99D102, 0x999867, 0x98E3CD, 0x98F814 },
        { "Buffalo",    0x998D5D, 0x98B16A, 0x98A5CD, 0x99C1FE, 0x989A18, 0x99B2C7 },
        { "Calgary",    0x97BA0D, 0x97E8E8, 0x978E95, 0x97EB52, 0x99AA05, 0x99CCBA },
        { "Chicago",    0x97A600, 0x99B4F7, 0x998B25, 0x97CDF3, 0x98A821, 0x98EA95 },
        { "Dallas",     0x96FD81, 0x98A120, 0x98ECD6, 0x97F023, 0x9A8C2C, 0x97AD8B },

        //                                                                Yzerman
        { "Detroit",    0x98F5D7, 0x9A940F, 0x99FAB4, 0x979BC5, 0x90FDE2, 0x99C426 },

        { "Edmonton",   0x9993FD, 0x99B097, 0x98898F, 0x97C682, 0x97BC8C, 0x98D876 },
        { "Florida",    0x97C188, 0x97F9BC, 0x97D06C, 0x998479, 0x97A0E8, 0x989EC9 },
        { "Hartford",   0x98DD01, 0x98AF19, 0x999F04, 0x98B608, 0x98ACC7, 0x99EE68 },
        { "LA",         0x97E40B, 0x97C8FE, 0x97A887, 0x97D7CC, 0x98D62E, 0x97DF28 },

        //              P.Roy                                             Muller
        { "Montreal",   0x9A862E, 0x9988EC, 0x99DFCF, 0x98CAC0, 0x97D557, 0x99F8AC },

        { "NewJersey",  0x9AA329, 0x98F158, 0x988264, 0x97AB0A, 0x9AA6D6, 0x99D545 },
        { "NYIslanders",0x9986B3, 0x99F079, 0x97C405, 0x9991C8, 0x98A377, 0x9A8225 },
        { "NYRangers",  0x9A8E26, 0x979406, 0x9884C8, 0x98CF54, 0x97B28D, 0x98BF41 },
        { "Ottawa",     0x8CFDEC, 0x99EA44, 0x99DB9B, 0x99F498, 0x99AE67, 0x99E619 },
        { "Philly",     0x988E50, 0x98C18E, 0x99E831, 0x9897BF, 0x98FC8E, 0x98DABC },

        //                                                                Lemieux
        { "Pittsburgh", 0x99F6A3, 0x989566, 0x9A8A31, 0x9A9F70, 0x98E610, 0x9AA500 },

        { "Quebec",     0x98EF17, 0x99D325, 0x97A374, 0x9A801F, 0x99A5A0, 0x9A901F },
        { "SJ",         0x98AA74, 0x98872C, 0x97F757, 0x98930C, 0x98FA51, 0x98B3B9 },
        { "STLouis",    0x9A9218, 0x98E18A, 0x99E401, 0x98F399, 0x99A36C, 0x9A99C9 },
        { "TampaBay",   0x97EDBC, 0x988000, 0x9A8832, 0x99A138, 0x99F289, 0x99BDAC },
        { "Toronto",    0x998F93, 0x99D981, 0x98D3E6, 0x979E57, 0x99FCBA, 0x9AA8AB },
        { "Vancouver",  0x98C3DB, 0x99DDB5, 0x97BF0A, 0x98B857, 0x98BAA5, 0x99B954 },
        { "Washington", 0x97DCB5, 0x98E853, 0x98C628, 0x99EC56, 0x988BF0, 0x97B00C },
        { "Winnipeg",   0x999632, 0x9A9D91, 0x99823D, 0x9A842A, 0x97D2E2, 0x99A7D3 },

        //              P.Roy                                             Lemieux
        { "ASE",        0x9A862E, 0x9A8A31, 0x998000, 0x99C1FE, 0x98F814, 0x9AA500 },

        //                                                                Yzerman
        { "ASW",        0x97A600, 0x9A9D91, 0x9A940F, 0x9A842A, 0x98D62E, 0x99C426 },
    };

    void SplitBankAndOffset(int n, unsigned char* pBank, unsigned short* pOffset)
    {
        *pOffset = n & 0xFFFF;
        n >>= 16;
        *pBank = n & 0xFF;
        n >>= 8;
        if (n) __debugbreak();
    }

    void InitializeDecompress(int teamIndex, int playerIndex)
    {
        if (playerIndex < 0 || playerIndex > 5)
            __debugbreak();

        TeamInfo* pTeam = &s_teams[teamIndex];

        SplitBankAndOffset(pTeam->CompressedDataLocations[playerIndex], &dbr, &mem0c);
        currentProfileImageIndex = playerIndex;

        {
            outputCpuLogFileName = ""; // No log
        }
        {
            std::stringstream strm;
            strm << pTeam->Name << playerIndex << ".out.bin";
            outputIndexedColorFileName = strm.str();
        }
        {
            std::stringstream strm;
            strm << "C:\\repos\\nhl94e\\ImageData\\" << pTeam->Name << ".bin";
            goldIndexedColorFileName = strm.str();
        }
    }

    void InitializeCPUAndOtherWRAM()
    {
        a = 0xFB30;
        y = 0xF8AE;
        n = false;
        z = false;
        c = false;
        mem00.Data16 = 0;
        mem04 = 0;
        mem06 = 0;
        mem08 = 0;
        mem0c = 0;
        mem0e = 0;
        mem10 = 0;
        mem12 = 0x007F;
        mem14 = 0;
        mem16 = 0;
        mem6a = 0;
        mem6c = 0;
        mem6f = 0;
        mem71 = 0;
        mem73 = 0;
        mem75 = 0;
        mem79 = 0;
        mem7b = 0;
        mem7d = 0;
        mem0760 = 0;
        loaded = 0;
        loaded16.Data16 = 0;
        low = 0;
        willCarry = false;
        willSetNegative = false;
    }

    // Player indices are in chronological written order, not some other order.
    int GetFinalWriteLocation()
    {
        int finalResultWriteLocation;
        int localIndex = 5 - currentProfileImageIndex;
        if (mem91_HomeOrAway == 0)
        {
            finalResultWriteLocation = 0x5100 + (0x600 * localIndex);
        }
        else if (mem91_HomeOrAway == 2)
        {
            finalResultWriteLocation = 0x2D00 + (0x600 * localIndex);
        }
        else
        {
            __debugbreak();
        }

        return finalResultWriteLocation;
    }

    void DumpDecompressedResult(int finalResultWriteLocation)
    {
        FILE* file{};
        fopen_s(&file, outputIndexedColorFileName.c_str(), "wb");
        unsigned char* pData = cache7F0000.data();
        fwrite(pData + finalResultWriteLocation, 1, 0x600, file);
        fclose(file);
    }

    void Decompress(int teamIndex, int playerIndex)
    {
        InitializeCaches();
        InitializeCPUAndOtherWRAM();

        InitializeDecompress(teamIndex, playerIndex);
        x = 0;
        y = mem0c + 2;
        mem91_HomeOrAway = 2;

        Fn_80BBB3();
        WriteIndexed(mem91_HomeOrAway);

        int finalResultWriteLocation = GetFinalWriteLocation();
        //DumpDecompressedResult(finalResultWriteLocation);

        // Diff decompressed result against the reference-- validate they're the same
        // Open the reference
        {
            FILE* file{};
            fopen_s(&file, goldIndexedColorFileName.c_str(), "rb");
            fseek(file, 0x600 * (5 - currentProfileImageIndex), SEEK_SET);
            fread(goldReferenceIndexedColor.data(), 1, 0x600, file);
            fclose(file);
        }

        // Diff decompressed result against the reference
        {
            unsigned char* pData = cache7F0000.data();
            unsigned char* pDecompressed = pData + finalResultWriteLocation;
            // There is garbage in memory stored past 0x480- although the entries are spaced 0x600 apart, only the first 0x480 are viable.
            for (int i = 0; i < 0x480; ++i)
            {
                if (pDecompressed[i] != goldReferenceIndexedColor[i])
                {
                    // Mismatch
                    __debugbreak();
                }
            }
        }
    }
     
}

bool Decompress_Fast_Init()
{
    // Load ROM file
    Fast::romFile = LoadBinaryFile8("nhl94.sfc");
    if (Fast::romFile.size() == 0)
        return false;

    Fast::CreateCaches();
    return true;
}

void Decompress_Fast_Run(int teamIndex, int playerIndex)
{
    Fast::Decompress(teamIndex, playerIndex);
}