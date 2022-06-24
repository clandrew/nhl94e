// Decompress2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <assert.h>

std::vector<unsigned char> expected;
std::vector<unsigned char> ram;
std::vector<unsigned short> actual;
std::ofstream debugLog;
int validationLength = 2;
int instructionLimit = 1000;
int printedInstructionCount = 0;

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

void DebugPrintRegs(unsigned short a, unsigned short x, unsigned short y, bool zero, bool carry)
{
    debugLog << " A:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << a;
    debugLog << " X:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << x;
    debugLog << " Y:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << y;
    debugLog << " P:envmxdi";
    debugLog << (zero ? "Z" : "z");
    debugLog << (carry ? "C" : "c");
}

void DebugPrintFinalize()
{
    debugLog << "\n";
    debugLog.flush();

    ++printedInstructionCount;

    if (printedInstructionCount > instructionLimit)
    {
        exit(0);
    }
}

void DebugPrint(const char* asmText, unsigned short a, unsigned short x, unsigned short y, bool zero, bool carry)
{
    debugLog << asmText;
    DebugPrintRegs(a, x, y, zero, carry);
    DebugPrintFinalize();
}

void DebugPrint85F4(unsigned short a, unsigned short x, unsigned short y, bool zero, bool carry)
{
    debugLog << "$9B/85F4 B1 0C       LDA ($0C),y[$7F:";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << y;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y, zero, carry);
    DebugPrintFinalize();
}

void DebugPrint864B(unsigned short a, unsigned short x, unsigned short y, bool zero, bool carry)
{
    debugLog << "$9B/864B 91 10       STA ($10),y[$7F:";
    unsigned short offset = 0x5100 + y;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << offset;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y, zero, carry);
    DebugPrintFinalize();
}

void DebugPrint8655(unsigned short a, unsigned short x, unsigned short y, bool zero, bool carry)
{
    debugLog << "$9B/8655 91 10       STA ($10),y[$7F:";
    unsigned short offset = 0x5100 + y;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << offset;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y, zero, carry);
    DebugPrintFinalize();
}

int main()
{
    debugLog.open("output.txt", std::ofstream::out | std::ofstream::trunc);

    expected = LoadBinaryFile("expected.bin");
    ram = LoadBinaryFile("ram.bin");
    actual.resize(0x2290 / 2);
    memset(actual.data(), 0, 0x2290);

    // The profile images are loaded in "backwards" order. The earliest memory offset one gets loaded last.
    // The output is saved out 2 bytes at a time.

    int pointer0C = 0x7F0000;
    int pointer10 = 0x7F5100;   // For output
    unsigned short mem00 = 0;
    unsigned short mem04 = 1;
    unsigned short mem06 = 0;
    unsigned short mem14 = 0;
    unsigned short mem16 = 0;
    unsigned short a = 0x0;
    unsigned short x = 0x80;
    unsigned short y = 0x10;
    bool z = false;
    bool c = false;
    bool willCarry = false;

goto label_85ED;

label_85DC:
    // $9B/85DC 8A          TXA                     A:0000 X:0080 Y:0004 P:envmxdiZc
    // $9B/85DD 4A          LSR A                   A:0080 X:0080 Y:0004 P:envmxdizc
    // $9B/85DE 4A          LSR A                   A:0040 X:0080 Y:0004 P:envmxdizc
    // $9B/85DF 4A          LSR A                   A:0020 X:0080 Y:0004 P:envmxdizc
    // $9B/85E0 4A          LSR A                   A:0010 X:0080 Y:0004 P:envmxdizc
    DebugPrint("$9B/85DC 8A          TXA                    ", 0, x, y, z, c);
    willCarry = x < 16;
    a = x;
    z = a == 0;

    DebugPrint("$9B/85DD 4A          LSR A                  ", a, x, y, z, c);
    a >>= 1;

    DebugPrint("$9B/85DE 4A          LSR A                  ", a, x, y, z, c);
    a >>= 1;

    DebugPrint("$9B/85DF 4A          LSR A                  ", a, x, y, z, c);
    a >>= 1;

    DebugPrint("$9B/85E0 4A          LSR A                  ", a, x, y, z, c);
    a >>= 1;

    // $9B/85E1 F0 54       BEQ $54    [$8637]      A:0008 X:0080 Y:0004 P:envmxdizc
    z = a == 0;
    c = willCarry;
    DebugPrint("$9B/85E1 F0 54       BEQ $54    [$8637]     ", a, x, y, z, c);

    if (x / 16 == 0)
    {
        goto label_8637;
    }

    // $9B/85E3 AA          TAX                     A:0008 X:0080 Y:0004 P:envmxdizc
    // $9B/85E4 C8          INY                     A:0008 X:0008 Y:0004 P:envmxdizc
    // $9B/85E5 C8          INY                     A:0008 X:0008 Y:0005 P:envmxdizc
    // $9B/85E6 80 0C       BRA $0C    [$85F4]      A:0008 X:0008 Y:0006 P:envmxdizc

    DebugPrint("$9B/85E3 AA          TAX                    ", a, x, y, z, c);
    x = a;

    DebugPrint("$9B/85E4 C8          INY                    ", a, x, y, z, c);
    y++;

    DebugPrint("$9B/85E5 C8          INY                    ", a, x, y, z, c);
    y++;
    
    DebugPrint("$9B/85E6 80 0C       BRA $0C    [$85F4]     ", a, x, y, z, c);

    goto label_85F4;

label_85E8:
    // $9B/85E8 A2 80 00    LDX #$0080              A:0000 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/85E8 A2 80 00    LDX #$0080             ", a, x, y, z, c);
    x = 0x80;

    // $9B/85EB 64 14       STZ $14    [$00:0014]   A:0000 X:0080 Y:0012 P:envmxdizc
    DebugPrint("$9B/85EB 64 14       STZ $14    [$00:0014]  ", a, x, y, z, c);
    mem14 = 0;

label_85ED:
    // $9B/85ED 64 16       STZ $16    [$00:0016]   A:0120 X:0080 Y:0000 P:envmxdizc
    DebugPrint("$9B/85ED 64 16       STZ $16    [$00:0016]  ", 0, x, y, z, c);
    mem16 = 0;
    {
        // $9B/85EF A5 04       LDA $04    [$00:0004]   A:0000 X:0080 Y:0010 P:envmxdizc
        // $9B/85F1 0A          ASL A                   A:0001 X:0080 Y:0010 P:envmxdizc
        // $9B/85F2 0A          ASL A                   A:0002 X:0080 Y:0010 P:envmxdizc
        // $9B/85F3 A8          TAY                     A:0004 X:0080 Y:0010 P:envmxdizc
        DebugPrint("$9B/85EF A5 04       LDA $04    [$00:0004]  ", 0, x, y, z, c);
        a = mem04;

        DebugPrint("$9B/85F1 0A          ASL A                  ", a, x, y, z, c);
        a *= 2;

        DebugPrint("$9B/85F2 0A          ASL A                  ", a, x, y, z, c);
        a *= 2;

        DebugPrint("$9B/85F3 A8          TAY                    ", mem04*4, x, y, z, c);
        y = a;
    }

label_85F4:
    {
        // $9B/85F4 B1 0C       LDA ($0C),y[$7F:0006]   A:0008 X:0008 Y:0006 P:envmxdizc
        DebugPrint85F4(a, x, y, z, c);
        unsigned short token1 = LoadFromRAM(pointer0C + y);
        a = token1;
        z = token1 == 0;

        // $9B/85F6 F0 E4       BEQ $E4    [$85DC]      A:0000 X:0008 Y:0006 P:envmxdiZc
        DebugPrint("$9B/85F6 F0 E4       BEQ $E4    [$85DC]     ", a, x, y, z, c);
        if (token1 == 0)
        {
            goto label_85DC;
        }
    }

    // $9B/85F8 EB          XBA                     A:0800 X:0008 Y:003E P:envmxdizc
    DebugPrint("$9B/85F8 EB          XBA                    ", a, x, y, z, c);
    {
        unsigned short temp = a & 0xFF;
        a <<= 8;
        a |= temp;
    }

    // $9B/85F9 A8          TAY                     A:0008 X:0008 Y:003E P:envmxdizc
    DebugPrint("$9B/85F9 A8          TAY                    ", a, x, y, z, c);
    y = a;

label_85FA:
    // $9B/85FA 98          TYA                     A:0008 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/85FA 98          TYA                    ", a, x, y, z, c);
    a = y;

    // $9B/85FB 0A          ASL A                   A:0008 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/85FB 0A          ASL A                  ", a, x, y, z, c);
    a *= 2;

    // $9B/85FC 90 05       BCC $05    [$8603]      A:0010 X:0008 Y:0008 P:envmxdizc
    if (c == false)
    {
        goto label_8603;
    }

label_8603:
    // $9B/8603 0A          ASL A                   A:0010 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/8603 0A          ASL A                  ", a, x, y, z, c);
    a *= 2;

    // $9B/8604 90 05       BCC $05    [$860B]      A:0020 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/8604 90 05       BCC $05    [$860B]     ", a, x, y, z, c);
    if (c == false)
    {
        goto label_860B;
    }

label_860B:
    // $9B/860B 0A          ASL A                   A:0010 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/860B 0A          ASL A                  ", a, x, y, z, c);
    a *= 2;

    // $9B/860C 90 05       BCC $05    [$8613]      A:0040 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/860C 90 05       BCC $05    [$8613]     ", a, x, y, z, c);
    if (c == false)
    {
        goto label_8613;
    }

label_8613:
    // $9B/8613 0A          ASL A                   A:0040 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/8613 0A          ASL A                  ", a, x, y, z, c);
    a *= 2;

    // $9B/8614 90 05       BCC $05    [$861B]      A:0080 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/8614 90 05       BCC $05    [$861B]     ", a, x, y, z, c);
    if (c == false)
    {
        goto label_861B;
    }

label_861B:
    // $9B/861B A8          TAY                     A:0080 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/861B A8          TAY                    ", a, x, y, z, c);
    y = a;

    // $9B/861C 8A          TXA                     A:0080 X:0008 Y:0080 P:envmxdizc
    DebugPrint("$9B/861C 8A          TXA                    ", a, x, y, z, c);
    a = x;

    // $9B/861D 4A          LSR A                   A:0008 X:0008 Y:0080 P:envmxdizc
    DebugPrint("$9B/861D 4A          LSR A                  ", a, x, y, z, c);
    a >>= 1;

    // $9B/861E AA          TAX                     A:0004 X:0008 Y:0080 P:envmxdizc
    DebugPrint("$9B/861E AA          TAX                    ", a, x, y, z, c);
    x = a;

    // $9B/861F E0 10 00    CPX #$0010              A:0004 X:0004 Y:0080 P:envmxdizc
    DebugPrint("$9B/861F E0 10 00    CPX #$0010             ", a, x, y, z, c);
    c = x >= 0x10;

    // $9B/8622 B0 D6       BCS $D6    [$85FA]      A:0004 X:0004 Y:0080 P:eNvmxdizc
    DebugPrint("$9B/8622 B0 D6       BCS $D6    [$85FA]     ", a, x, y, z, c);
    if (c)
    {
        goto label_85FA;
    }

    // $9B/8624 E0 00 00    CPX #$0000              A:0004 X:0004 Y:0080 P:eNvmxdizc
    DebugPrint("$9B/8624 E0 00 00    CPX #$0000             ", a, x, y, z, c);
    z = x == 0;

    // $9B/8627 F0 0E       BEQ $0E    [$8637]      A:0004 X:0004 Y:0080 P:envmxdizC
    DebugPrint("$9B/8627 F0 0E       BEQ $0E    [$8637]     ", a, x, y, z, c);
    if (z)
    {
        goto label_8637;
    }

    // $9B/8629 E0 08 00    CPX #$0008              A:0004 X:0004 Y:0080 P:envmxdizC
    DebugPrint("$9B/8629 E0 08 00    CPX #$0008             ", a, x, y, z, c);
    c = x >= 0x8;

    // $9B/862C 90 CC       BCC $CC    [$85FA]      A:0004 X:0004 Y:0080 P:eNvmxdizc
    DebugPrint("$9B/862C 90 CC       BCC $CC    [$85FA]     ", a, x, y, z, c);
    if (c == false)
    {
        goto label_85FA;
    }

    __debugbreak(); // not impl


label_8637:
    // $9B/8637 A5 06       LDA $06    [$00:0006]   A:0000 X:0008 Y:0006 P:envmxdiZC
    DebugPrint("$9B/8637 A5 06       LDA $06    [$00:0006]  ", a, x, y, z, c);
    a = mem06;
    z = a == 0;

    // $9B/8639 1A          INC A                   A:0000 X:0008 Y:0006 P:envmxdiZC
    DebugPrint("$9B/8639 1A          INC A                  ", a, x, y, z, c);
    ++a;
    z = a == 0;

    // $9B/863A 1A          INC A                   A:0001 X:0008 Y:0006 P:envmxdizC
    DebugPrint("$9B/863A 1A          INC A                  ", a, x, y, z, c);
    ++a;
    z = a == 0;

    // $9B/863B A8          TAY                     A:0002 X:0008 Y:0006 P:envmxdizC
    DebugPrint("$9B/863B A8          TAY                    ", a, x, y, z, c);
    y = a;

    y = mem06 + 2;

    // $9B/863C 29 10 00    AND #$0010              A:0002 X:0008 Y:0002 P:envmxdizC
    DebugPrint("$9B/863C 29 10 00    AND #$0010             ", a, x, y, z, c);
    a = a & 0x10;
    z = a == 0;

    unsigned short check = y & 0x10;

    // $9B/863F F0 06       BEQ $06    [$8647]      A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/863F F0 06       BEQ $06    [$8647]     ", a, x, y, z, c);

    if (check == 0)
    {
        goto label_8647;
    }

    // $9B/8641 98          TYA                     A:0010 X:0008 Y:0010 P:envmxdizC
    DebugPrint("$9B/8641 98          TYA                    ", a, x, y, z, c);
    a = y;

    // $9B/8642 18          CLC                     A:0010 X:0008 Y:0010 P:envmxdizC
    DebugPrint("$9B/8642 18          CLC                    ", a, x, y, z, c);
    c = false;

    // $9B/8643 69 10 00    ADC #$0010              A:0010 X:0008 Y:0010 P:envmxdizc
    DebugPrint("$9B/8643 69 10 00    ADC #$0010             ", a, x, y, z, c);
    a += 0x10;

    // $9B/8646 A8          TAY                     A:0020 X:0008 Y:0010 P:envmxdizc
    DebugPrint("$9B/8646 A8          TAY                    ", a, x, y, z, c);
    y = a;

label_8647:
    // $9B/8647 84 06       STY $06    [$00:0006]   A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/8647 84 06       STY $06    [$00:0006]  ", a, x, y, z, c);
    mem06 = y;

    // $9B/8649 A5 16       LDA $16    [$00:0016]   A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/8649 A5 16       LDA $16    [$00:0016]  ", a, x, y, z, c);
    a = mem16;
    z = a == 0;

    // $9B/864B 91 10       STA ($10),y[$7F:5102]   A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint864B(a, x, y, z, c);

    WriteOutput(pointer10 + y, mem16);

    // $9B/864D 98          TYA                     A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/864D 98          TYA                    ", a, x, y, z, c);
    a = y;
    z = a == 0;

    // $9B/864E 18          CLC                     A:0002 X:0008 Y:0002 P:envmxdizC
    DebugPrint("$9B/864E 18          CLC                    ", a, x, y, z, c);
    c = false;

    // $9B/864F 69 10 00    ADC #$0010              A:0002 X:0008 Y:0002 P:envmxdizc
    DebugPrint("$9B/864F 69 10 00    ADC #$0010             ", a, x, y, z, c);
    a += 0x10;

    // $9B/8652 A8          TAY                     A:0012 X:0008 Y:0002 P:envmxdizc
    DebugPrint("$9B/8652 A8          TAY                    ", a, x, y, z, c);
    y = a;

    // $9B/8653 A5 14       LDA $14    [$00:0014]   A:0012 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/8653 A5 14       LDA $14    [$00:0014]  ", a, x, y, z, c);
    a = mem14;
    z = a == 0;
    c = false;

    // $9B/8655 91 10       STA ($10),y[$7F:5112]   A:0000 X:0008 Y:0012 P:envmxdiZc
    DebugPrint8655(a, x, y, z, c);

    WriteOutput(pointer10 + y, mem14);

    // $9B/8657 E6 04       INC $04    [$00:0004]   A:0000 X:0008 Y:0012 P:envmxdiZc
    DebugPrint("$9B/8657 E6 04       INC $04    [$00:0004]  ", a, x, y, z, c);
    mem04++;
    z = mem04 == 0;

    // $9B/8659 C6 00       DEC $00    [$00:0000]   A:0000 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/8659 C6 00       DEC $00    [$00:0000]  ", a, x, y, z, c);
    mem00--;

    // $9B/865B 10 8B       BPL $8B    [$85E8]      A:0000 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/865B 10 8B       BPL $8B    [$85E8]     ", a, x, y, z, c);

    if (mem00 > 0)
    {
        goto label_85E8;
    }

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