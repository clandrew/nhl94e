#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <iomanip>
#include <assert.h>
#include <Windows.h>
#include "Util.h"
#include "Decompress2.h"
#include "CompressedSizes.h"

namespace Fast
{
    unsigned short GetStagingBufferDescriptorCount(
        unsigned short iter,
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pNextCaseCond,
        unsigned short* pCompressedSourceIndex,
        unsigned short* pByteRepititionCount,
        unsigned short* pSwapValueToken,
        unsigned short* pCaseKey);
    bool Decode12(
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pCompressedSourceIndex,
        unsigned short* pByteRepititionCount, 
        unsigned short* pSwapValueToken,
        unsigned short* pCaseCond,
        unsigned short* pCacheIndex);
    void AlignedLoad(
        unsigned short cacheIndex,
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pCompressedSourceIndex,
        unsigned short* pSwapValueToken,
        unsigned short* pResultCaseCond);

    unsigned char dbr = 0x9A;
    unsigned short currentProfileImageIndex = 0;
    std::string outputCpuLogFileName;
    std::string outputIndexedColorFileName;
    std::string goldIndexedColorFileName;

    bool outputCompressionRatio = false;
    bool outputDecompressedResult = false;

    unsigned char shiftedCompressedByte = 0;

    unsigned short mem91_HomeOrAway = 0;

    std::vector<unsigned char> dictionaryValues; // Scratch data read and written by both Monstrosity0 and Monstrosity1.
                                            // Data written by Monstrosity0 is read by Monstrosity1.

    // Gold reference output
    std::vector<unsigned char> goldReferenceIndexedColor;

    // Loaded plainly
    std::vector<unsigned char> romFile;

    unsigned char Load8FromAddress(unsigned short bank, unsigned short offset)
    {
        unsigned char result{};

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
            return romFile[fileOffset + local];
        }
        else if (bank == 0x7E)
        {
            if (offset == 0)
            {
                return 0;
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
                result.Data16 = 0;
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

    Mem16 Load16FromVector(std::vector<unsigned char> const& vec, int index)
    {
        Mem16 result;
        result.Low8 = vec[index];
        result.High8 = vec[index + 1];
        return result;
    }

    void LoadNextFrom0CInc(
        std::vector<unsigned char> const& compressedSource, 
        unsigned short* pCompressedSourceIndex, 
        unsigned short* pSwapValueToken)
    {
        // This runs in 8 bit mode.
        (*pSwapValueToken) &= 0xFF00;
        (*pSwapValueToken) |= compressedSource[*pCompressedSourceIndex];
        (*pCompressedSourceIndex)++;
    }

    struct Monstrosity0Result
    {
        std::vector<unsigned char> mem7E0500_7E0700; // Monstrosity0 writes this. Monstrosity1 reads it.
        Mem32 cache7E0730;
        Mem16 cache7E0750; 
        unsigned short CaseCond;
        unsigned short ControlFlowSwitch;
        unsigned short CompressedSourceIndex;
        unsigned short CompressedDataToken;
        unsigned short SwapValueToken;

        void Initialize()
        {
            mem7E0500_7E0700.resize(0x200);   
            memset(mem7E0500_7E0700.data(), 0, mem7E0500_7E0700.size());
            cache7E0750.Data16 = 0;
            CaseCond = 0;
            CompressedSourceIndex = 0;
            CompressedDataToken = 0;
            SwapValueToken = 0;
        }
    };

    unsigned short LoadNextFrom0500(
        Monstrosity0Result const& result0, 
        unsigned short cacheIndex, 
        std::vector<unsigned char>* cache7F0000_decompressedStaging,
        unsigned short indirectHigh,
        unsigned short* pIndirectLow)
    {
        // Loads a value from the staging output written by Monstrosity0.
        // Saves the result to indirect.

        // 16bit A, 8bit index
        unsigned char loaded = result0.mem7E0500_7E0700[cacheIndex];

        if (indirectHigh == 0x7E && (*pIndirectLow) >= 0x100)
        {
            dictionaryValues[(*pIndirectLow) - 0x100] = loaded;
        }
        else if (indirectHigh == 0x7F)
        {
            cache7F0000_decompressedStaging->data()[(*pIndirectLow)] = loaded;
        }
        else
        {
            __debugbreak();
        }
        (*pIndirectLow)++;
        return loaded;
    }

    void LoadNextFrom0CMaskAndShift(int shifts, std::vector<unsigned char> const& compressedSource, unsigned short compressedSourceIndex, unsigned short* pSwapValueToken)
    {
        Mem16 compressedShort = Load16FromVector(compressedSource, compressedSourceIndex); // Load a single byte.
        compressedShort.High8 = 0;

        for (int i = 0; i < shifts; ++i)
        {
            compressedShort.Data16 *= 2;
        }

        shiftedCompressedByte = compressedShort.Low8;
        *pSwapValueToken |= compressedShort.High8;
    }

    void ShiftRotateDecrementMem6F(unsigned short* pByteRepititionCount, unsigned short* pSwapValueToken, bool* pCarry)
    {
        (*pCarry) = (*pSwapValueToken) >= 0x8000;
        (*pSwapValueToken) *= 2;

        RotateLeft(pByteRepititionCount, pCarry);
    }

    Monstrosity0Result Monstrosity0(std::vector<unsigned char> const& compressedSource)
    {
        Monstrosity0Result result{};
        result.Initialize();

        // Some temp memory used for this function
        std::vector<unsigned char> stagingBufferDescriptorCounts;
        stagingBufferDescriptorCounts.resize(0xA); // A range of 0x10 looks possible in theory, but only 0xA bytes are used in practice.
        memset(stagingBufferDescriptorCounts.data(), 0, stagingBufferDescriptorCounts.size());

        std::vector<unsigned char> cache7E0720temp;
        cache7E0720temp.resize(0x20);
        memset(cache7E0720temp.data(), 0, cache7E0720temp.size());

        std::vector<unsigned char> cache7E0740temp;
        cache7E0740temp.resize(0x20);
        memset(cache7E0740temp.data(), 0, cache7E0740temp.size());

        unsigned short compressedSourceIndex = 0;
        compressedSourceIndex += 5;

        Mem16 loaded16 = Load16FromVector(compressedSource, compressedSourceIndex);
        unsigned short compressedDataToken = loaded16.Data16;
        compressedSourceIndex++;
        
        unsigned short swapValueToken;
        {
            loaded16 = Load16FromVector(compressedSource, compressedSourceIndex);
            swapValueToken = loaded16.Data16;
            compressedSourceIndex += 2;

            swapValueToken = ExchangeShortHighAndLow(swapValueToken);
        }

        unsigned short descriptorTotal = 0;
        unsigned short valueAccumulator = 0;
        unsigned short numDatumMultiplies = 0xE;
        unsigned short byteRepititionCount = 0;
        
        unsigned short caseKey = 8; // nextCaseCond is 2* this

        int setBytesInCacheCounter = 0;
        int iteration = 0;
        bool doneInitializing = false;

        // This loop sets values of stagingBufferDescriptorCounts, cache7E0720temp and cache7E0740temp.
        while (!doneInitializing)
        {
            valueAccumulator *= 2;

            Mem16 sparseValue{};
            sparseValue.Data16 = valueAccumulator - descriptorTotal;

            cache7E0720temp[iteration * 2] = sparseValue.Low8;
            cache7E0720temp[iteration * 2 + 1] = sparseValue.High8;

            // 8bit index
            unsigned short desciptorCount = GetStagingBufferDescriptorCount(
                iteration * 2,
                compressedSource,
                &result.CaseCond, 
                &compressedSourceIndex,
                &byteRepititionCount,
                &swapValueToken,
                &caseKey);
            stagingBufferDescriptorCounts[iteration] = static_cast<unsigned char>(desciptorCount);

            descriptorTotal += desciptorCount;

            setBytesInCacheCounter = descriptorTotal;

            Mem16 datum{};
            if (desciptorCount != 0)
            {
                valueAccumulator += desciptorCount;
                datum.Data16 = valueAccumulator;

                for (int i = 0; i < numDatumMultiplies; ++i)
                {
                    datum.Data16 *= 2;
                }

                // If the last multiply makes the datum end up going negative, we exit the loop.
                doneInitializing = datum.Data16 >= 0x8000; 
                datum.Data16 *= 2;
            }

            cache7E0740temp[iteration * 2] = datum.Low8;
            cache7E0740temp[iteration * 2 + 1] = datum.High8;

            iteration++;
            numDatumMultiplies--;
        }

        result.ControlFlowSwitch = (iteration - 1) * 2;

        // Zero out the intermediate
        for (int i = 0; i < 0x100; i++)
        {
            result.mem7E0500_7E0700[i] = 0;
        }

        // This is hard coded.
        unsigned short cacheDestIndex = 0;

        unsigned short dictionaryValue = 0xFF;

        // Set bytes in cache
        for (int i=0; i< setBytesInCacheCounter; ++i)
        {
            // Skip the x index past N entries in the cache which are too low, < 0x80.
            // If x gets to go past 255, it wraps back to 0.
            // There are guaranteed to actually be enough low entries.
            unsigned short howManyLowEntriesToSkip = GetStagingBufferDescriptorCount(
                dictionaryValue,
                compressedSource,
                &result.CaseCond, 
                &compressedSourceIndex,
                &byteRepititionCount,
                &swapValueToken,
                &caseKey) + 1;

            while (howManyLowEntriesToSkip > 0)
            {
                ++dictionaryValue;
                dictionaryValue &= 0xFF;
                if (result.mem7E0500_7E0700[dictionaryValue] < 0x80)
                {
                    --howManyLowEntriesToSkip;
                }
            }

            result.mem7E0500_7E0700[dictionaryValue]--;

            // This is running in 8 bit index mode.
            assert(dictionaryValue <= 0xFF);
            dictionaryValues[i] = static_cast<unsigned char>(dictionaryValue);
        }

        unsigned short sourceIndex = 0;
        unsigned char destIndex = 0;

        for (int i=0; i<8; ++i)
        {
            int descriptorCount = stagingBufferDescriptorCounts[i];
            for (int j = 0; j < descriptorCount; ++j)
            {
                unsigned char dictionaryValue = dictionaryValues[sourceIndex];
                unsigned char descriptor = i * 2;

                ++sourceIndex;

                if (dictionaryValue == (compressedDataToken & 0xFF))
                {
                    unsigned short mask = i + 1;

                    compressedDataToken &= 0x00FF; // Keep the lower byte
                    compressedDataToken |= (mask << 8); // Replace the upper byte

                    descriptor = 0x12;
                }

                int pow2table[] = { 128, 64, 32, 16, 8, 4, 2, 1 }; // This could concisely be expressed as a bitshift, but table is more self explanatory.
                int numberOfBytesToWrite = pow2table[i];
                for (int k = 0; k < numberOfBytesToWrite; ++k)
                {
                    result.mem7E0500_7E0700[destIndex] = dictionaryValue;
                    result.mem7E0500_7E0700[0x100 + destIndex] = descriptor;
                    ++destIndex;
                }
            }
        }

        // Always tack a bunch of dummy 0x10 descriptors on at the end.
        int byteCountToSet = 0xFF - destIndex + 1;
        for (int i=0; i< byteCountToSet; ++i) 
        {
            result.mem7E0500_7E0700[destIndex + i] = 0x0;
            result.mem7E0500_7E0700[0x100 + destIndex + i] = 0x10;
        }

        result.CompressedSourceIndex = compressedSourceIndex;
        result.CompressedDataToken = compressedDataToken;
        result.SwapValueToken = swapValueToken;

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

    struct Monstrosity1Result
    {
        std::vector<unsigned char> cache7F0000_decompressedStaging;
        unsigned short CompressedSize;
    };

    Monstrosity1Result Monstrosity1(
        int teamIndex, 
        int playerIndex,
        std::vector<unsigned char> const& compressedSource,
        Monstrosity0Result& result0)
    {
        Monstrosity1Result result{ };
        unsigned char decompressedValue = 0;
        unsigned short decompressedValueCandidate = 0;
        unsigned short compressedSourceIndex = result0.CompressedSourceIndex;
        unsigned short swapValueToken = result0.SwapValueToken;

        unsigned short indirectHigh = 0x007F;
        unsigned short indirectLow = 0;

        unsigned short nextCaseIndex = s_caseTable[0].NextCaseIndices[result0.CaseCond / 2 - 1];

        result.cache7F0000_decompressedStaging.resize(0xFFFF);
        memset(result.cache7F0000_decompressedStaging.data(), 0, result.cache7F0000_decompressedStaging.size());

        unsigned short nextCaseCond{};
        {
            unsigned short localCacheIndex = swapValueToken >> 8;
            nextCaseCond = result0.mem7E0500_7E0700[0x100 + localCacheIndex];
        }

        bool doneDecompression = false;
        while (!doneDecompression)
        {
            unsigned short currentCaseIndex = nextCaseIndex;
            nextCaseIndex = s_caseTable[currentCaseIndex].NextCaseIndices[nextCaseCond / 2 - 1];

            if (nextCaseCond < 0x10)
            {
                unsigned short localCacheIndex = swapValueToken >> 8;

                unsigned short firstMultiplier = s_caseTable[currentCaseIndex].FirstMultipliers[nextCaseCond / 2 - 1];
                unsigned short secondMultiplier = s_caseTable[currentCaseIndex].SecondMultipliers[nextCaseCond / 2 - 1];

                swapValueToken *= firstMultiplier;
                if (secondMultiplier != 0)
                {
                    LoadNextFrom0CInc(compressedSource, &compressedSourceIndex, &swapValueToken);
                    swapValueToken *= secondMultiplier;
                }
                decompressedValueCandidate = LoadNextFrom0500(result0, localCacheIndex, &result.cache7F0000_decompressedStaging, indirectHigh, &indirectLow);
                
                unsigned short localCacheIndex2 = swapValueToken >> 8;
                nextCaseCond = result0.mem7E0500_7E0700[0x100 + localCacheIndex2];

                continue;
            }

            if (nextCaseCond == 0x10)
            {
                // The jump760 case with what was formerly known as switchcase 8.
                nextCaseCond = 0x12 - (currentCaseIndex * 2);
                LoadNextFrom0CMaskAndShift(currentCaseIndex - 1, compressedSource, compressedSourceIndex, &swapValueToken);

                bool shiftHigh = false;
                if (result0.ControlFlowSwitch == 0x12)
                {
                    shiftHigh = swapValueToken >= result0.cache7E0750.Data16;
                }

                unsigned short loadSource = swapValueToken;
                unsigned short countUntilMatch = 0;
                if (shiftHigh)
                {
                    loadSource /= 64;
                    loadSource -= result0.cache7E0730.High16;
                    countUntilMatch = 2;
                }
                else
                {
                    loadSource /= 128;
                    loadSource -= result0.cache7E0730.Low16;
                    countUntilMatch = 1;
                }

                // This is 8 bit acc.
                Mem16 loaded16;
                loaded16.Data16 = dictionaryValues[loadSource];
                if (indirectHigh == 0x7E && indirectLow >= 0x100)
                {
                    dictionaryValues[indirectLow - 0x100] = loaded16.Low8;
                }
                else if (indirectHigh == 0x7F)
                {
                    result.cache7F0000_decompressedStaging[indirectLow] = loaded16.Low8;
                }

                indirectLow += 1;

                decompressedValueCandidate = dictionaryValues[loadSource];
                compressedSourceIndex++;

                {
                    Mem16 mem6b;
                    mem6b.Low8 = shiftedCompressedByte;
                    mem6b.High8 = swapValueToken & 0xFF;
                    swapValueToken = mem6b.Data16;
                }

                for (int iter = 0; iter < 8; iter++)
                {
                    bool foundMatch = false;
                    if (nextCaseCond == caseTable8Entries[iter].Cond)
                    {
                        for (int i = caseTable8Entries[iter].Lower; i < caseTable8Entries[iter].Lower + caseTable8Entries[iter].IterCount; ++i)
                        {
                            swapValueToken *= 2;
                            if (i == 0 || i == 8)
                            {
                                LoadNextFrom0CInc(compressedSource, &compressedSourceIndex, &swapValueToken);
                            }

                            countUntilMatch--;
                            if (countUntilMatch == 0)
                            {
                                unsigned short localCacheIndex = swapValueToken >> 8;
                                nextCaseCond = result0.mem7E0500_7E0700[0x100 + localCacheIndex];
                                nextCaseIndex = (i % 8) + 1;
                                foundMatch = true;
                                break;
                            }
                        }
                        assert(foundMatch);
                        break;
                    }
                }
                continue;
            }
            
            if (nextCaseCond == 0x12)
            {
                // Write output and check if done.
                unsigned short resultCaseCond = 0x12 - (currentCaseIndex * 2);
                unsigned short localCacheIndex = result0.CompressedDataToken / 256;

                AlignedLoad(
                    localCacheIndex,
                    compressedSource,
                    &compressedSourceIndex, 
                    &swapValueToken, 
                    &resultCaseCond);

                unsigned short byteRepititionCount{};

                bool continueDecompression = Decode12(
                    compressedSource,
                    &compressedSourceIndex,
                    &byteRepititionCount,
                    &swapValueToken,
                    &resultCaseCond,
                    &localCacheIndex);
                if (!continueDecompression)
                {
                    doneDecompression = true;
                    break;
                }

                // Write the value, some number of times.
                // These are output shorts.
                assert(decompressedValueCandidate <= 0xFF);
                decompressedValue = static_cast<unsigned char>(decompressedValueCandidate);
                for (int i = 0; i < byteRepititionCount; ++i)
                {
                    if (indirectHigh == 0x7E && indirectLow >= 0x100)
                    {
                        dictionaryValues[indirectLow - 0x100] = decompressedValue;
                    }
                    else if (indirectHigh == 0x7F)
                    {
                        result.cache7F0000_decompressedStaging[indirectLow] = decompressedValue;
                    }
                    else
                    {
                        __debugbreak();
                    }
                    indirectLow += 1;
                }

                nextCaseIndex = s_caseTable[0].NextCaseIndices[resultCaseCond / 2 - 1];
                
                unsigned short localCacheIndex2 = swapValueToken >> 8;
                nextCaseCond = result0.mem7E0500_7E0700[0x100 + localCacheIndex2];

                continue;
            }
        }

        result.CompressedSize = compressedSourceIndex - 1;

        return result;
    }

    struct Fn_80BBB3_DecompressResult
    {
        std::vector<unsigned char> cache7F0000_decompressedStaging;
        unsigned short CompressedSize;
    };

    Fn_80BBB3_DecompressResult Fn_80BBB3_Decompress(int teamIndex, int playerIndex, std::vector<unsigned char> const& compressedSource)
    {
        // This is a sizeable function, a.k.a. 'the monstrosity'.
        //
        // Postconditions:
        //     Decompressed staging data is written to the destination address.
        //

        bool outputStaging = false;
        bool inputStaging = false;
        bool outputShorts = false;

        Monstrosity0Result result0 = Monstrosity0(compressedSource);

        if (inputStaging && teamIndex == 0 && playerIndex == 0)
        {
            result0.mem7E0500_7E0700.clear();

            std::stringstream inDir;
            inDir << "D:\\repos\\nhl94e\\Decompress2\\StageToShorts\\" << GetTeamName((Team)teamIndex) << "_" << playerIndex;
            
            std::stringstream inPath;
            inPath << inDir.str() << "\\staging.hacked.bin";

            FILE* file{};
            fopen_s(&file, inPath.str().c_str(), "rb");
            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            result0.mem7E0500_7E0700.resize(fileSize);
            fread_s(result0.mem7E0500_7E0700.data(), fileSize, 1, fileSize, file);
            fclose(file);
        }

        if (outputStaging && teamIndex == 0 && playerIndex == 0)
        {
            std::stringstream outDir;
            outDir << "D:\\repos\\nhl94e\\Decompress2\\StageToShorts\\" << GetTeamName((Team)teamIndex) << "_" << playerIndex;

            std::stringstream outPath;
            outPath << outDir.str() << "\\staging.bin";

            FILE* file{};
            fopen_s(&file, outPath.str().c_str(), "wb");
            unsigned char const* pData = result0.mem7E0500_7E0700.data();
            fwrite(pData, 1, result0.mem7E0500_7E0700.size(), file);
            fclose(file);
        }

        Monstrosity1Result result1 = Monstrosity1(teamIndex, playerIndex, compressedSource, result0);

        if (outputShorts && teamIndex == 0 && playerIndex == 0)
        {
            std::stringstream outDir;
            outDir << "D:\\repos\\nhl94e\\Decompress2\\StageToShorts\\" << GetTeamName((Team)teamIndex) << "_" << playerIndex;

            std::stringstream outPath;
            outPath << outDir.str() << "\\shorts.hacked.bin";

            FILE* file{};
            fopen_s(&file, outPath.str().c_str(), "wb");
            unsigned char const* pData = result1.cache7F0000_decompressedStaging.data();
            fwrite(pData, 1, result1.cache7F0000_decompressedStaging.size(), file);
            fclose(file);
            exit(0);
        }

        Fn_80BBB3_DecompressResult result{};
        result.cache7F0000_decompressedStaging = result1.cache7F0000_decompressedStaging;
        result.CompressedSize = result1.CompressedSize;

        return result;
    }

    void DecrementCaseKey_ResetCaseKeyAndLoadNext(
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pCompressedSourceIndex,
        unsigned short* pCaseKey,
        unsigned short* pSwapValueToken)
    {
        --(*pCaseKey);
        if (*pCaseKey == 0)
        {
            LoadNextFrom0CInc(compressedSource, pCompressedSourceIndex, pSwapValueToken); // Clobbers. Effectively forgets SwapToken, and uses the next compressed byte instead
            *pCaseKey = 0x8;
        }
    }

    unsigned short GetStagingBufferDescriptorCount(
        unsigned short iter,
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pNextCaseCond, 
        unsigned short* pCompressedSourceIndex,
        unsigned short* pByteRepititionCount,
        unsigned short* pSwapValueToken,
        unsigned short* pCaseKey)
    {
        // Input: mem6c, which is the SwapToken from the compressed data.
        //        y as an index. y is [0..7]
        // 
        // Multiplies mem6c, and/or replaces it with the next compressed byte.
        // Advances mem0c.

        *pByteRepititionCount = 0;
        unsigned short nextSwapValueToken = *pSwapValueToken;
        bool carry = nextSwapValueToken >= 0x8000;
        nextSwapValueToken *= 2;

        DecrementCaseKey_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseKey, &nextSwapValueToken);

        if (carry)
        {
            ShiftRotateDecrementMem6F(pByteRepititionCount, &nextSwapValueToken, &carry);

            DecrementCaseKey_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseKey, &nextSwapValueToken);

            ShiftRotateDecrementMem6F(pByteRepititionCount, &nextSwapValueToken, &carry);

            DecrementCaseKey_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseKey, &nextSwapValueToken);

            *pSwapValueToken = nextSwapValueToken;
            *pNextCaseCond = *pCaseKey * 2;
            return *pByteRepititionCount;
        }

        unsigned short numberOfRotates = 0x2;

        carry = false;
        while (!carry)
        {
            carry = nextSwapValueToken >= 0x8000;
            nextSwapValueToken *= 2;
            DecrementCaseKey_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseKey, &nextSwapValueToken);
            ++numberOfRotates;
        }

        for (int i = 0; i < numberOfRotates; ++i)
        {
            ShiftRotateDecrementMem6F(pByteRepititionCount, &nextSwapValueToken, &carry);
            DecrementCaseKey_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseKey, &nextSwapValueToken);
        }

        *pSwapValueToken = nextSwapValueToken;

        static unsigned short s_ROMValueTable_80C2B6[] = { 0, 0, 0, 0x4, 0xC, 0x1C, 0x3C, 0x7C, 0xFC };
        *pByteRepititionCount += s_ROMValueTable_80C2B6[numberOfRotates];
        *pNextCaseCond = (*pCaseKey) * 2;
        return *pByteRepititionCount;
    }

    void DecrementCaseCond_ResetCaseKeyAndLoadNext(
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pCompressedSourceIndex,
        unsigned short* pCaseCond,
        unsigned short* pSwapValueToken)
    {
        (*pCaseCond) -= 2;
        if (*pCaseCond == 0)
        {
            LoadNextFrom0CInc(compressedSource, pCompressedSourceIndex, pSwapValueToken); // Clobbers. Effectively forgets SwapToken, and uses the next compressed byte instead
            *pCaseCond = 0x10;
        }
    }

    bool Decode12(
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pCompressedSourceIndex,
        unsigned short* pByteRepititionCount,
        unsigned short* pSwapValueToken,
        unsigned short* pCaseCond,
        unsigned short* pCacheIndex) // Returns whether we should continue decompression.
    {
        unsigned short nextSwapValueToken = *pSwapValueToken;
        *pByteRepititionCount = 0;

        bool carry = nextSwapValueToken >= 0x8000;
        nextSwapValueToken *= 2;

        DecrementCaseCond_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseCond, &nextSwapValueToken);

        if (carry)
        {
            ShiftRotateDecrementMem6F(pByteRepititionCount, &nextSwapValueToken, &carry);

            DecrementCaseCond_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseCond, &nextSwapValueToken);

            ShiftRotateDecrementMem6F(pByteRepititionCount, &nextSwapValueToken, &carry);

            DecrementCaseCond_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseCond, &nextSwapValueToken);

            *pSwapValueToken = nextSwapValueToken;
            return *pByteRepititionCount != 0;
        }

        *pCacheIndex = 2;

        carry = false;
        while (!carry)
        {
            carry = nextSwapValueToken >= 0x8000;
            nextSwapValueToken *= 2;

            DecrementCaseCond_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseCond, &nextSwapValueToken);

            (*pCacheIndex)++;
        }

        for (int i = 0; i < *pCacheIndex; ++i)
        {
            ShiftRotateDecrementMem6F(pByteRepititionCount, &nextSwapValueToken, &carry);

            DecrementCaseCond_ResetCaseKeyAndLoadNext(compressedSource, pCompressedSourceIndex, pCaseCond, &nextSwapValueToken);
        }

        *pSwapValueToken = nextSwapValueToken;

        static const unsigned short lookup[] = { 0x4, 0xC, 0x1C, 0x3C, 0x7C };
        int lookupIndex = ((*pCacheIndex) * 2 - 6) / 2;
        *pByteRepititionCount += lookup[lookupIndex];
        return *pByteRepititionCount != 0;
    }

    void AlignedLoad(
        unsigned short cacheIndex,
        std::vector<unsigned char> const& compressedSource,
        unsigned short* pCompressedSourceIndex, 
        unsigned short* pSwapValueToken, 
        unsigned short* pResultCaseCond)
    {
        for (int i = 0; i < cacheIndex; ++i)
        {
            *pSwapValueToken *= 2;
            *pResultCaseCond -= 2;
            if (*pResultCaseCond == 0)
            {
                LoadNextFrom0CInc(compressedSource, pCompressedSourceIndex, pSwapValueToken);
                *pResultCaseCond = 0x10;
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
        dictionaryValues.resize(0x100);
        goldReferenceIndexedColor.resize(0x600);
    }

    void InitializeCaches()
    {
        memset(dictionaryValues.data(), 0, dictionaryValues.size());
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

    void InitializeDecompress(int teamIndex, int playerIndex, unsigned short* pCompressedSourceLocation)
    {
        if (playerIndex < 0 || playerIndex > 5)
            __debugbreak();

        TeamInfo* pTeam = &s_teams[teamIndex];

        SplitBankAndOffset(pTeam->CompressedDataLocations[playerIndex], &dbr, pCompressedSourceLocation);
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

        unsigned short compressedSourceLocation;
        InitializeDecompress(teamIndex, playerIndex, &compressedSourceLocation);
        mem91_HomeOrAway = 2;

        int compressedSize = -1;
        for (int i = 0; i < _countof(s_compressedSizes); ++i)
        {
            if (s_compressedSizes[i].TeamIndex == teamIndex && s_compressedSizes[i].PlayerIndex == playerIndex)
            {
                compressedSize = s_compressedSizes[i].CompressedSize;
                break;
            }
        }
        assert(compressedSize != -1);

        std::vector<unsigned char> compressedSource;
        for (int i = 0; i < compressedSize + 1; ++i)
        {
            unsigned char ch = Load8FromAddress(dbr, compressedSourceLocation + i);
            compressedSource.push_back(ch);
        }

        Fn_80BBB3_DecompressResult decompressedStaging = Fn_80BBB3_Decompress(teamIndex, playerIndex, compressedSource);

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