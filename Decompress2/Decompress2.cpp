// Decompress2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <assert.h>

std::vector<unsigned char> expected;
std::vector<unsigned char> ram;
std::vector<unsigned char> actual;
int validationLength = 2;

std::vector<unsigned char> LoadBinaryFile(char const* fileName)
{
    std::vector<unsigned char> result;

    FILE* file{};
    fopen_s(&file, fileName, "rb");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    result.resize(length);
    fseek(file, 0, SEEK_SET);
    fread(result.data(), 1, length, file);
    fclose(file);

    return result;
}

unsigned char LoadFromRAM(int address)
{
    if (address < 0x7F0000 || address > 0x7F0000 + ram.size())
    {
        __debugbreak();
        return 0xFF;
    }

    return ram[address - 0x7F0000];
}

void WriteOutput(int address, unsigned short output)
{
    if (address < 0x7F5100 || address > 0x7F7390)
    {
        __debugbreak();
        return;
    }

    int outputOffset = address - 0x7F5100;
    assert(outputOffset % 2 == 0);

    int outputElement = outputOffset / 2;

    actual[outputElement] = output;
}

int main()
{
    expected = LoadBinaryFile("expected.bin");
    ram = LoadBinaryFile("ram.bin");
    actual.resize(0x2290 / 2);
    memset(actual.data(), 0, 0x2290);

    // The profile images are loaded in "backwards" order. The earliest memory offset one gets loaded last.
    // The output is saved out 2 bytes at a time.

    int pointer0C = 0x7F0000;
    int pointer10 = 0x7F5100;   // For output
    unsigned short mem04 = 1;
    unsigned short mem06 = 0;
    unsigned short mem14 = 0;
    unsigned short mem16 = 0;
    unsigned short a = 0x0;
    unsigned short x = 0x80;
    unsigned short y = 0x10;

label_85ED:
    // $9B/85ED 64 16       STZ $16    [$00:0016]   A:0120 X:0080 Y:0000 P:envmxdizc
    mem16 = 0;

    // $9B/85EF A5 04       LDA $04    [$00:0004]   A:0000 X:0080 Y:0010 P:envmxdizc
    // $9B/85F1 0A          ASL A                   A:0001 X:0080 Y:0010 P:envmxdizc
    // $9B/85F2 0A          ASL A                   A:0002 X:0080 Y:0010 P:envmxdizc
    // $9B/85F3 A8          TAY                     A:0004 X:0080 Y:0010 P:envmxdizc
    // $9B/85F4 B1 0C       LDA ($0C),y[$7F:0004]   A:0004 X:0080 Y:0004 P:envmxdizc
    unsigned short element = mem04;
    unsigned short byteOffset = element * 4;
    y = byteOffset;
    int pointerLocation = pointer0C + byteOffset;
    unsigned short token0 = LoadFromRAM(pointerLocation);


    // $9B/85F6 F0 E4       BEQ $E4    [$85DC]      A:0000 X:0080 Y:0004 P:envmxdiZc
    if (token0 == 0)
    {
label_85DC:
        // $9B/85DC 8A          TXA                     A:0000 X:0080 Y:0004 P:envmxdiZc
        // $9B/85DD 4A          LSR A                   A:0080 X:0080 Y:0004 P:envmxdizc
        // $9B/85DE 4A          LSR A                   A:0040 X:0080 Y:0004 P:envmxdizc
        // $9B/85DF 4A          LSR A                   A:0020 X:0080 Y:0004 P:envmxdizc
        // $9B/85E0 4A          LSR A                   A:0010 X:0080 Y:0004 P:envmxdizc
        unsigned short temp = x / 16;

        // $9B/85E1 F0 54       BEQ $54    [$8637]      A:0008 X:0080 Y:0004 P:envmxdizc
        if (temp == 0)
        {
            // $9B/8637 A5 06       LDA $06    [$00:0006]   A:0000 X:0008 Y:0006 P:envmxdiZC
            // $9B/8639 1A          INC A                   A:0000 X:0008 Y:0006 P:envmxdiZC
            // $9B/863A 1A          INC A                   A:0001 X:0008 Y:0006 P:envmxdizC
            // $9B/863B A8          TAY                     A:0002 X:0008 Y:0006 P:envmxdizC
            y = mem06 + 2;

            // $9B/863C 29 10 00    AND #$0010              A:0002 X:0008 Y:0002 P:envmxdizC
            unsigned short check = y & 0x10;
            if (check == 0)
            {
                // $9B/8647 84 06       STY $06    [$00:0006]   A:0000 X:0008 Y:0002 P:envmxdiZC
                // $9B/8649 A5 16       LDA $16    [$00:0016]   A:0000 X:0008 Y:0002 P:envmxdiZC
                // $9B/864B 91 10       STA ($10),y[$7F:5102]   A:0000 X:0008 Y:0002 P:envmxdiZC
                mem06 = y;
                WriteOutput(pointer10 + y, mem16);

                // $9B/864D 98          TYA                     A:0000 X:0008 Y:0002 P:envmxdiZC
                // $9B/864E 18          CLC                     A:0002 X:0008 Y:0002 P:envmxdizC
                // $9B/864F 69 10 00    ADC #$0010              A:0002 X:0008 Y:0002 P:envmxdizc
                // $9B/8652 A8          TAY                     A:0012 X:0008 Y:0002 P:envmxdizc
                // $9B/8653 A5 14       LDA $14    [$00:0014]   A:0012 X:0008 Y:0012 P:envmxdizc
                // $9B/8655 91 10       STA ($10),y[$7F:5112]   A:0000 X:0008 Y:0012 P:envmxdiZc
                y += 0x10;
                WriteOutput(pointer10 + y, mem14);


            }
            else
            {
                __debugbreak(); // Not impl
            }
        }
        else
        {
            // $9B/85E3 AA          TAX                     A:0008 X:0080 Y:0004 P:envmxdizc
            // $9B/85E4 C8          INY                     A:0008 X:0008 Y:0004 P:envmxdizc
            // $9B/85E5 C8          INY                     A:0008 X:0008 Y:0005 P:envmxdizc
            // $9B/85E6 80 0C       BRA $0C    [$85F4]      A:0008 X:0008 Y:0006 P:envmxdizc
            x = temp;
            y += 2;
            goto label_85F4;
        }
    }
    else
    {
        __debugbreak(); // Not impl
    }

label_85F4:
    // $9B/85F4 B1 0C       LDA ($0C),y[$7F:0006]   A:0008 X:0008 Y:0006 P:envmxdizc
    unsigned short token1 = LoadFromRAM(pointer0C + y);

    // $9B/85F6 F0 E4       BEQ $E4    [$85DC]      A:0000 X:0008 Y:0006 P:envmxdiZc
    if (token1 == 0)
    {
        goto label_85DC;
    }
    else
    {
        __debugbreak(); // Not impl
    }

    for (int i = 0; i < 2; ++i)
    {


        {
        }


        // $9B/8649 A5 16       LDA $16    [$00:0016]   A:0000 X:0008 Y:0000 P:envmxdiZC
        // $9B/864B 91 10       STA ($10),y[$7F:5100]   A:0000 X:0008 Y:0000 P:envmxdiZC
        actual.push_back(mem16);
    }

    // $9B/864B 91 10       STA ($10),y[$7F:5102]   A:0000 X:0008 Y:0002 P:envmxdiZC

    if (actual.size() < validationLength)
    {
        __debugbreak();
        return -1;
    }

    for (int i = 0; i < validationLength; ++i)
    {
        if (expected[i] != actual[i])
        {
            __debugbreak();
            return -1;
        }
    }

    return 0;
}