#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <iomanip>
#include <assert.h>
#include "Util.h"
#include "Decompress2.h"

namespace Fast
{
    unsigned short Fn_80C1B0_GetSparseValueIncrement(unsigned short iter);
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

    bool outputCompressionRatio = false;
    bool outputDecompressedResult = false;

    Mem16 mem00{};
    unsigned short mem06 = 0;
    unsigned short mem08 = 0;
    unsigned short mem0c = 0xF8AC;
    unsigned short mem0e = 0x7f;
    unsigned short mem10 = 0;
    unsigned short mem12 = 0x007F;
    unsigned short mem6a = 0;
    Mem16 LoadMem6b();
    void SaveMem6b(Mem16 const& v);
    unsigned short mem6c = 0;
    unsigned short mem6f = 0;
    unsigned short mem73 = 0;
    unsigned short mem7b = 0;
    unsigned short mem91_HomeOrAway = 0;
    unsigned short mem0760 = 0;

    unsigned short indirectHigh;
    unsigned short indirectLow;

    std::vector<unsigned char> cache7E0100; // Scratch data read and written by both Monstrosity0 and Monstrosity1.

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

    struct Monstrosity0Result
    {
        std::vector<unsigned char> mem7E0500_7E0700; // Monstrosity0 writes this. Monstrosity1 reads it.
        Mem32 cache7E0730;
        Mem16 cache7E0750; 
        unsigned short CaseCond;

        int CompressedSize; // For statistics-keeping
        void Initialize()
        {
            mem7E0500_7E0700.resize(0x200);   
            memset(mem7E0500_7E0700.data(), 0, mem7E0500_7E0700.size());
            cache7E0750.Data16 = 0;
            CompressedSize = 0;
        }
    };

    void LoadNextFrom0500(Monstrosity0Result const& result0, std::vector<unsigned char>* cache7F0000_decompressedStaging)
    {
        // Loads a value from the staging output written by Monstrosity0.
        // Saves the result to indirect.
        // Result is also returned in mem08.

        // 16bit A, 8bit index
        unsigned char loaded = result0.mem7E0500_7E0700[y];

        loaded16.Data16 = loaded;
        if (indirectHigh == 0x7E && indirectLow >= 0x100)
        {
            cache7E0100[indirectLow - 0x100] = loaded16.Low8;
        }
        else if (indirectHigh == 0x7F)
        {
            cache7F0000_decompressedStaging->data()[indirectLow] = loaded16.Low8;
        }
        else
        {
            __debugbreak();
        }
        indirectLow++;
        mem08 = loaded;
    }

    void LoadNextFrom0600(Monstrosity0Result const& result0)
    {
        mem6c = a;
        y = mem6c >> 8;
        x = result0.mem7E0500_7E0700[0x100 + y];
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

    void ShiftRotateDecrementMem7F(int xDecAmt, int yDecAmt)
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

    Monstrosity0Result Monstrosity0()
    {
        Monstrosity0Result result{};
        result.Initialize();

        // Some temp memory used for this function
        std::vector<unsigned char> cache7E0700temp;
        cache7E0700temp.resize(0x14); // A range of 0x20 looks possible in theory, but only 0x14 bytes are used in practice.
        memset(cache7E0700temp.data(), 0, cache7E0700temp.size());

        std::vector<unsigned char> cache7E0720temp;
        cache7E0720temp.resize(0x20);
        memset(cache7E0720temp.data(), 0, cache7E0720temp.size());

        std::vector<unsigned char> cache7E0740temp;
        cache7E0740temp.resize(0x20);
        memset(cache7E0740temp.data(), 0, cache7E0740temp.size());

        unsigned short compressedSourceLocation = mem0c;

        x = 0;
        y = 0;
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
        unsigned short valueIncrementTotal = 0;
        unsigned short valueAccumulator = 0;
        unsigned short numDatumMultiplies = 0xF;
        a = 0x10;
        x = 0xFE;
        int setBytesInCacheCounter = 0;
        unsigned short controlFlowSwitch = 0;
        c = false;
        int iteration = 0;
        bool doneInitializing = false;

        // This loop sets values of cache7E0700temp, cache7E0720temp and cache7E0740temp.
        while (!doneInitializing)
        {
            valueAccumulator *= 2;
            unsigned short sparseValue = valueAccumulator - valueIncrementTotal;

            loaded16.Data16 = sparseValue;
            cache7E0720temp[iteration] = loaded16.Low8;
            cache7E0720temp[iteration + 1] = loaded16.High8;

            // 8bit index
            unsigned short valueIncrement = Fn_80C1B0_GetSparseValueIncrement(iteration);
            cache7E0700temp[iteration] = static_cast<unsigned char>(valueIncrement);

            valueIncrementTotal += valueIncrement;

            setBytesInCacheCounter = valueIncrementTotal;

            Mem16 datum{};
            if (valueIncrement != 0)
            {
                valueAccumulator += valueIncrement;
                datum.Data16 = valueAccumulator;

                for (int i = 0; i < numDatumMultiplies-1; ++i)
                {
                    datum.Data16 *= 2;
                }

                // If the last multiply makes the datum end up going negative, we exit the loop.
                doneInitializing = datum.Data16 >= 0x8000; 
                datum.Data16 *= 2;
            }

            cache7E0740temp[iteration] = datum.Low8;
            cache7E0740temp[iteration + 1] = datum.High8;

            iteration += 2;
            numDatumMultiplies--;
        }

        controlFlowSwitch = iteration - 2;

        // Zero out the intermediate
        for (int i = 0; i < 0x100; i++)
        {
            result.mem7E0500_7E0700[i] = 0;
        }

        // This is hard coded.
        indirectHigh = 0x7E;
        indirectLow = 0x100;

        x = 0xFF;

        // Set bytes in cache
        while (setBytesInCacheCounter != 0)
        {
            // Skip the x index past N entries in the cache which are too low, < 0x80.
            // If x gets to go past 255, it wraps back to 0.
            // There are guaranteed to actually be enough low entries.
            unsigned short howManyLowEntriesToSkip = Fn_80C1B0_GetSparseValueIncrement(x) + 1;
            while (howManyLowEntriesToSkip > 0)
            {
                ++x;
                x &= 0xFF;
                if (result.mem7E0500_7E0700[x] < 0x80)
                {
                    --howManyLowEntriesToSkip;
                }
            }

            result.mem7E0500_7E0700[x]--;

            // This is running in 8 bit index mode.
            loaded16.Data16 = x;
            cache7E0100[indirectLow - 0x100] = loaded16.Low8;
            indirectLow += 1;

            setBytesInCacheCounter--;
        }
        result.CaseCond = y * 2;
        y = 0;
        mem7b = 0;

        Mem16 resultValue00{};
        while (mem7b * 2 != 0x10)
        {
            x = resultValue00.Data16;

            int numOfBytesToSeek = cache7E0700temp[mem7b * 2];
            for (int i = 0; i < numOfBytesToSeek; ++i)
            {
                resultValue00.Data16 = mem7b * 2;

                // This is running in 8 bit accumulator and index mode.
                loaded16.Data16 = resultValue00.Data16;
                loaded16.Low8 = cache7E0100[y];
                a = loaded16.Data16;

                resultValue00.High8 = cache7E0100[y];

                ++y;

                if (a == (mem73 & 0xFF))
                {
                    a = mem7b + 1;

                    mem73 &= 0x00FF; // Keep the first, lower byte
                    mem73 |= (a << 8); // Replace the upper byte, second byte

                    resultValue00.Data16 = 0x12;
                }

                // Mem00 contains the data to get written.
                int numberOfBytesToWrite = romFile[0x3C7B + mem7b] - 1;
                for (int j = 0; j < numberOfBytesToWrite; ++j)
                {
                    result.mem7E0500_7E0700[x] = resultValue00.High8;
                    result.mem7E0500_7E0700[0x100 + x] = resultValue00.Low8;
                    ++x;
                }
            }

            mem7b++;
            resultValue00.Data16 = x;
        }

        x = resultValue00.Low8;

        // Always tack a bunch of 0x10 on at the end.
        int byteCountToSet = 0xFF - x + 1;
        for (int i=0; i< byteCountToSet; ++i) 
        {            
            loaded16.Data16 = a;
            result.mem7E0500_7E0700[0x100 + x] = 0x10;

            ++x;
            x &= 0x00FF;
        }

        if (controlFlowSwitch == 0x10)
        {
            mem0760 = 0xBFC5;
        }
        else if (controlFlowSwitch == 0x12)
        {
            mem0760 = 0xBFC8;
        }
        else
        {
            __debugbreak(); // notimpl
        }

        indirectHigh = mem12;
        indirectLow = mem10;
        result.CompressedSize = mem0c - compressedSourceLocation;

        result.cache7E0730.Low8 = cache7E0720temp[0x10];
        result.cache7E0730.Mid8 = cache7E0720temp[0x11];
        result.cache7E0730.High8 = cache7E0720temp[0x12];
        result.cache7E0730.Top8 = cache7E0720temp[0x13];

        result.cache7E0750.Low8 = cache7E0740temp[0x10];
        result.cache7E0750.High8 = cache7E0740temp[0x11];

        return result;
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

    std::vector<unsigned char> Monstrosity1(int teamIndex, int playerIndex, Monstrosity0Result & result0)
    {
        bool continueDecompression = true;
        unsigned char decompressedValue = 0;
        bool shiftHigh = false;
        unsigned short currentCaseIndex = 0;
        unsigned short nextCaseCond = 0;
        unsigned short nextCaseIndex = 0;
        unsigned short mainIndex = 0;
        unsigned short exitValue = 0;

        if (teamIndex == 0 && playerIndex == 0)
        {
        }

        a = mem6c;
        x = result0.CaseCond;
        nextCaseCond = result0.CaseCond;
        LoadNextFrom0600(result0);
        nextCaseIndex = s_caseTable[0].NextCaseIndices[nextCaseCond / 2 - 1];

        std::vector<unsigned char> cache7F0000_decompressedStaging;
        cache7F0000_decompressedStaging.resize(0xFFFF);
        memset(cache7F0000_decompressedStaging.data(), 0, cache7F0000_decompressedStaging.size());

        bool doneDecompression = false;

        while (!doneDecompression)
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
                LoadNextFrom0500(result0, &cache7F0000_decompressedStaging);
                LoadNextFrom0600(result0);
                continue;
            }

            if (nextCaseCond == 0x10)
            {
                // The jump760 case with what was formerly known as switchcase 8.
                LoadNextFrom0CMaskAndShift(exitValue, currentCaseIndex - 1);

                shiftHigh = false;
                if (mem0760 == 0xBFC8)
                {
                    loaded16.Low8 = result0.cache7E0750.Low8;
                    loaded16.High8 = result0.cache7E0750.High8;
                    shiftHigh = a >= loaded16.Data16;
                }

                unsigned short loadSource = a;

                if (shiftHigh)
                {
                    loadSource /= 64;
                    loadSource -= result0.cache7E0730.High16;
                    y = 2;
                }
                else
                {
                    loadSource /= 128;
                    loadSource -= result0.cache7E0730.Low16;
                    y = 1;
                }

                a = cache7E0100[loadSource];
                z = a == (mem73 & 0xFF); // we are in 8bit mode

                // This is 8 bit acc.
                loaded16.Data16 = a;
                if (indirectHigh == 0x7E && indirectLow >= 0x100)
                {
                    cache7E0100[indirectLow - 0x100] = loaded16.Low8;
                }
                else if (indirectHigh == 0x7F)
                {
                    cache7F0000_decompressedStaging[indirectLow] = loaded16.Low8;
                }

                indirectLow += 1;

                mem08 = a;
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
                                LoadNextFrom0600(result0);
                                nextCaseIndex = (i % 8) + 1;
                                foundMatch = true;
                                break;
                            }
                        }
                        assert(foundMatch);
                        break;
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
                    doneDecompression = true;
                    break;
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
                        cache7F0000_decompressedStaging[indirectLow] = decompressedValue;
                    }
                    else
                    {
                        __debugbreak();
                    }
                    indirectLow += 1;
                }

                a = mem6c;

                nextCaseCond = x;
                LoadNextFrom0600(result0);
                nextCaseIndex = s_caseTable[0].NextCaseIndices[nextCaseCond / 2 - 1];
            }
        }

        return cache7F0000_decompressedStaging;
    }

    struct Fn_80BBB3_DecompressResult
    {
        std::vector<unsigned char> cache7F0000_decompressedStaging;
        int CompressedSize;
    };

    Fn_80BBB3_DecompressResult Fn_80BBB3_Decompress(int teamIndex, int playerIndex)
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

        Fn_80BBB3_DecompressResult result{};

        Monstrosity0Result result0 = Monstrosity0();
        result.CompressedSize = result0.CompressedSize;

        result.cache7F0000_decompressedStaging = Monstrosity1(teamIndex, playerIndex, result0);
        return result;
    }

    unsigned short Fn_80C1B0_GetSparseValueIncrement(unsigned short iter)
    {
        // Input: mem6c, which is the SwapToken from the compressed data.
        //        y as an index. y is [0..7]
        // 
        // Multiplies mem6c a bunch of times, and/or replaces it with the next compressed byte.
        // Advances mem0c.

        mem6f = 0;
        a = mem6c;

        c = a >= 0x8000;
        a *= 2;

        --y;
        if (y == 0)
        {
            LoadNextFrom0CInc(); // Clobbers a. Effectively forgets SwapToken, and uses the next compressed byte instead
            y = 0x8;
        }

        if (c)
        {
            ShiftRotateDecrementMem7F(0, 1); // Sets z if y==0

            if (z)
            {
                LoadNextFrom0CInc();
                y = 0x8;
            }

            ShiftRotateDecrementMem7F(0, 1);

            if (z)
            {
                LoadNextFrom0CInc();
                y = 8;
            }

            mem6c = a;
            return mem6f;
        }

        mem00.Data16 = iter;
        unsigned short numberOfRotates = 0x2;

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

            ++numberOfRotates;
        }

        for (int i = 0; i < numberOfRotates; ++i)
        {
            ShiftRotateDecrementMem7F(0, 1);

            if (z)
            {
                LoadNextFrom0CInc();
                y = 0x8;
            }
        }

        mem6c = a;
        x = mem00.Data16;

        static unsigned short s_ROMValueTable_80C2B6[] = { 0, 0, 0, 0x4, 0xC, 0x1C, 0x3C, 0x7C, 0xFC };
        mem6f += s_ROMValueTable_80C2B6[numberOfRotates];
        return mem6f;
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
            ShiftRotateDecrementMem7F(2, 0);

            if (z)
            {
                LoadNextFrom0CInc();
                x = 0x10;
            }

            ShiftRotateDecrementMem7F(2, 0);

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

        for (int i = 0; i < y; ++i)
        {
            ShiftRotateDecrementMem7F(2, 0);

            if (z)
            {
                LoadNextFrom0CInc();
                x = 0x10;
            }
        }

        mem6c = a;

        static const unsigned short lookup[] = { 0x4, 0xC, 0x1C, 0x3C, 0x7C };
        int lookupIndex = (y * 2 - 6) / 2;
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
        unsigned short* pAcc,
        IndexedColorResult* pResult)
    {
        if (DoubleAndCheckCarry(pAcc))
        {
            pResult->High |= (resultComponent << 8);
        }

        if (DoubleAndCheckCarry(pAcc))
        {
            pResult->High |= resultComponent;
        }

        if (DoubleAndCheckCarry(pAcc))
        {
            pResult->Low |= (resultComponent << 8);
        }

        if (DoubleAndCheckCarry(pAcc))
        {
            pResult->Low |= resultComponent;
        }
    }

    void FormulateOutput(
        unsigned short acc,
        unsigned short* pResultComponent,
        IndexedColorResult* pResult)
    {
        while (true)
        {
            GetIndexedColor(*pResultComponent, &acc, pResult);

            if (*pResultComponent == 0 || *pResultComponent == 1)
                return;

            *pResultComponent /= 2;

            if (*pResultComponent == 0x8)
                return;
        }
    }

    struct IndexedColorToShorts
    {
        unsigned char DesiredIndexedColor[4];
        Mem32 ShortsCache;

        bool operator<(const IndexedColorToShorts& other) const
        {
            return ShortsCache.Data32 < other.ShortsCache.Data32;
        }
    };

    std::set<IndexedColorToShorts> indexedColorToShorts;

    void DumpIndexedColorToShortsImpl()
    {
        std::ofstream log;
        log.open("d:\\repos\\nhl94e\\decompress2\\indexedcolortoshorts\\TestData.h", std::ofstream::out );

        log << "#pragma once\n";
        log << "\n";
        log << "struct TestCase\n";
        log << "{\n";
        log << "    unsigned char IndexedColor[4];\n";
        log << "    unsigned short Shorts[2];\n";
        log << "} testCases[] =\n";
        log << "{\n";

        // Try all combs
        for (auto it = Fast::indexedColorToShorts.begin(); it != Fast::indexedColorToShorts.end(); ++it)
        {
            log << "{";
            log << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)it->DesiredIndexedColor[0] << ", ";
            log << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)it->DesiredIndexedColor[1] << ", ";
            log << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)it->DesiredIndexedColor[2] << ", ";
            log << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)it->DesiredIndexedColor[3] << ", ";

            log << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << it->ShortsCache.Low16 << ", ";
            log << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << it->ShortsCache.High16 << "},\n";

        }
        log << "};\n";

        log.close();
    }

    IndexedColorResult CalculateIndexedColorResult(int iter, std::vector<unsigned char> const& cache7F0000_decompressedStaging)
    {
        IndexedColorResult result{};

        unsigned short sourceDataOffset = iter * 4;
        unsigned short resultComponent = 0x80;

        // Load the two shorts
        Mem16 short0;
        short0.Low8 = cache7F0000_decompressedStaging[sourceDataOffset + 1];
        short0.High8 = cache7F0000_decompressedStaging[sourceDataOffset + 0];

        Mem16 short1;
        short1.Low8 = cache7F0000_decompressedStaging[sourceDataOffset + 3];
        short1.High8 = cache7F0000_decompressedStaging[sourceDataOffset + 2];


        if (short0.Data16 != 0)
        {
            FormulateOutput(short0.Data16, &resultComponent, &result);
        }

        if (resultComponent > 16)
        {
            resultComponent /= 16;
            assert(resultComponent < 16);
        }

        if (short1.Data16 != 0)
        {
            FormulateOutput(short1.Data16, &resultComponent, &result);
        }

        IndexedColorToShorts entry{};
        Mem16 resultComponents{};
        resultComponents.Data16 = result.Low;
        entry.DesiredIndexedColor[0] = resultComponents.Low8;
        entry.DesiredIndexedColor[1] = resultComponents.High8;
        resultComponents.Data16 = result.High;
        entry.DesiredIndexedColor[2] = resultComponents.Low8;
        entry.DesiredIndexedColor[3] = resultComponents.High8;
        entry.ShortsCache.Low16 = short0.Data16;
        entry.ShortsCache.High16 = short1.Data16;
        indexedColorToShorts.insert(entry);

        return result;
    }

    std::vector<unsigned char> WriteIndexed(unsigned short homeOrAway, std::vector<unsigned char> cache7F0000_decompressedStaging)
    {
        // This writes 0x480 bytes total.
        // The original SNES function wrote 0x900 bytes: 0x480 bytes of image data, and 0x480 bytes of zeroes.
        // But we're writing to already-zeroed memory so no need to write the zeroes here.

        // Each profile image is 48x48 pixels. 48x48 = 0x900, and each pixel is half a byte (16 possible colors per pixel), 
        // so 0x480 is the full decompressed size.

        // Figure out the destination offset based on profile index and whether we're home or away.
        unsigned short localIndex = 0xA - (currentProfileImageIndex * 2);
        unsigned short destDataAddressLow = homeOrAway == 0 ? 0x5100 : 0x2D00;
        destDataAddressLow += Load16FromAddress(0x9D, 0xCCAE + localIndex).Data16;

        std::vector<unsigned char> cache7F0000_indexedColor;
        cache7F0000_indexedColor.resize(0xFFFF);
        memset(cache7F0000_indexedColor.data(), 0, cache7F0000_indexedColor.size());

        for (int iter = 0; iter < 0x120; ++iter)
        {
            IndexedColorResult result = CalculateIndexedColorResult(iter, cache7F0000_decompressedStaging);
            Mem16 resultComponents{};

            int destOffsetHighOrder = (iter / 8) * 0x20;
            int destOffsetLowOrder = (iter % 8) * 2;
            int destOffset = destOffsetHighOrder + destOffsetLowOrder;

            // Write four bytes of output.

            resultComponents.Data16 = result.Low;
            cache7F0000_indexedColor[destDataAddressLow + destOffset] = resultComponents.Low8;
            cache7F0000_indexedColor[destDataAddressLow + destOffset + 1] = resultComponents.High8;

            destOffset += 0x10;

            resultComponents.Data16 = result.High;
            cache7F0000_indexedColor[destDataAddressLow + destOffset] = resultComponents.Low8;
            cache7F0000_indexedColor[destDataAddressLow + destOffset + 1] = resultComponents.High8;
        }

        return cache7F0000_indexedColor;
    }

    void CreateCaches()
    {
        cache7E0100.resize(0x100);
        goldReferenceIndexedColor.resize(0x600);
    }

    void InitializeCaches()
    {
        memset(cache7E0100.data(), 0, cache7E0100.size());
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
            strm << "D:\\repos\\nhl94e\\ImageData\\" << pTeam->Name << ".bin";
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
        mem06 = 0;
        mem08 = 0;
        mem0c = 0;
        mem0e = 0;
        mem10 = 0;
        mem12 = 0x007F;
        mem6a = 0;
        mem6c = 0;
        mem6f = 0;
        mem73 = 0;
        mem7b = 0;
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

    void DumpDecompressedResult(std::vector<unsigned char> const& cache7F0000_indexedColor, int finalResultWriteLocation)
    {
        FILE* file{};
        fopen_s(&file, outputIndexedColorFileName.c_str(), "wb");
        unsigned char const* pData = cache7F0000_indexedColor.data();
        fwrite(pData, 1, cache7F0000_indexedColor.size(), file);
        fclose(file);
    }

    void Decompress(int teamIndex, int playerIndex)
    {
        InitializeCaches();
        InitializeCPUAndOtherWRAM();

        InitializeDecompress(teamIndex, playerIndex);
        mem91_HomeOrAway = 2;

        Fn_80BBB3_DecompressResult decompressedStaging = Fn_80BBB3_Decompress(teamIndex, playerIndex);

        /*
        if (teamIndex == 0 && playerIndex ==0)
        {
            std::stringstream outPath;
            outPath << "D:\\repos\\nhl94e\\Decompress2\\StageToShorts\\Anaheim_0\\Shorts_Hacked_";
            outPath << GetTeamName((Team)teamIndex);
            outPath << "_" << playerIndex << ".bin";

            FILE* file{};
            fopen_s(&file, outPath.str().c_str(), "wb");
            unsigned char const* pData = decompressedStaging.cache7F0000_decompressedStaging.data();
            fwrite(pData, 1, 0x480, file);
            fclose(file);
            exit(0);
        }*/

        std::vector<unsigned char> cache7F0000_indexedColor = WriteIndexed(mem91_HomeOrAway, decompressedStaging.cache7F0000_decompressedStaging);

        int finalResultWriteLocation = GetFinalWriteLocation();

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
            unsigned char* pData = cache7F0000_indexedColor.data();
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

        if (outputCompressionRatio)
        {
            int sourceSize = decompressedStaging.CompressedSize;
            int destSize = 0x480;
            float ratio = (float)sourceSize / (float)destSize;
            float ratioPercent = ratio * 100;

            std::string teamName = GetTeamName((Team)teamIndex);
            std::string playerName = GetPlayerName(s_teams[teamIndex].CompressedDataLocations[playerIndex]);
            std::cout << teamName << "\t|" << playerName << "\t|" << sourceSize << "\t|Compression ratio : \t|" << ratioPercent << "% " << "\n";
        }

        if (outputDecompressedResult)
        {
            DumpDecompressedResult(cache7F0000_indexedColor, finalResultWriteLocation);
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

void DumpIndexedColorToShorts()
{
    Fast::DumpIndexedColorToShortsImpl();
}