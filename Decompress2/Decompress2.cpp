// Decompress2.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 19168 / 30876

#include <vector>
#include <fstream>
#include "Util.h"

void Fn_80C1B0();
void Fn_80C232();
void Fn_80C2DC();

unsigned short a = 0xFB30;
unsigned short x = 0x0480;
unsigned short y = 0xF8AE;
bool n = false;
bool z = false;
bool c = false;

union Mem16
{
    unsigned short Data16;
    struct
    {
        unsigned char Low8;
        unsigned char High8;
    };
};

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

// Output.
std::vector<unsigned char> mem7E0500_7E0700;

unsigned short indirectHigh;
unsigned short indirectLow;

std::vector<unsigned char> cache7F0000;
std::vector<unsigned char> cache7E0100;
std::vector<unsigned char> cache7E0700;
std::vector<unsigned char> cache7E0720;
std::vector<unsigned char> cache7E0740;
std::vector<unsigned char> cache7E0760;

// TODO: Need to represent 7Fxxxx.

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

unsigned short LoadFromROM99F8B1(int address)
{
    if (address < 0x99F8B1 || address > 0x99FAB4)
    {
        __debugbreak();
        return 0xFF;
    }

    int offs = address - 0x99F8B1;

    unsigned char ch0 = romFile[0xCF8B1 + offs];
    unsigned char ch1 = romFile[0xCF8B1 + offs + 1];       

    unsigned short result = (ch1 << 8) | ch0;
    return result;
}

void LoadNextFrom0CInc(unsigned short pc)
{
    // $80/BD13 E2 20       SEP #$20                A:9400 X:0002 Y:0025 P:envmxdizc 
    DebugPrintWithPC(pc, "E2 20       SEP #$20               ", a, x, y); 
    pc += 2;

    // $80/BD15 B2 0C       LDA ($0C)  [$99:F8FA]   A:9400 X:0002 Y:0025 P:envmxdizc
    DebugPrintWithPCAndIndex(pc, "B2 0C       LDA ($0C)  [$99:", mem0c, a, x, y);
    loaded = LoadFromROM99F8B1(0x990000 | mem0c);
    a &= 0xFF00;
    a |= loaded & 0xFF;
    pc += 2;

    // $80/BD17 C2 20       REP #$20                A:948C X:0002 Y:0025 P:envmxdizc
    DebugPrintWithPC(pc, "C2 20       REP #$20               ", a, x, y); // 16-bit acc
    pc += 2;

    // $80/BD19 E6 0C       INC $0C    [$00:000C]   A:948C X:0002 Y:0025 P:envmxdizc
    DebugPrintWithPC(pc, "E6 0C       INC $0C    [$00:000C]  ", a, x, y);
    mem0c++;
}

void LoadNextFrom0500(unsigned short pc)
{
    // $80/BD1B BE 00 05    LDX $0500,y[$99:0525]   A:948C X:0002 Y:0025 P:envmxdizc
    DebugPrintWithPCAndIndex(pc, "BE 00 05    LDX $0500,y[$99:", 0x500 + y, a, x, y);
    x = mem7E0500_7E0700[y];
    pc += 3;

    // $80/BD1E 8E 80 21    STX $2180  [$99:2180]   A:948C X:0000 Y:0025 P:envmxdizc
    DebugPrintWithPC(pc, "8E 80 21    STX $2180  [$99:2180]  ", a, x, y);
    loaded16.Data16 = x;
    if (indirectHigh == 0x7E && indirectLow >= 0x100)
    {
        cache7E0100[indirectLow - 0x100] = loaded16.Low8;
        cache7E0100[indirectLow + 1 - 0x100] = loaded16.High8;
    }
    else if (indirectHigh == 0x7F)
    {
        cache7F0000[indirectLow] = loaded16.Low8;
        cache7F0000[indirectLow+1] = loaded16.High8;
    }
    else
    {
        __debugbreak();
    }
    indirectLow += 2;
    pc += 3;

    // $80/BD21 86 08       STX $08    [$00:0008]   A:948C X:0000 Y:0025 P:envmxdizc
    DebugPrintWithPC(pc, "86 08       STX $08    [$00:0008]  ", a, x, y);
    mem08 = x;
}

void LoadNextFrom0600(unsigned short pc)
{
    // $80/BE5D 85 6C       STA $6C    [$00:006C]   A:9420 X:0000 Y:0025 P:envmxdizc
    DebugPrintWithPC(pc, "85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;
    pc += 2;

    // $80/BE5F A4 6D       LDY $6D    [$00:006D]   A:9420 X:0000 Y:0025 P:envmxdizc
    DebugPrintWithPC(pc, "A4 6D       LDY $6D    [$00:006D]  ", a, x, y);
    y = mem6c >> 8;
    pc += 2;

    // $80/BE61 BE 00 06    LDX $0600,y[$99:0694]   A:9420 X:0000 Y:0094 P:envmxdizc
    DebugPrintWithPCAndIndex(pc, "BE 00 06    LDX $0600,y[$99:", 0x600 + y, a, x, y);
    x = mem7E0500_7E0700[0x100 + y];
}

void LoadNextFrom0CMaskAndShift(unsigned short pc, unsigned char nextX, int shifts)
{
    DebugPrintWithPCAndImm8(pc, "A2", "LDX", nextX, a, x, y);
    x = nextX;
    pc += 2;

    // $80/BED3 64 6A       STZ $6A    [$00:006A]   A:F180 X:0006 Y:00F1 P:envmxdizc
    DebugPrintWithPC(pc, "64 6A       STZ $6A    [$00:006A]  ", a, x, y);
    mem6a = 0;
    pc += 2;

    // $80/BED5 B2 0C       LDA ($0C)  [$99:F8F9]   A:F180 X:0006 Y:00F1 P:envmxdizc
    DebugPrintWithPCAndIndex(pc, "B2 0C       LDA ($0C)  [$99:", mem0c, a, x, y);
    a = LoadFromROM99F8B1(0x990000 | mem0c);
    pc += 2;

    // $80/BED7 29 FF 00    AND #$00FF              A:8C94 X:0006 Y:00F1 P:envmxdizc
    DebugPrintWithPC(pc, "29 FF 00    AND #$00FF             ", a, x, y);
    a &= 0xFF;
    pc += 3;

    for (int i = 0; i < shifts; ++i)
    {
        DebugPrintWithPC(pc, "0A          ASL A                  ", a, x, y);
        a *= 2;
        pc++;
    }

    DebugPrintWithPC(pc, "05 6B       ORA $6B    [$00:006B]  ", a, x, y);
    loaded16 = LoadMem6b();
    a |= loaded16.Data16;
    pc += 2;

    DebugPrintWithPC(pc, "85 6B       STA $6B    [$00:006B]  ", a, x, y);
    loaded16.Data16 = a;
    SaveMem6b(loaded16);
    pc += 2;

    DebugPrintWithPC(pc, "A5 6C       LDA $6C    [$00:006C]  ", a, x, y);
    a = mem6c;
}

void ShiftThenLoad100ThenCompare(unsigned short pc, int shifts, int subtractDataAddress, int nextY)
{
    for (int i = 0; i < shifts; ++i)
    {
        // $80/BF8F 4A          LSR A                   A:F192 X:0006 Y:00F1 P:envmxdizc
        DebugPrintWithPC(pc, "4A          LSR A                  ", a, x, y);
        a >>= 1;
        ++pc;
    }

    // $80/BF96 38          SEC                     A:01E3 X:0006 Y:00F1 P:envmxdizc
    DebugPrintWithPC(pc, "38          SEC                    ", a, x, y);
    ++pc;

    if (subtractDataAddress == 0x0730)
    {
        DebugPrintWithPC(pc, "ED 30 07    SBC $0730  [$99:0730]  ", a, x, y);
        loaded16.Low8 = cache7E0720[0x10];
        loaded16.High8 = cache7E0720[0x11];
    }
    else if (subtractDataAddress == 0x732)
    {
        DebugPrintWithPC(pc, "ED 32 07    SBC $0732  [$99:0732]  ", a, x, y);
        loaded16.Low8 = cache7E0720[0x12];
        loaded16.High8 = cache7E0720[0x13];
    }
    else
    {
        __debugbreak(); // notimpl
    }
    a -= loaded16.Data16;
    pc += 3;

    // $80/BF9A A8          TAY                     A:003C X:0006 Y:00F1 P:envmxdizc
    DebugPrintWithPC(pc, "A8          TAY                    ", a, x, y);
    y = a;
    ++pc;

    // $80/BF9B E2 20       SEP #$20                A:003C X:0006 Y:003C P:envmxdizc
    DebugPrintWithPC(pc, "E2 20       SEP #$20               ", a, x, y);
    pc += 2;

    // $80/BF9D B9 00 01    LDA $0100,y[$99:013C]   A:003C X:0006 Y:003C P:envmxdizc
    DebugPrintWithPCAndIndex(pc, "B9 00 01    LDA $0100,y[$99:", 0x100 + y, a, x, y);
    a = cache7E0100[y];
    pc += 3;

    if (nextY == 1)
    {
        DebugPrintWithPC(pc, "A0 01       LDY #$01               ", a, x, y);
    }
    else if (nextY == 2)
    {
        DebugPrintWithPC(pc, "A0 02       LDY #$02               ", a, x, y);
    }
    else
    {
        __debugbreak();
    }
    y = nextY;
    pc += 2;

    // $80/BFA2 C5 73       CMP $73    [$00:0073]   A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrintWithPC(pc, "C5 73       CMP $73    [$00:0073]  ", a, x, y);
    z = a == (mem73 & 0xFF); // we are in 8bit mode
}

void ShiftRotateDecrement(unsigned short pc, int xDecAmt, int yDecAmt)
{
    DebugPrintWithPC(pc, "0A          ASL A                  ", a, x, y);
    c = a >= 0x8000;
    a *= 2;
    pc++;

    DebugPrintWithPC(pc, "26 6F       ROL $6F    [$00:006F]  ", a, x, y);
    RotateLeft(&mem6f, &c);
    pc += 2;

    for (int i = 0; i < xDecAmt; ++i)
    {
        DebugPrintWithPC(pc, "CA          DEX                    ", a, x, y);
        --x;
        z = x == 0;
        ++pc;
    }

    for (int i = 0; i < yDecAmt; ++i)
    {
        DebugPrintWithPC(pc, "88          DEY                    ", a, x, y);
        --y;
        z = y == 0;
        ++pc;
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

    // Use 8bit X and Y
    DebugPrint("$80/BBB3 E2 10       SEP #$10               ", a, x, y);
    x &= 0xFF;
    y &= 0xFF;
    
    DebugPrint("$80/BBB5 A5 0C       LDA $0C    [$00:000C]  ", a, x, y);
    a = mem0c;

    DebugPrint("$80/BBB7 18          CLC                    ", a, x, y);
    c = false;

    DebugPrint("$80/BBB8 69 05 00    ADC #$0005             ", a, x, y);
    a += 5;

    DebugPrint("$80/BBBB 85 0C       STA $0C    [$00:000C]  ", a, x, y);
    mem0c = a;

    DebugPrint("$80/BBBD B2 0C       LDA ($0C)  [$99:F8B1]  ", a, x, y);
    a = LoadFromROM99F8B1(0x990000 | mem0c);

    // $80/BBBF 85 73       STA $73    [$00:0073]   A:960F X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBBF 85 73       STA $73    [$00:0073]  ", a, x, y);
    mem73 = a;

    // $80/BBC1 E6 0C       INC $0C    [$00:000C]   A:960F X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC1 E6 0C       INC $0C    [$00:000C]  ", a, x, y);
    mem0c++;

    // $80/BBC3 B2 0C       LDA ($0C)  [$99:F8B2]   A:960F X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC3 B2 0C       LDA ($0C)  [$99:F8B2]  ", a, x, y);
    a = LoadFromROM99F8B1(0x990000 | mem0c);

    // $80/BBC5 E6 0C       INC $0C    [$00:000C]   A:6596 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC5 E6 0C       INC $0C    [$00:000C]  ", a, x, y);
    mem0c++;

    // $80/BBC7 E6 0C       INC $0C    [$00:000C]   A:6596 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC7 E6 0C       INC $0C    [$00:000C]  ", a, x, y);
    mem0c++;

    // $80/BBC9 EB          XBA                     A:6596 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC9 EB          XBA                    ", a, x, y);
    a = ExchangeShortHighAndLow(a);

    // $80/BBCA 85 6C       STA $6C    [$00:006C]   A:9665 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBCA 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    // $80/BBCC A0 08       LDY #$08                A:9665 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBCC A0 08       LDY #$08               ", a, x, y);
    y = 8;

    // $80/BBCE 64 77       STZ $77    [$00:0077]   A:9665 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBCE 64 77       STZ $77    [$00:0077]  ", a, x, y);
    mem77 = 0;

    // $80/BBD0 64 75       STZ $75    [$00:0075]   A:9665 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBD0 64 75       STZ $75    [$00:0075]  ", a, x, y);
    mem75 = 0;

    DebugPrint("$80/BBD2 A9 10 00    LDA #$0010             ", a, x, y);
    a = 0x10;

    DebugPrint("$80/BBD5 85 14       STA $14    [$00:0014]  ", a, x, y);
    mem14 = 0x10;

    // $80/BBD7 A2 FE       LDX #$FE                A:0010 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBD7 A2 FE       LDX #$FE               ", a, x, y);
    x = 0xFE;

label_BBD9:
    // $80/BBD9 E8          INX                     A:0010 X:00FE Y:0008 P:envmxdizc
    DebugPrint("$80/BBD9 E8          INX                    ", a, x, y);
    x = IncLow8(x);

    // $80/BBDA E8          INX                     A:0010 X:00FF Y:0008 P:envmxdizc
    DebugPrint("$80/BBDA E8          INX                    ", a, x, y);
    x = IncLow8(x);

    // $80/BBDB C6 14       DEC $14    [$00:0014]   A:0010 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBDB C6 14       DEC $14    [$00:0014]  ", a, x, y);
    mem14--;

    // $80/BBDD 06 75       ASL $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBDD 06 75       ASL $75    [$00:0075]  ", a, x, y);
    mem75 *= 2;

    // $80/BBDF A5 75       LDA $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBDF A5 75       LDA $75    [$00:0075]  ", a, x, y);
    a = mem75;

    // $80/BBE1 38          SEC                     A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBE1 38          SEC                    ", a, x, y);
    c = true;

    // $80/BBE2 E5 77       SBC $77    [$00:0077]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBE2 E5 77       SBC $77    [$00:0077]  ", a, x, y);
    a -= mem77;

    // $80/BBE4 9D 20 07    STA $0720,x[$99:0720]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrintWithIndex("$80/BBE4 9D 20 07    STA $0720,x[$99:", 0x720 + x, a, x, y);
    loaded16.Data16 = a;
    cache7E0720[x] = loaded16.Low8; // Write A to 99/0720 -- this is 7E0720 (it's shadowed).
    cache7E0720[x+1] = loaded16.High8;

    // $80/BBE7 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBE7 20 B0 C1    JSR $C1B0  [$80:C1B0]  ", a, x, y);
    Fn_80C1B0();

    // $80/BBEA 9D 00 07    STA $0700,x[$99:0700]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithIndex("$80/BBEA 9D 00 07    STA $0700,x[$99:", 0x700 + x, a, x, y);
    cache7E0700[x] = a;

    // $80/BBED 18          CLC                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBED 18          CLC                    ", a, x, y);
    c = false;

    // $80/BBEE 65 77       ADC $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBEE 65 77       ADC $77    [$00:0077]  ", a, x, y);
    a += mem77;

    // $80/BBF0 85 77       STA $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF0 85 77       STA $77    [$00:0077]  ", a, x, y);
    mem77 = a;

    // $80/BBF2 A5 6F       LDA $6F    [$00:006F]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF2 A5 6F       LDA $6F    [$00:006F]  ", a, x, y);
    a = mem6f;

    // $80/BBF4 18          CLC                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF4 18          CLC                    ", a, x, y);
    c = false;

    // $80/BBF5 65 75       ADC $75    [$00:0075]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF5 65 75       ADC $75    [$00:0075]  ", a, x, y);
    a += mem75;

    // $80/BBF7 85 75       STA $75    [$00:0075]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF7 85 75       STA $75    [$00:0075]  ", a, x, y);
    mem75 = a;

    // $80/BBF9 A5 6F       LDA $6F    [$00:006F]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF9 A5 6F       LDA $6F    [$00:006F]  ", a, x, y);
    a = mem6f;
    z = a == 0;

    // $80/BBFB D0 05       BNE $05    [$BC02]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBFB D0 05       BNE $05    [$BC02]     ", a, x, y);
    if (!z)
    {
        goto label_BC02;
    }

    // $80/BBFD 9E 40 07    STZ $0740,x[$99:0740]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithIndex("$80/BBFD 9E 40 07    STZ $0740,x[$99:", 0x740+x, a, x, y);
    loaded16.Data16 = a;
    cache7E0740[x] = loaded16.Low8;
    cache7E0740[x+1] = loaded16.High8;

    // $80/BC00 80 D7       BRA $D7    [$BBD9]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC00 80 D7       BRA $D7    [$BBD9]     ", a, x, y);
    goto label_BBD9;

    __debugbreak(); // notimpl

label_BC02:
    // $80/BC02 A5 75       LDA $75    [$00:0075]   A:0001 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC02 A5 75       LDA $75    [$00:0075]  ", a, x, y);
    a = mem75;

    // $80/BC04 85 00       STA $00    [$00:0000]   A:0001 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC04 85 00       STA $00    [$00:0000]  ", a, x, y);
    mem00.Data16 = a;

    // $80/BC06 A5 14       LDA $14    [$00:0014]   A:0001 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC06 A5 14       LDA $14    [$00:0014]  ", a, x, y);
    a = mem14;
    z = a == 0;

    // $80/BC08 18          CLC                     A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC08 18          CLC                    ", a, x, y);
    c = false;

    // $80/BC09 F0 05       BEQ $05    [$BC10]      A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC09 F0 05       BEQ $05    [$BC10]     ", a, x, y);
    if (z)
    {
        __debugbreak(); // notimpl
        //goto label_BC10;
    }

label_BC0B:
    // $80/BC0B 06 00       ASL $00    [$00:0000]   A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC0B 06 00       ASL $00    [$00:0000]  ", a, x, y);
    c = mem00.Data16 >= 0x8000;
    mem00.Data16 *= 2;

    // $80/BC0D 3A          DEC A                   A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC0D 3A          DEC A                  ", a, x, y);
    --a;
    z = a == 0;

    // $80/BC0E D0 FB       BNE $FB    [$BC0B]      A:000D X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC0E D0 FB       BNE $FB    [$BC0B]     ", a, x, y);
    if (!z)
    {
        goto label_BC0B;
    }

    // $80/BC10 A5 00       LDA $00    [$00:0000]   A:0000 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC10 A5 00       LDA $00    [$00:0000]  ", a, x, y);
    a = mem00.Data16;

    // $80/BC12 9D 40 07    STA $0740,x[$99:0742]   A:4000 X:0002 Y:0002 P:envmxdizc
    DebugPrintWithIndex("$80/BC12 9D 40 07    STA $0740,x[$99:", 0x740 + x, a, x, y);
    loaded16.Data16 = a;
    cache7E0740[x] = loaded16.Low8;
    cache7E0740[x+1] = loaded16.High8;

    // $80/BC15 90 C2       BCC $C2    [$BBD9]      A:4000 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC15 90 C2       BCC $C2    [$BBD9]     ", a, x, y);
    if (!c)
    {
        goto label_BBD9;
    }

    // Can fall through-- cary can be set by ASL on mem00 earlier

    // $80/BC17 8A          TXA                     A:0000 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC17 8A          TXA                    ", a, x, y);
    a = x;

    // $80/BC18 4A          LSR A                   A:0012 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC18 4A          LSR A                  ", a, x, y);
    a >>= 1;

    // $80/BC19 85 79       STA $79    [$00:0079]   A:0009 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC19 85 79       STA $79    [$00:0079]  ", a, x, y);
    mem79 = a;

    // $80/BC1B A2 3E       LDX #$3E                A:0009 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC1B A2 3E       LDX #$3E               ", a, x, y);
    x = 0x3E;

label_BC1D:
    // $80/BC1D 9E 00 05    STZ $0500,x[$99:053E]   A:0009 X:003E Y:0008 P:envmxdizc
    // X = 0x3E, 0x3C, 0x3A, ... 6, 4, 2, 0
    DebugPrintWithIndex("$80/BC1D 9E 00 05    STZ $0500,x[$99:", 0x990500 + x, a, x, y);
    mem7E0500_7E0700[x] = 0;
    mem7E0500_7E0700[x+1] = 0;

    // $80/BC20 9E 40 05    STZ $0540,x[$99:057E]   A:0009 X:003E Y:0008 P:envmxdizc
    DebugPrintWithIndex("$80/BC20 9E 40 05    STZ $0540,x[$99:", 0x990540 + x, a, x, y);
    mem7E0500_7E0700[0x40 + x] = 0;
    mem7E0500_7E0700[0x40 + x+1] = 0;

    // $80/BC23 9E 80 05    STZ $0580,x[$99:05BE]   A:0009 X:003E Y:0008 P:envmxdizc
    DebugPrintWithIndex("$80/BC23 9E 80 05    STZ $0580,x[$99:", 0x990580 + x, a, x, y);
    mem7E0500_7E0700[0x80 + x] = 0;
    mem7E0500_7E0700[0x80 + x+1] = 0;

    // $80/BC26 9E C0 05    STZ $05C0,x[$99:05FE]   A:0009 X:003E Y:0008 P:envmxdizc
    DebugPrintWithIndex("$80/BC26 9E C0 05    STZ $05C0,x[$99:", 0x9905C0 + x, a, x, y);
    mem7E0500_7E0700[0xC0 + x] = 0;
    mem7E0500_7E0700[0xC0 + x+1] = 0;
    
    DebugPrint("$80/BC29 CA          DEX                    ", a, x, y);
    --x;
    x &= 0xFF;

    DebugPrint("$80/BC2A CA          DEX                    ", a, x, y);
    --x;
    x &= 0xFF;
    n = x >= 0x80;

    DebugPrint("$80/BC2B 10 F0       BPL $F0    [$BC1D]     ", a, x, y);
    if (!n)
    {
        goto label_BC1D;
    }

    // Indirect RAM access -- hardcoded access 0x7E0100
    // $80/BC2D A2 7E       LDX #$7E                A:0009 X:00FE Y:0008 P:envmxdizc
    DebugPrint("$80/BC2D A2 7E       LDX #$7E               ", a, x, y);
    x = 0x7E;

    // $80/BC2F 8E 83 21    STX $2183  [$99:2183]   A:0009 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC2F 8E 83 21    STX $2183  [$99:2183]  ", a, x, y);
    indirectHigh = x;

    // $80/BC32 A9 00 01    LDA #$0100              A:0009 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC32 A9 00 01    LDA #$0100             ", a, x, y);
    a = 0x100;

    // $80/BC35 8D 81 21    STA $2181  [$99:2181]   A:0100 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC35 8D 81 21    STA $2181  [$99:2181]  ", a, x, y);
    indirectLow = a;

    // $80/BC38 A2 FF       LDX #$FF                A:0100 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC38 A2 FF       LDX #$FF               ", a, x, y);
    x = 0xFF;

label_BC3A:

    // $80/BC3A 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0100 X:00FF Y:0008 P:envmxdizc
    DebugPrint("$80/BC3A 20 B0 C1    JSR $C1B0  [$80:C1B0]  ", a, x, y);
    Fn_80C1B0();

    // $80/BC3D E2 20       SEP #$20                A:0000 X:00FF Y:0005 P:envmxdizc
    DebugPrint("$80/BC3D E2 20       SEP #$20               ", a, x, y);

    // $80/BC3F 1A          INC A                   A:0000 X:00FF Y:0005 P:envmxdizc
    DebugPrint("$80/BC3F 1A          INC A                  ", a, x, y);
    ++a;

label_BC40:

    // $80/BC40 E8          INX                     A:0001 X:00FF Y:0005 P:envmxdizc
    DebugPrint("$80/BC40 E8          INX                    ", a, x, y);
    ++x;
    x &= 0xFF;

    // $80/BC41 3C 00 05    BIT $0500,x[$99:0500]   A:0001 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithIndex("$80/BC41 3C 00 05    BIT $0500,x[$99:", 0x500 + x, a, x, y);
    n = mem7E0500_7E0700[x] >= 0x80;

    // $80/BC44 30 FA       BMI $FA    [$BC40]      A:0012 X:0022 Y:0006 P:envMXdiZc
    DebugPrint("$80/BC44 30 FA       BMI $FA    [$BC40]     ", a, x, y);
    if (n)
    {
        goto label_BC40;
    }

    // $80/BC46 3A          DEC A                   A:0001 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC46 3A          DEC A                  ", a, x, y);
    --a;
    z = a == 0;

    // $80/BC47 D0 F7       BNE $F7    [$BC40]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC47 D0 F7       BNE $F7    [$BC40]     ", a, x, y);
    if (!z)
    {
        goto label_BC40;
    }

    // $80/BC49 DE 00 05    DEC $0500,x[$99:0500]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithIndex("$80/BC49 DE 00 05    DEC $0500,x[$99:", 0x500 + x, a, x, y);
    mem7E0500_7E0700[x]--;

    // $80/BC4C C2 20       REP #$20                A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC4C C2 20       REP #$20               ", a, x, y);

    // $80/BC4E 8E 80 21    STX $2180  [$99:2180]   A:0000 X:0000 Y:0005 P:envmxdizc
    // This is running in 8 bit index mode.
    DebugPrint("$80/BC4E 8E 80 21    STX $2180  [$99:2180]  ", a, x, y);
    if (indirectHigh == 0x7E && indirectLow >= 0x100)
    {
        loaded16.Data16 = x;
        cache7E0100[indirectLow - 0x100] = loaded16.Low8;
    }
    else
    {
        __debugbreak();
    }
    indirectLow += 1;

    // $80/BC51 C6 77       DEC $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC51 C6 77       DEC $77    [$00:0077]  ", a, x, y);
    mem77--;
    z = mem77 == 0;

    // $80/BC53 D0 E5       BNE $E5    [$BC3A]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC53 D0 E5       BNE $E5    [$BC3A]     ", a, x, y);
    if (!z)
    {
        goto label_BC3A;
    }

    // $80/BC55 98          TYA                     A:0000 X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC55 98          TYA                    ", a, x, y);
    a = y;

    // $80/BC56 0A          ASL A                   A:0006 X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC56 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BC57 85 71       STA $71    [$00:0071]   A:000C X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC57 85 71       STA $71    [$00:0071]  ", a, x, y);
    mem71 = a;

    // $80/BC59 E2 20       SEP #$20                A:000C X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC59 E2 20       SEP #$20               ", a, x, y);

    // $80/BC5B A2 00       LDX #$00                A:000C X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC5B A2 00       LDX #$00               ", a, x, y);
    x = 0;

    // $80/BC5D 9B          TXY                     A:000C X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/BC5D 9B          TXY                    ", a, x, y);
    y = x;

    // $80/BC5E 64 7B       STZ $7B    [$00:007B]   A:000C X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC5E 64 7B       STZ $7B    [$00:007B]  ", a, x, y);
    mem7b = 0;

label_BC60:

    // $80/BC60 86 00       STX $00    [$00:0000]   A:000C X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC60 86 00       STX $00    [$00:0000]  ", a, x, y);
    mem00.Data16 = x;

    // $80/BC62 A5 7B       LDA $7B    [$00:007B]   A:000C X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC62 A5 7B       LDA $7B    [$00:007B]  ", a, x, y);
    a = mem7b;

    // $80/BC64 0A          ASL A                   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC64 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BC65 AA          TAX                     A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC65 AA          TAX                    ", a, x, y);
    x = a;

    // $80/BC66 E0 10       CPX #$10                A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC66 E0 10       CPX #$10               ", a, x, y);
    z = x == 0x10;

    // $80/BC68 F0 55       BEQ $55    [$BCBF]      A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC68 F0 55       BEQ $55    [$BCBF]     ", a, x, y);
    if (z)
    {
        goto label_BCBF;
    }

    // $80/BC6A BD 00 07    LDA $0700,x[$99:0700]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrintWithIndex("$80/BC6A BD 00 07    LDA $0700,x[$99:", 0x700 + x, a, x, y);
    a = cache7E0700[x]; // X = 0x0-0x50
    
    // $80/BC6D 85 77       STA $77    [$00:0077]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC6D 85 77       STA $77    [$00:0077]  ", a, x, y);
    mem77 = a;

    // $80/BC6F A6 7B       LDX $7B    [$00:007B]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC6F A6 7B       LDX $7B    [$00:007B]  ", a, x, y);
    x = mem7b;

    // $80/BC71 BF 7B BC 80 LDA $80BC7B,x[$80:BC7B] A:0000 X:0000 Y:0000 P:envmxdizc
    // x=0..25
    // Load from ROM. 8bit load
    DebugPrintWithIndex("$80/BC71 BF 7B BC 80 LDA $80BC7B,x[$80:", 0xBC7B + x, a, x, y, true);
    a = romFile[0x3C7B + x];
    
    // $80/BC75 85 7D       STA $7D    [$00:007D]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC75 85 7D       STA $7D    [$00:007D]  ", a, x, y);
    mem7d = a;

    // $80/BC77 A6 00       LDX $00    [$00:0000]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC77 A6 00       LDX $00    [$00:0000]  ", a, x, y);
    x = mem00.Data16;

    // $80/BC79 80 36       BRA $36    [$BCB1]      A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC79 80 36       BRA $36    [$BCB1]     ", a, x, y);
    goto label_BCB1;

    __debugbreak();

label_BC83:

    // $80/BC83 A5 7B       LDA $7B    [$00:007B]   A:0041 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC83 A5 7B       LDA $7B    [$00:007B]  ", a, x, y);
    a = mem7b;

    // $80/BC85 0A          ASL A                   A:0001 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC85 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BC86 85 00       STA $00    [$00:0000]   A:0002 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC86 85 00       STA $00    [$00:0000]  ", a, x, y);
    mem00.Data16 = a;

    // $80/BC88 B9 00 01    LDA $0100,y[$99:0100]   A:0002 X:0000 Y:0000 P:envmxdizc
    // This is running in 8 bit accumulator and index mode.
    DebugPrintWithIndex("$80/BC88 B9 00 01    LDA $0100,y[$99:", 0x100 + y, a, x, y);
    loaded16.Data16 = a;
    loaded16.Low8 = cache7E0100[y];
    a = loaded16.Data16;

    // $80/BC8B 85 01       STA $01    [$00:0001]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC8B 85 01       STA $01    [$00:0001]  ", a, x, y);
    mem00.High8 = a;

    // $80/BC8D C8          INY                     A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC8D C8          INY                    ", a, x, y);
    ++y;

    // $80/BC8E C5 73       CMP $73    [$00:0073]   A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC8E C5 73       CMP $73    [$00:0073]  ", a, x, y);
    z = a == (mem73 & 0xFF); // we are in 8bit mode

    // $80/BC90 D0 09       BNE $09    [$BC9B]      A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC90 D0 09       BNE $09    [$BC9B]     ", a, x, y);
    if (!z)
    {
        goto label_BC9B; 
    }

    // $80/BC92 A5 7B       LDA $7B    [$00:007B]   A:000F X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC92 A5 7B       LDA $7B    [$00:007B]  ", a, x, y);
    a = mem7b;

    // $80/BC94 1A          INC A                   A:0005 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC94 1A          INC A                  ", a, x, y);
    ++a;

    // $80/BC95 85 74       STA $74    [$00:0074]   A:0006 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC95 85 74       STA $74    [$00:0074]  ", a, x, y);
    mem73 &= 0x00FF; // Keep the first, lower byte
    mem73 |= (a << 8); // Replace the upper byte, second byte

    // $80/BC97 A9 12       LDA #$12                A:0006 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC97 A9 12       LDA #$12               ", a, x, y);
    a = 0x12;

    // $80/BC99 85 00       STA $00    [$00:0000]   A:0012 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC99 85 00       STA $00    [$00:0000]  ", a, x, y);
    mem00.Low8 = a;

label_BC9B:

    // $80/BC9B 84 04       STY $04    [$00:0004]   A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC9B 84 04       STY $04    [$00:0004]  ", a, x, y);
    mem04 = y;

    // $80/BC9D A4 7D       LDY $7D    [$00:007D]   A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC9D A4 7D       LDY $7D    [$00:007D]  ", a, x, y);
    y = mem7d;

    // $80/BC9F 80 0B       BRA $0B    [$BCAC]      A:0000 X:0000 Y:0041 P:envmxdizc
    DebugPrint("$80/BC9F 80 0B       BRA $0B    [$BCAC]     ", a, x, y);
    goto label_BCAC;

label_BCA1:

    // $80/BCA1 A5 01       LDA $01    [$00:0001]   A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCA1 A5 01       LDA $01    [$00:0001]  ", a, x, y);
    // Select the upper byte of mem0
    a = mem00.High8;

    // $80/BCA3 9D 00 05    STA $0500,x[$99:0500]   A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrintWithIndex("$80/BCA3 9D 00 05    STA $0500,x[$99:", 0x500 + x, a, x, y);
    mem7E0500_7E0700[x] = (a & 0xFF); // Is this right?

    // $80/BCA6 A5 00       LDA $00    [$00:0000]   A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCA6 A5 00       LDA $00    [$00:0000]  ", a, x, y);
    a = mem00.Low8;

    // $80/BCA8 9D 00 06    STA $0600,x[$99:0600]   A:0002 X:0000 Y:0040 P:envmxdizc
    DebugPrintWithIndex("$80/BCA8 9D 00 06    STA $0600,x[$99:", 0x600 + x, a, x, y);
    mem7E0500_7E0700[0x100 + x] = (a & 0xFF);

    // $80/BCAB E8          INX                     A:0002 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCAB E8          INX                    ", a, x, y);
    ++x;

label_BCAC:
    // $80/BCAC 88          DEY                     A:0000 X:0000 Y:0041 P:envmxdizc
    DebugPrint("$80/BCAC 88          DEY                    ", a, x, y);
    --y;
    z = y == 0;

    // $80/BCAD D0 F2       BNE $F2    [$BCA1]      A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCAD D0 F2       BNE $F2    [$BCA1]     ", a, x, y);
    if (!z)
    {
        goto label_BCA1;
    }

    // $80/BCAF A4 04       LDY $04    [$00:0004]   A:0002 X:0040 Y:0000 P:envmxdizc
    DebugPrint("$80/BCAF A4 04       LDY $04    [$00:0004]  ", a, x, y);
    y = mem04;

label_BCB1:

    // $80/BCB1 C6 77       DEC $77    [$00:0077]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB1 C6 77       DEC $77    [$00:0077]  ", a, x, y);
    mem77--;
    n = mem77 >= 0x8000;

    // $80/BCB3 10 CE       BPL $CE    [$BC83]      A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB3 10 CE       BPL $CE    [$BC83]     ", a, x, y);
    if (!n)
    {
        goto label_BC83;
    }

    // $80/BCB5 E6 7B       INC $7B    [$00:007B]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB5 E6 7B       INC $7B    [$00:007B]  ", a, x, y);
    mem7b++;

    // $80/BCB7 A5 79       LDA $79    [$00:0079]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB7 A5 79       LDA $79    [$00:0079]  ", a, x, y);
    a = mem79;

    // $80/BCB9 C5 7B       CMP $7B    [$00:007B]   A:0009 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB9 C5 7B       CMP $7B    [$00:007B]  ", a, x, y);
    c = a >= mem7b;

    DebugPrint("$80/BCBB B0 A3       BCS $A3    [$BC60]     ", a, x, y);
    // $80/BCBB B0 A3       BCS $A3    [$BC60]      A:0009 X:0000 Y:0000 P:envmxdizc
    if (c)
    {
        goto label_BC60;
    }
    
    __debugbreak();

label_BCBF:

    // $80/BCBF A6 00       LDX $00    [$00:0000]   A:0010 X:0010 Y:003B P:envmxdizc
    DebugPrint("$80/BCBF A6 00       LDX $00    [$00:0000]  ", a, x, y);
    x = mem00.Low8;
    z = x == 0;

    // $80/BCC1 F0 08       BEQ $08    [$BCCB]      A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrint("$80/BCC1 F0 08       BEQ $08    [$BCCB]     ", a, x, y);
    if (z)
    {
        __debugbreak();
        //goto label_BCCB;
    }

    // $80/BCC3 A9 10       LDA #$10                A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrint("$80/BCC3 A9 10       LDA #$10               ", a, x, y);
    a = 0x10;

label_BCC5:

    // $80/BCC5 9D 00 06    STA $0600,x[$99:06F1]   A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrintWithIndex("$80/BCC5 9D 00 06    STA $0600,x[$99:", 0x600 + x, a, x, y);
    mem7E0500_7E0700[0x100 + x] = a;

    // $80/BCC8 E8          INX                     A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrint("$80/BCC8 E8          INX                    ", a, x, y);
    ++x;
    x &= 0x00FF;
    z = x == 0;

    // $80/BCC9 D0 FA       BNE $FA    [$BCC5]      A:0010 X:00F2 Y:003B P:envmxdizc
    DebugPrint("$80/BCC9 D0 FA       BNE $FA    [$BCC5]     ", a, x, y);
    if (!z)
    {
        goto label_BCC5;
    }

    // $80/BCCB A5 79       LDA $79    [$00:0079]   A:0010 X:0000 Y:003B P:envmxdizc
    DebugPrint("$80/BCCB A5 79       LDA $79    [$00:0079]  ", a, x, y);
    a = mem79;

    // $80/BCCD 0A          ASL A                   A:0009 X:0000 Y:003B P:envmxdizc
    DebugPrint("$80/BCCD 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BCCE AA          TAX                     A:0012 X:0000 Y:003B P:envmxdizc
    DebugPrint("$80/BCCE AA          TAX                    ", a, x, y);
    x = a;

    // $80/BCCF C2 20       REP #$20                A:0012 X:0012 Y:003B P:envmxdizc
    DebugPrint("$80/BCCF C2 20       REP #$20               ", a, x, y);

    // $80/BCD1 BF D9 BC 80 LDA $80BCD9,x[$80:BCEB] A:0012 X:0012 Y:003B P:envmxdizc
    DebugPrintWithIndex("$80/BCD1 BF D9 BC 80 LDA $80BCD9,x[$80:", 0xBCD9 + x, a, x, y, true);
    if (x == 0x10)
    {
        a = 0xBFC5;
    }
    else if (x == 0x12)
    {
        a = 0xBFC8;
    }
    else
    {
        __debugbreak(); // notimpl
    }

    // $80/BCD5 8D 60 07    STA $0760  [$99:0760]   A:BFC8 X:0012 Y:003B P:envmxdizc
    DebugPrint("$80/BCD5 8D 60 07    STA $0760  [$99:0760]  ", a, x, y);
    loaded16.Data16 = a;
    cache7E0760[0] = loaded16.Low8;
    cache7E0760[1] = loaded16.High8;

    // $80/BCD8 A4 12       LDY $12    [$00:0012]   A:BFC8 X:0012 Y:003B P:envmxdizc
    DebugPrint("$80/BCD8 A4 12       LDY $12    [$00:0012]  ", a, x, y);
    y = mem12;

    // $80/BCDA 8C 83 21    STY $2183  [$99:2183]   A:BFC8 X:0012 Y:007F P:envmxdizc    
    DebugPrint("$80/BCDA 8C 83 21    STY $2183  [$99:2183]  ", a, x, y); // Indirect wram high bit
    indirectHigh = mem12;

    // $80/BCDD A5 10       LDA $10    [$00:0010]   A:BFC8 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCDD A5 10       LDA $10    [$00:0010]  ", a, x, y);
    a = mem10;

    // $80/BCDF 8D 81 21    STA $2181  [$99:2181]   A:0000 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCDF 8D 81 21    STA $2181  [$99:2181]  ", a, x, y);   // Indirect wram low and mid byte
    indirectLow = mem10;

    // $80/BCE2 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCE2 A5 6C       LDA $6C    [$00:006C]  ", a, x, y);
    a = mem6c;

    // $80/BCE4 A6 71       LDX $71    [$00:0071]   A:2508 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCE4 A6 71       LDX $71    [$00:0071]  ", a, x, y);
    x = mem71;

    // $80/BCE6 7C F9 BC    JMP ($BCF9,x)[$80:BDBE] A:2508 X:000C Y:007F P:envmxdizc
    // x = {2, 6, C, E}
    if (x == 2)
    {
        // goto 80:BF53
        __debugbreak();
    }
    else if (x == 6)
    {
        // goto 80:BEAE
        __debugbreak();
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BCE6 7C F9 BC    JMP ($BCF9,x)[$80:BDBE]", a, x, y);
        goto label_BDBE;
    }
    else if (x == 0xE)
    {
        // goto 80:BD70
        __debugbreak();
    }
    else
    {
        __debugbreak();
    }

label_BD0B:
    DebugPrint("$80/BD0B 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD0C:
    DebugPrint("$80/BD0C 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD0D:

    // $80/BD0D 0A          ASL A                   A:B8AC X:000A Y:00B8 P:envmxdizc
    DebugPrint("$80/BD0D 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD0E:

    // $80/BD0E 0A          ASL A                   A:7158 X:000A Y:00B8 P:envmxdizc
    DebugPrint("$80/BD0E 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD0F:
    // $80/BD0F 0A          ASL A                   A:E2B0 X:000A Y:00B8 P:envmxdizc
    DebugPrint("$80/BD0F 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BD10 0A          ASL A                   A:C560 X:000A Y:00B8 P:envmxdizc
    DebugPrint("$80/BD10 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD11:

    // $80/BD11 0A          ASL A                   A:8AC0 X:000A Y:00B8 P:envmxdizc
    DebugPrint("$80/BD11 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BD12 0A          ASL A                   A:1580 X:000A Y:00B8 P:envmxdizc
    DebugPrint("$80/BD12 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBD13);

    LoadNextFrom0500(0xBD1B);

label_BD23:
    LoadNextFrom0600(0xBD23);

    // $80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BD41] A:948C X:0012 Y:0094 P:envmxdizc
    if (x == 0x12)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BD41]", a, x, y);
        goto label_BD41;
    }
    else if (x == 0x2)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BDB4]", a, x, y);
        goto label_BDB4;
    }
    else if (x == 0x8)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BEA1]", a, x, y);
        goto label_BEA1;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BEF2]", a, x, y);
        goto label_BEF2;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BD0B]", a, x, y);
        goto label_BD0B;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BD46]", a, x, y);
        goto label_BD46;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BF44]", a, x, y);
        goto label_BF44;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BE51]", a, x, y);
        goto label_BE51;
    }
    else
    {
        __debugbreak();
    }

    __debugbreak();

label_BD41:
    DebugPrint("$80/BD41 A2 10       LDX #$10               ", a, x, y);
    x = 0x10;

    DebugPrint("$80/BD43 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

label_BD46:

    LoadNextFrom0CMaskAndShift(0xBD46, 0x10, 0);

    DebugPrint("$80/BD55 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);
    goto label_BFC8;

label_BD58:

    DebugPrint("$80/BD58 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD59:
    DebugPrint("$80/BD59 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD5A:

    DebugPrint("$80/BD5A 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD5B:

    DebugPrint("$80/BD5B 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD5C:
    DebugPrint("$80/BD5C 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BD5D 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BD5E:
    DebugPrint("$80/BD5E 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBD5F);

    DebugPrint("$80/BD67 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0500(0xBD68);

label_BD70:
    LoadNextFrom0600(0xBD70);

    if (x == 0xE)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BD58]", a, x, y);
        goto label_BD58;
    }
    else if (x == 2)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BE03]", a, x, y);
        goto label_BE03;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BF45]", a, x, y);
        goto label_BF45;
    }
    else if (x == 8)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BEF3]", a, x, y);
        goto label_BEF3;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BD93]", a, x, y);
        goto label_BD93;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BD0C]", a, x, y);
        goto label_BD0C;
    }
    else if (x == 0x12)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BD8E]", a, x, y);
        goto label_BD8E;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BEA2]", a, x, y);
        goto label_BEA2;
    }
    else
    {
        __debugbreak();
    }

label_BD8E:

    DebugPrint("$80/BD8E A2 0E       LDX #$0E               ", a, x, y);
    x = 0xE;

    DebugPrint("$80/BD90 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

label_BD93:
    LoadNextFrom0CMaskAndShift(0xBD93, 0xE, 1);

    DebugPrint("$80/BDA3 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);
    goto label_BFC8;

label_BDA6:
    DebugPrint("$80/BDA6 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDA7:
    DebugPrint("$80/BDA7 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDA8:
    DebugPrint("$80/BDA8 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDA9:
    // $80/BDA9 0A          ASL A                   A:85C0 X:0008 Y:0085 P:envmxdizc
    DebugPrint("$80/BDA9 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDAA:
    // $80/BDAA 0A          ASL A                   A:0B80 X:0008 Y:0085 P:envmxdizc
    DebugPrint("$80/BDAA 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BDAB 0A          ASL A                   A:1700 X:0008 Y:0085 P:envmxdizc
    DebugPrint("$80/BDAB 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBDAC);

label_BDB4:
    // $80/BDB4 0A          ASL A                   A:2E2B X:0008 Y:0085 P:envmxdizc
    DebugPrint("$80/BDB4 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BDB5 0A          ASL A                   A:5C56 X:0008 Y:0085 P:envmxdizc
    DebugPrint("$80/BDB5 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0500(0xBDB6);

label_BDBE:

    LoadNextFrom0600(0xBDBE);

    // $80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BE53] A:2508 X:0002 Y:0025 P:envmxdizc
    if (x == 2)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BE53]", a, x, y);
        goto label_BE53;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BD0D]", a, x, y);
        goto label_BD0D;
    }
    else if (x == 8)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BF46]", a, x, y);
        goto label_BF46;
    }
    else if (x == 0x12)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BDDC]", a, x, y);
        goto label_BDDC;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BD59]", a, x, y);
        goto label_BD59;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BDA6]", a, x, y);
        goto label_BDA6;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BEF4]", a, x, y);
        goto label_BEF4;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BDE1]", a, x, y);
        goto label_BDE1;
    }
    else
    {
        __debugbreak(); // notimpl
    }

    __debugbreak();

label_BDDC:

    DebugPrint("$80/BDDC A2 0C       LDX #$0C               ", a, x, y);
    x = 0xC;

    DebugPrint("$80/BDDE 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

label_BDE1:

    LoadNextFrom0CMaskAndShift(0xBDE1, 0xC, 2);

    DebugPrint("$80/BDF2 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);
    goto label_BFC8;

label_BDF5:
    DebugPrint("$80/BDF5 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDF6:
    DebugPrint("$80/BDF6 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDF7:
    DebugPrint("$80/BDF7 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDF8:
    DebugPrint("$80/BDF8 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BDF9:
    DebugPrint("$80/BDF9 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBDFA);

    DebugPrint("$80/BE02 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BE03:
    DebugPrint("$80/BE03 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BE04 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0500(0xBE05);

label_BE0D:
    LoadNextFrom0600(0xBE0D);

    if (x == 0x10)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BE30]", a, x, y);
        goto label_BE30;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BDF5]", a, x, y);
        goto label_BDF5;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BDA7]", a, x, y);
        goto label_BDA7;
    }
    else if (x == 8)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BD0E]", a, x, y);
        goto label_BD0E;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BD5A]", a, x, y);
        goto label_BD5A;
    }
    else if (x == 2)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BEA4]", a, x, y);
        goto label_BEA4;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BF47]", a, x, y);
        goto label_BF47;
    }
    else if (x == 0x12)
    {
        DebugPrint("$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BE2B]", a, x, y);
        goto label_BE2B;
    }
    else
    {
        __debugbreak();
    }

label_BE2B:

    DebugPrint("$80/BE2B A2 0A       LDX #$0A               ", a, x, y);
    x = 0xA;

    DebugPrint("$80/BE2D 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

label_BE30:
    LoadNextFrom0CMaskAndShift(0xBE30, 0xA, 3);

    DebugPrint("$80/BE42 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);

goto label_BFC8;

label_BE45:
    DebugPrint("$80/BE45 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BE46:
    DebugPrint("$80/BE46 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BE47:

    DebugPrint("$80/BE47 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BE48:

    DebugPrint("$80/BE48 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBE49);

label_BE51:
    DebugPrint("$80/BE51 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BE52 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BE53:

    // $80/BE53 0A          ASL A                   A:2508 X:0002 Y:0025 P:envmxdizc
    DebugPrint("$80/BE53 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/BE54 0A          ASL A                   A:4A10 X:0002 Y:0025 P:envmxdizc
    DebugPrint("$80/BE54 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0500(0xBE55);

label_BE5D:
    LoadNextFrom0600(0xBE5D);

    // $80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BE7B] A:9420 X:0012 Y:0094 P:envmxdizc
    if (x == 0x12)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BE7B]", a, x, y);
        goto label_BE7B;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BE45]", a, x, y);
        goto label_BE45;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BDA8]", a, x, y);
        goto label_BDA8;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BDF6]", a, x, y);
        goto label_BDF6;
    }
    else if (x == 8)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BD5B]", a, x, y);
        goto label_BD5B;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BE80]", a, x, y);
        goto label_BE80;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BD0F]", a, x, y);
        goto label_BD0F;
    }
    else if (x == 2)
    {
        DebugPrint("$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BEF6]", a, x, y);
        goto label_BEF6;
    }
    else
    {
        __debugbreak(); // notimpl
    }

    __debugbreak();

label_BE7B:
    // $80/BE7B A2 08       LDX #$08                A:9420 X:0012 Y:0094 P:envmxdizc
    DebugPrint("$80/BE7B A2 08       LDX #$08               ", a, x, y);
    x = 0x8;

    // $80/BE7D 4C 7C C1    JMP $C17C  [$80:C17C]   A:9420 X:0008 Y:0094 P:envmxdizc
    DebugPrint("$80/BE7D 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

label_BE80:

    LoadNextFrom0CMaskAndShift(0xBE80, 8, 4);

    DebugPrint("$80/BE93 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);
    goto label_BFC8;

label_BE96:

    DebugPrint("$80/BE96 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BE97:
    DebugPrint("$80/BE97 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BE98:
    DebugPrint("$80/BE98 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBE99);

label_BEA1:

    DebugPrint("$80/BEA1 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BEA2:

    DebugPrint("$80/BEA2 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BEA3 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BEA4:

    DebugPrint("$80/BEA4 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BEA5 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0500(0xBEA6);

label_BEAE:

    LoadNextFrom0600(0xBEAE);

    // $80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BED1] A:F180 X:0010 Y:00F1 P:envmxdizc
    if (x == 0x10)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BED1]", a, x, y);
        goto label_BED1;
    }
    else if (x == 0x8)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BDA9]", a, x, y);
        goto label_BDA9;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BDF7]", a, x, y);
        goto label_BDF7;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BE96]", a, x, y);
        goto label_BE96;
    }
    else if (x == 2)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BF49]", a, x, y);
        goto label_BF49;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BE46]", a, x, y);
        goto label_BE46;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BD5C]", a, x, y);
        goto label_BD5C;
    }
    else if (x == 0x12)
    {
        DebugPrint("$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BECC]", a, x, y);
        goto label_BECC;
    }
    else
    {
        __debugbreak(); // notimpl
    }

label_BECC:

    DebugPrint("$80/BECC A2 06       LDX #$06               ", a, x, y);
    x = 0x6;

    DebugPrint("$80/BECE 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

    __debugbreak();

label_BED1:

    LoadNextFrom0CMaskAndShift(0xBED1, 6, 5);

    // $80/BEE5 6C 60 07    JMP ($0760)[$80:BFC8]   A:F192 X:0006 Y:00F1 P:envmxdizc
    DebugPrint("$80/BEE5 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);
    goto label_BFC8;

label_BEE8:

    DebugPrint("$80/BEE8 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BEE9:

    DebugPrint("$80/BEE9 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBEEA);

label_BEF2:

    DebugPrint("$80/BEF2 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BEF3:

    DebugPrint("$80/BEF3 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BEF4:
    DebugPrint("$80/BEF4 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BEF5 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BEF6:
    DebugPrint("$80/BEF6 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BEF7 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0500(0xBEF8);

label_BF00:

    LoadNextFrom0600(0xBF00);
    
    // $80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BD11] A:2500 X:0002 Y:0025 P:envmxdizc
    if (x == 2)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BD11]", a, x, y);
        goto label_BD11;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BE47]", a, x, y);
        goto label_BE47;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BEE8]", a, x, y);
        goto label_BEE8;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BE97]", a, x, y);
        goto label_BE97;
    }
    else if (x == 8)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BDF8]", a, x, y);
        goto label_BDF8;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BDAA]", a, x, y);
        goto label_BDAA;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BF23]", a, x, y);
        goto label_BF23;
    }
    else if (x == 0x12)
    {
        DebugPrint("$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BF1E]", a, x, y);
        goto label_BF1E;
    }
    else
    {
        __debugbreak();
    }

label_BF1E:

    DebugPrint("$80/BF1E A2 04       LDX #$04               ", a, x, y);
    x = 4;

    DebugPrint("$80/BF20 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

    __debugbreak();

label_BF23:

    LoadNextFrom0CMaskAndShift(0xBF23, 4, 6);

    DebugPrint("$80/BF38 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);
    goto label_BFC8;

label_BF3B:

    DebugPrint("$80/BF3B 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xBF3C);

label_BF44:

    DebugPrint("$80/BF44 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BF45:

    DebugPrint("$80/BF45 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BF46:

    DebugPrint("$80/BF46 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BF47:
    DebugPrint("$80/BF47 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BF48 0A          ASL A                  ", a, x, y);
    a *= 2;

label_BF49:

    DebugPrint("$80/BF49 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/BF4A 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0500(0xBF4B);

label_BF53:
    LoadNextFrom0600(0xBF53);

    if (x == 8)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BE48]", a, x, y);
        goto label_BE48;
    }
    else if (x == 2)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BD5E]", a, x, y);
        goto label_BD5E;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BEE9]", a, x, y);
        goto label_BEE9;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BF3B]", a, x, y);
        goto label_BF3B;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BE98]", a, x, y);
        goto label_BE98;
    }
    else if (x == 6)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BDF9]", a, x, y);
        goto label_BDF9;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BF76]", a, x, y);
        goto label_BF76;
    }
    else if (x == 0x12)
    {
        DebugPrint("$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BF71]", a, x, y);
        goto label_BF71;
    }
    else
    {
        __debugbreak();
    }

label_BF71:

    DebugPrint("$80/BF71 A2 02       LDX #$02               ", a, x, y);
    x = 2;

    DebugPrint("$80/BF73 4C 7C C1    JMP $C17C  [$80:C17C]  ", a, x, y);
    goto label_C17C;

label_BF76:

    LoadNextFrom0CMaskAndShift(0xBF76, 2, 7);

    DebugPrint("$80/BF8C 6C 60 07    JMP ($0760)[$80:BFC8]  ", a, x, y);
    goto label_BFC8;

label_BF8F:

    ShiftThenLoad100ThenCompare(0xBF8F, 7, 0x0730, 0x1);

    // $80/BFA4 F0 1C       BEQ $1C    [$BFC2]      A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/BFA4 F0 1C       BEQ $1C    [$BFC2]     ", a, x, y);
    if (z)
    {
        __debugbreak(); // notimpl
    }

    // $80/BFA6 4C E8 C0    JMP $C0E8  [$80:C0E8]   A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/BFA6 4C E8 C0    JMP $C0E8  [$80:C0E8]  ", a, x, y);
    goto label_C0E8;

label_BFA9:

    ShiftThenLoad100ThenCompare(0xBFA9, 6, 0x732, 2);

    // $80/BFBD F0 03       BEQ $03    [$BFC2]      A:00AA X:0006 Y:0002 P:envmxdizc
    DebugPrint("$80/BFBD F0 03       BEQ $03    [$BFC2]     ", a, x, y);
    if (z)
    {
        __debugbreak(); // notimpl
    }

    // $80/BFBF 4C E8 C0    JMP $C0E8  [$80:C0E8]   A:00AA X:0006 Y:0002 P:envmxdizc
    DebugPrint("$80/BFBF 4C E8 C0    JMP $C0E8  [$80:C0E8]  ", a, x, y);
    goto label_C0E8;

    __debugbreak();

label_BFC8:

    // $80/BFC8 CD 50 07    CMP $0750  [$99:0750]   A:F192 X:0006 Y:00F1 P:envmxdizc
    DebugPrint("$80/BFC8 CD 50 07    CMP $0750  [$99:0750]  ", a, x, y);
    loaded16.Low8 = cache7E0740[0x10];
    loaded16.High8 = cache7E0740[0x11];
    c = a >= loaded16.Data16;

    // $80/BFCB 90 C2       BCC $C2    [$BF8F]      A:F192 X:0006 Y:00F1 P:envmxdizc
    DebugPrint("$80/BFCB 90 C2       BCC $C2    [$BF8F]     ", a, x, y);
    if (!c)
    {
        goto label_BF8F;
    }

    DebugPrint("$80/BFCD 4C A9 BF    JMP $BFA9  [$80:BFA9]  ", a, x, y);
    goto label_BFA9;

label_C0E8:

    // $80/C0E8 8D 80 21    STA $2180  [$99:2180]   A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C0E8 8D 80 21    STA $2180  [$99:2180]  ", a, x, y);

    loaded16.Data16 = a;
    if (indirectHigh == 0x7E && indirectLow >= 0x100)
    {
        cache7E0100[indirectLow - 0x100] = loaded16.Low8;
        cache7E0100[indirectLow + 1 - 0x100] = loaded16.High8;
    }
    else if (indirectHigh == 0x7F)
    {
        cache7F0000[indirectLow] = loaded16.Low8;
        cache7F0000[indirectLow + 1] = loaded16.High8;
    }
    else
    {
        __debugbreak();
    }
    indirectLow += 2;

    // $80/C0EB 85 08       STA $08    [$00:0008]   A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C0EB 85 08       STA $08    [$00:0008]  ", a, x, y);
    loaded16.Data16 = mem08;
    loaded16.Low8 = a & 0xFF;
    mem08 = loaded16.Data16;

    // $80/C0ED C2 20       REP #$20                A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C0ED C2 20       REP #$20               ", a, x, y);
    // 
    // $80/C0EF E6 0C       INC $0C    [$00:000C]   A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C0EF E6 0C       INC $0C    [$00:000C]  ", a, x, y);
    mem0c++;
    // 
    // $80/C0F1 A5 6B       LDA $6B    [$00:006B]   A:0008 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C0F1 A5 6B       LDA $6B    [$00:006B]  ", a, x, y);
    loaded16 = LoadMem6b();
    a = loaded16.Data16;

    // $80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C11A] A:9280 X:0006 Y:0001 P:envmxdizc
    if (x == 6)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C11A]", a, x, y);
        goto label_C11A;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C112]", a, x, y);
        goto label_C112;
    }
    else if (x == 8)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C116]", a, x, y);
        goto label_C116;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C10A]", a, x, y);
        goto label_C10A;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C106]", a, x, y);
        goto label_C106;
    }
    else if (x == 4)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C11E]", a, x, y);
        goto label_C11E;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C10E]", a, x, y);
        goto label_C10E;
    }
    else if (x == 2)
    {
        DebugPrint("$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C122]", a, x, y);
        goto label_C122;
    }
    else
    {
        __debugbreak(); // notimpl
    }

    __debugbreak();

label_C106:
    DebugPrint("$80/C106 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/C107 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    DebugPrint("$80/C108 F0 4F       BEQ $4F    [$C159]     ", a, x, y);
    if (z)
    {
        goto label_C159;
    }

label_C10A:
    DebugPrint("$80/C10A 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/C10B 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    DebugPrint("$80/C10C F0 48       BEQ $48    [$C156]     ", a, x, y);
    if (z)
    {
        goto label_C156;
    }

label_C10E:
    DebugPrint("$80/C10E 0A          ASL A                  ", a, x, y);
    a *= 2;

    DebugPrint("$80/C10F 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    DebugPrint("$80/C110 F0 41       BEQ $41    [$C153]     ", a, x, y);
    if (z)
    {
        goto label_C153;
    }

label_C112:

    // $80/C112 0A          ASL A                   A:9280 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C112 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/C113 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C113 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    DebugPrint("$80/C114 F0 3A       BEQ $3A    [$C150]     ", a, x, y);
    if (z)
    {
        goto label_C150;
    }

label_C116:
    DebugPrint("$80/C116 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/C117 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C117 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    DebugPrint("$80/C118 F0 33       BEQ $33    [$C14D]     ", a, x, y);
    if (z)
    {
        goto label_C14D;
    }

label_C11A:

    // $80/C11A 0A          ASL A                   A:9280 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C11A 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/C11B 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C11B 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    // $80/C11C F0 2C       BEQ $2C    [$C14A]      A:2500 X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C11C F0 2C       BEQ $2C    [$C14A]     ", a, x, y);
    if (z)
    {
        goto label_C14A;
    }

label_C11E:
    // $80/C11E 0A          ASL A                   A:9280 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C11E 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/C11F 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc
    DebugPrint("$80/C11F 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    // $80/C120 F0 2C       BEQ $2C    [$C14A]      A:2500 X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C120 F0 25       BEQ $25    [$C147]     ", a, x, y);
    if (z)
    {
        goto label_C147;
    }

label_C122:

    DebugPrint("$80/C122 0A          ASL A                  ", a, x, y);
    a *= 2;

    LoadNextFrom0CInc(0xC123);

    DebugPrint("$80/C12B 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    DebugPrint("$80/C12C F0 2E       BEQ $2E    [$C15C]     ", a, x, y);
    if (z)
    {
        goto label_C15C;
    }

    __debugbreak();

label_C147:
    DebugPrint("$80/C147 4C 53 BF    JMP $BF53  [$80:BF53]  ", a, x, y);
    goto label_BF53;

label_C14A:
    // $80/C14A 4C 00 BF    JMP $BF00  [$80:BF00]   A:2500 X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C14A 4C 00 BF    JMP $BF00  [$80:BF00]  ", a, x, y);
    goto label_BF00;

label_C14D:
    DebugPrint("$80/C14D 4C AE BE    JMP $BEAE  [$80:BEAE]  ", a, x, y);
    goto label_BEAE;

label_C150:
    DebugPrint("$80/C150 4C 5D BE    JMP $BE5D  [$80:BE5D]  ", a, x, y);
    goto label_BE5D;

label_C153:
    DebugPrint("$80/C153 4C 0D BE    JMP $BE0D  [$80:BE0D]  ", a, x, y);
    goto label_BE0D;

label_C156:
    DebugPrint("$80/C156 4C BE BD    JMP $BDBE  [$80:BDBE]  ", a, x, y);
    goto label_BDBE;

label_C159:
    DebugPrint("$80/C159 4C 70 BD    JMP $BD70  [$80:BD70]  ", a, x, y);
    goto label_BD70;

label_C15C:
    DebugPrint("$80/C15C 4C 23 BD    JMP $BD23  [$80:BD23]  ", a, x, y);
    goto label_BD23;

label_C17C:
    DebugPrint("$80/C17C A4 74       LDY $74    [$00:0074]  ", a, x, y);
    y = mem73 >> 8;

    DebugPrint("$80/C17E 20 DC C2    JSR $C2DC  [$80:C2DC]  ", a, x, y);
    Fn_80C2DC();

    DebugPrint("$80/C181 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    DebugPrint("$80/C183 20 32 C2    JSR $C232  [$80:C232]  ", a, x, y);
    Fn_80C232();

    DebugPrint("$80/C186 F0 0D       BEQ $0D    [$C195]     ", a, x, y);
    if (z)
    {
        goto label_C195;
    }

    DebugPrint("$80/C188 A4 08       LDY $08    [$00:0008]  ", a, x, y);
    y = mem08;

label_C18A:
    // $80/C18A 8C 80 21    STY $2180  [$99:2180]   A:003E X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C18A 8C 80 21    STY $2180  [$99:2180]  ", a, x, y);

    loaded16.Data16 = y;
    if (indirectHigh == 0x7E && indirectLow >= 0x100)
    {
        cache7E0100[indirectLow - 0x100] = loaded16.Low8;
        cache7E0100[indirectLow + 1 - 0x100] = loaded16.High8;
    }
    else if (indirectHigh == 0x7F)
    {
        cache7F0000[indirectLow] = loaded16.Low8;
        cache7F0000[indirectLow + 1] = loaded16.High8;
    }
    else
    {
        __debugbreak();
    }
    indirectLow += 2;

    // $80/C18D 3A          DEC A                   A:003E X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C18D 3A          DEC A                  ", a, x, y);
    a--;
    z = a == 0;

    // $80/C18E D0 FA       BNE $FA    [$C18A]      A:003D X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C18E D0 FA       BNE $FA    [$C18A]     ", a, x, y);
    if (!z)
    {
        goto label_C18A;
    }

    // $80/C190 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C190 A5 6C       LDA $6C    [$00:006C]  ", a, x, y);
    a = mem6c;

    // $80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BEAE] A:F180 X:0006 Y:0000 P:envmxdizc
    if (x == 6)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BEAE]", a, x, y);
        goto label_BEAE;
    }
    else if (x == 2)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BF53]", a, x, y);
        goto label_BF53;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BE0D]", a, x, y);
        goto label_BE0D;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BDBE]", a, x, y);
        goto label_BDBE;
    }
    else if (x == 8)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BE5D]", a, x, y);
        goto label_BE5D;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BD70]", a, x, y);
        goto label_BD70;
    }
    else if (x == 0x10)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BD23]", a, x, y);
        goto label_BD23;
    }
    else if (x == 4)
    {
        DebugPrint("$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BF00]", a, x, y);
        goto label_BF00;
    }
    else
    {
        __debugbreak(); // notimpl
    }

label_C195:

    DebugPrint("$80/C195 A5 6C       LDA $6C    [$00:006C]  ", a, x, y);
    a = mem6c;
    n = mem6c >= 0x8000;

    DebugPrint("$80/C197 30 16       BMI $16    [$C1AF]     ", a, x, y);
    if (n)
    {
        goto label_C1AF;
    }

    __debugbreak(); // notimpl

label_C1AF:

    DebugPrint("$80/C1AF 60          RTS                    ", a, x, y); // Done
    return;
}


void Fn_80C1B0()
{
    // $80/C1B0 64 6F       STZ $6F    [$00:006F]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B0 64 6F       STZ $6F    [$00:006F]  ", a, x, y);
    mem6f = 0;

    // $80/C1B2 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B2 A5 6C       LDA $6C    [$00:006C]  ", a, x, y);
    a = mem6c;

    // $80/C1B4 0A          ASL A                   A:9665 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B4 0A          ASL A                  ", a, x, y);
    c = a >= 0x8000;
    a *= 2;

    // $80/C1B5 88          DEY                     A:2CCA X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B5 88          DEY                    ", a, x, y);
    --y;
    z = y == 0;

    // $80/C1B6 F0 13       BEQ $13    [$C1CB]      A:2CCA X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C1B6 F0 13       BEQ $13    [$C1CB]     ", a, x, y);
    if (z)
    {
        goto label_C1CB;
    }

label_C1B8:

    // $80/C1B8 90 38       BCC $38    [$C1F2]      A:2CCA X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C1B8 90 38       BCC $38    [$C1F2]     ", a, x, y);
    if (!c)
    {
        goto label_C1F2;
    }

    ShiftRotateDecrement(0xC1BA, 0, 1);

    // $80/C1BE F0 17       BEQ $17    [$C1D7]      A:5994 X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/C1BE F0 17       BEQ $17    [$C1D7]     ", a, x, y);
    if (z)
    {
        goto label_C1D7;
    }

label_C1C0:

    ShiftRotateDecrement(0xC1C0, 0, 1);

    // $80/C1C4 F0 1D       BEQ $1D    [$C1E3]      A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C4 F0 1D       BEQ $1D    [$C1E3]     ", a, x, y);
    if (z)
    {
        goto label_C1E3;
    }

    // $80/C1C6 85 6C       STA $6C    [$00:006C]   A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C6 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    // $80/C1C8 A5 6F       LDA $6F    [$00:006F]   A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C8 A5 6F       LDA $6F    [$00:006F]  ", a, x, y);
    a = mem6f;

    // $80/C1CA 60          RTS                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1CA 60          RTS                    ", a, x, y);
    return;

label_C1CB:

    LoadNextFrom0CInc(0xC1CB);

    // $80/C1D3 A0 08       LDY #$08                A:2212 X:0056 Y:0000 P:envmxdizc
    DebugPrint("$80/C1D3 A0 08       LDY #$08               ", a, x, y);
    y = 0x8;

    // $80/C1D5 80 E1       BRA $E1    [$C1B8]      A:2212 X:0056 Y:0008 P:envmxdizc
    DebugPrint("$80/C1D5 80 E1       BRA $E1    [$C1B8]     ", a, x, y);
    goto label_C1B8;

label_C1D7:

    LoadNextFrom0CInc(0xC1D7);

    // $80/C1DF A0 08       LDY #$08                A:653C X:0004 Y:0000 P:envmxdizc
    DebugPrint("$80/C1DF A0 08       LDY #$08               ", a, x, y);
    y = 0x8;

    // $80/C1E1 80 DD       BRA $DD    [$C1C0]      A:653C X:0004 Y:0008 P:envmxdizc
    DebugPrint("$80/C1E1 80 DD       BRA $DD    [$C1C0]     ", a, x, y);
    goto label_C1C0;

    __debugbreak();

label_C1E3:

    LoadNextFrom0CInc(0xC1E3);

    // $80/C1EB A0 08       LDY #$08                A:811C X:0012 Y:0000 P:envmxdizc
    DebugPrint("$80/C1EB A0 08       LDY #$08               ", a, x, y);
    y = 8;

    // $80/C1ED 85 6C       STA $6C    [$00:006C]   A:811C X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/C1ED 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    // $80/C1EF A5 6F       LDA $6F    [$00:006F]   A:811C X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/C1EF A5 6F       LDA $6F    [$00:006F]  ", a, x, y);
    a = mem6f;

    // $80/C1F1 60          RTS                     A:0002 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/C1F1 60          RTS                    ", a, x, y);
    return;

label_C1F2:

    // $80/C1F2 86 00       STX $00    [$00:0000]   A:A780 X:0008 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F2 86 00       STX $00    [$00:0000]  ", a, x, y);
    mem00.Data16 = x;

    // $80/C1F4 A2 02       LDX #$02                A:A780 X:0008 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F4 A2 02       LDX #$02               ", a, x, y);
    x = 0x2;

label_C1F6:
    // $80/C1F6 0A          ASL A                   A:A780 X:0002 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F6 0A          ASL A                  ", a, x, y);
    c = a >= 0x8000;
    a *= 2;

    // $80/C1F7 88          DEY                     A:4F00 X:0002 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F7 88          DEY                    ", a, x, y);
    --y;
    z = y == 0;

    // $80/C1F8 F0 20       BEQ $20    [$C21A]      A:4F00 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/C1F8 F0 20       BEQ $20    [$C21A]     ", a, x, y);
    if (z)
    {
        LoadNextFrom0CInc(0xC21A);

        // $80/C222 A0 08       LDY #$08                A:91D1 X:0002 Y:0000 P:envmxdizc
        DebugPrint("$80/C222 A0 08       LDY #$08               ", a, x, y);
        y = 0x8;

        // $80/C224 80 D4       BRA $D4    [$C1FA]      A:91D1 X:0002 Y:0008 P:envmxdizc
        DebugPrint("$80/C224 80 D4       BRA $D4    [$C1FA]     ", a, x, y);
        goto label_C1FA;
    }

label_C1FA:
    // $80/C1FA E8          INX                     A:4F00 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FA E8          INX                    ", a, x, y);
    ++x;

    // $80/C1FB 90 F9       BCC $F9    [$C1F6]      A:4F00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FB 90 F9       BCC $F9    [$C1F6]     ", a, x, y);
    if (!c)
    {
        goto label_C1F6;
    }

    // $80/C1FD 86 04       STX $04    [$00:0004]   A:4F00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FD 86 04       STX $04    [$00:0004]  ", a, x, y);
    mem04 = x;

label_C1FF_Finish:

    ShiftRotateDecrement(0xC1FF, 0, 1);

    DebugPrint("$80/C203 F0 21       BEQ $21    [$C226]     ", a, x, y);
    if (z)
    {
        LoadNextFrom0CInc(0xC226);

        DebugPrint("$80/C22E A0 08       LDY #$08               ", a, x, y);
        y = 0x8;

        DebugPrint("$80/C230 80 D3       BRA $D3    [$C205]     ", a, x, y);
    }

    // $80/C205 CA          DEX                     A:9E00 X:0003 Y:0001 P:envmxdizc
    DebugPrint("$80/C205 CA          DEX                    ", a, x, y);
    --x;
    z = x == 0;

    // $80/C206 D0 F7       BNE $F7    [$C1FF]      A:9E00 X:0002 Y:0001 P:envmxdizc
    DebugPrint("$80/C206 D0 F7       BNE $F7    [$C1FF]     ", a, x, y);
    if (!z)
    {
        goto label_C1FF_Finish;
    }

    // $80/C208 85 6C       STA $6C    [$00:006C]   A:7922 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C208 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    // $80/C20A 06 04       ASL $04    [$00:0004]   A:7922 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C20A 06 04       ASL $04    [$00:0004]  ", a, x, y);
    mem04 *= 2;

    // $80/C20C A6 04       LDX $04    [$00:0004]   A:7922 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C20C A6 04       LDX $04    [$00:0004]  ", a, x, y);
    x = mem04;

    // $80/C20E BF B6 C2 80 LDA $80C2B6,x[$80:C2BC] A:7922 X:0006 Y:0007 P:envmxdizc
    DebugPrintWithIndex("$80/C20E BF B6 C2 80 LDA $80C2B6,x[$80:", 0xC2B6 + x, a, x, y, true);
    // X is one of {6,8,A,C,E,10} here. The ROM values are
    static unsigned short s_ROMValueTable_80C2B6[] = { 0, 0, 0, 0x4, 0xC, 0x1C, 0x3C, 0x7C, 0xFC };
    a = s_ROMValueTable_80C2B6[x / 2];

    // $80/C212 A6 00       LDX $00    [$00:0000]   A:0004 X:0006 Y:0007 P:envmxdizc
    DebugPrint("$80/C212 A6 00       LDX $00    [$00:0000]  ", a, x, y);
    x = mem00.Data16;

    // $80/C214 18          CLC                     A:0004 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C214 18          CLC                    ", a, x, y);
    c = false;

    // $80/C215 65 6F       ADC $6F    [$00:006F]   A:0004 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C215 65 6F       ADC $6F    [$00:006F]  ", a, x, y);
    a += mem6f;

    // $80/C217 85 6F       STA $6F    [$00:006F]   A:0006 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C217 85 6F       STA $6F    [$00:006F]  ", a, x, y);
    mem6f = a;

    // $80/C219 60          RTS                     A:0006 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C219 60          RTS                    ", a, x, y);
    return;
}

void Fn_80C232()
{
    // $80/C232 64 6F       STZ $6F    [$00:006F]   A:085C X:000C Y:0000 P:envmxdizc
    DebugPrint("$80/C232 64 6F       STZ $6F    [$00:006F]  ", a, x, y);
    mem6f = 0;

    // $80/C234 A5 6C       LDA $6C    [$00:006C]   A:085C X:000C Y:0000 P:envmxdizc
    DebugPrint("$80/C234 A5 6C       LDA $6C    [$00:006C]  ", a, x, y);
    a = mem6c;

    // $80/C236 0A          ASL A                   A:085C X:000C Y:0000 P:envmxdizc
    DebugPrint("$80/C236 0A          ASL A                  ", a, x, y);
    willCarry = a >= 0x8000;
    a *= 2;
    c = willCarry;

    // $80/C237 CA          DEX                     A:10B8 X:000C Y:0000 P:envmxdizc
    DebugPrint("$80/C237 CA          DEX                    ", a, x, y);
    x--;

    // $80/C238 CA          DEX                     A:10B8 X:000B Y:0000 P:envmxdizc
    DebugPrint("$80/C238 CA          DEX                    ", a, x, y);
    x--;
    z = x == 0;

    // $80/C239 F0 15       BEQ $15    [$C250]      A:10B8 X:000A Y:0000 P:envmxdizc
    DebugPrint("$80/C239 F0 15       BEQ $15    [$C250]     ", a, x, y);
    if (z)
    {
        goto label_C250;
    }

label_C23B:
    // $80/C23B 90 3A       BCC $3A    [$C277]      A:10B8 X:000A Y:0000 P:envmxdizc
    DebugPrint("$80/C23B 90 3A       BCC $3A    [$C277]     ", a, x, y);
    if (!c)
    {
        goto label_C277;
    }

    ShiftRotateDecrement(0xC23D, 2, 0);

    DebugPrint("$80/C242 F0 18       BEQ $18    [$C25C]     ", a, x, y);
    if (z)
    {
        goto label_C25C;
    }

label_C244:
    ShiftRotateDecrement(0xC244, 2, 0);

    DebugPrint("$80/C249 F0 1D       BEQ $1D    [$C268]     ", a, x, y);
    if (z)
    {
        goto label_C268;
    }

    DebugPrint("$80/C24B 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    DebugPrint("$80/C24D A5 6F       LDA $6F    [$00:006F]  ", a, x, y);
    a = mem6f;
    z = a == 0;

    DebugPrint("$80/C24F 60          RTS                    ", a, x, y);
    return;

label_C250:

    LoadNextFrom0CInc(0xC250);

    DebugPrint("$80/C258 A2 10       LDX #$10               ", a, x, y);
    x = 0x10;

    DebugPrint("$80/C25A 80 DF       BRA $DF    [$C23B]     ", a, x, y);
    goto label_C23B;

label_C25C:
    LoadNextFrom0CInc(0xC25C);

    DebugPrint("$80/C264 A2 10       LDX #$10               ", a, x, y);
    x = 0x10;

    DebugPrint("$80/C266 80 DC       BRA $DC    [$C244]     ", a, x, y);
    goto label_C244;

label_C268:

    LoadNextFrom0CInc(0xC268);

    DebugPrint("$80/C270 A2 10       LDX #$10               ", a, x, y);
    x = 0x10;

    DebugPrint("$80/C272 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    DebugPrint("$80/C274 A5 6F       LDA $6F    [$00:006F]  ", a, x, y);
    a = mem6f;
    z = a == 0;

    // $80/C1CA 60          RTS                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C276 60          RTS                    ", a, x, y);
    return;

label_C277:

    // $80/C277 A0 02       LDY #$02                A:10B8 X:000A Y:0000 P:envmxdizc
    DebugPrint("$80/C277 A0 02       LDY #$02               ", a, x, y);
    y = 2;

label_C279:

    // $80/C279 0A          ASL A                   A:10B8 X:000A Y:0002 P:envmxdizc
    DebugPrint("$80/C279 0A          ASL A                  ", a, x, y);
    willCarry = a >= 0x8000;
    a *= 2;
    c = willCarry;

    // $80/C27A CA          DEX                     A:2170 X:000A Y:0002 P:envmxdizc
    DebugPrint("$80/C27A CA          DEX                    ", a, x, y);
    --x;

    // $80/C27B CA          DEX                     A:2170 X:0009 Y:0002 P:envmxdizc
    DebugPrint("$80/C27B CA          DEX                    ", a, x, y);
    --x;
    z = x == 0;

    // $80/C27C F0 24       BEQ $24    [$C2A2]      A:2170 X:0008 Y:0002 P:envmxdizc
    DebugPrint("$80/C27C F0 24       BEQ $24    [$C2A2]     ", a, x, y);
    if (z)
    {
        LoadNextFrom0CInc(0xC2A2);

        // $80/C2AA A2 10       LDX #$10                A:8C2E X:0000 Y:0002 P:envmxdizc
        DebugPrint("$80/C2AA A2 10       LDX #$10               ", a, x, y);
        x = 0x10;

        // $80/C2AC 80 D0       BRA $D0    [$C27E]      A:8C2E X:0010 Y:0002 P:envmxdizc
        DebugPrint("$80/C2AC 80 D0       BRA $D0    [$C27E]     ", a, x, y);
    }

    // $80/C27E C8          INY                     A:2170 X:0008 Y:0002 P:envmxdizc
    DebugPrint("$80/C27E C8          INY                    ", a, x, y);
    y++;

    // $80/C27F 90 F8       BCC $F8    [$C279]      A:2170 X:0008 Y:0003 P:envmxdizc
    DebugPrint("$80/C27F 90 F8       BCC $F8    [$C279]     ", a, x, y);
    if (!c)
    {
        goto label_C279;
    }

    // $80/C281 84 14       STY $14    [$00:0014]   A:0B80 X:0002 Y:0006 P:envmxdizc
    DebugPrint("$80/C281 84 14       STY $14    [$00:0014]  ", a, x, y);
    mem14 = y;

label_C283:

    ShiftRotateDecrement(0xC283, 2, 0);

    // $80/C288 F0 24       BEQ $24    [$C2AE]      A:1700 X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/C288 F0 24       BEQ $24    [$C2AE]     ", a, x, y);
    if (z)
    {
        LoadNextFrom0CInc(0xC2AE);

        // $80/C2B6 A2 10       LDX #$10                A:178C X:0000 Y:0006 P:envmxdizc
        DebugPrint("$80/C2B6 A2 10       LDX #$10               ", a, x, y);
        x = 0x10;

        // $80/C2B8 80 D0       BRA $D0    [$C28A]      A:178C X:0010 Y:0006 P:envmxdizc
        DebugPrint("$80/C2B8 80 D0       BRA $D0    [$C28A]     ", a, x, y);
    }

    // $80/C28A 88          DEY                     A:178C X:0010 Y:0006 P:envmxdizc
    DebugPrint("$80/C28A 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    // $80/C28B D0 F6       BNE $F6    [$C283]      A:178C X:0010 Y:0005 P:envmxdizc
    DebugPrint("$80/C28B D0 F6       BNE $F6    [$C283]     ", a, x, y);
    if (!z)
    {
        goto label_C283;
    }

    // $80/C28D 85 6C       STA $6C    [$00:006C]   A:F180 X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C28D 85 6C       STA $6C    [$00:006C]  ", a, x, y);
    mem6c = a;

    // $80/C28F 9B          TXY                     A:F180 X:0006 Y:0000 P:envmxdizc
    DebugPrint("$80/C28F 9B          TXY                    ", a, x, y);
    y = x;

    // $80/C290 A5 14       LDA $14    [$00:0014]   A:F180 X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C290 A5 14       LDA $14    [$00:0014]  ", a, x, y);
    a = mem14;

    // $80/C292 29 FF 00    AND #$00FF              A:0006 X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C292 29 FF 00    AND #$00FF             ", a, x, y);
    a &= 0xFF;

    // $80/C295 0A          ASL A                   A:0006 X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C295 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/C296 AA          TAX                     A:000C X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C296 AA          TAX                    ", a, x, y);
    x = a;

    // $80/C297 BF B6 C2 80 LDA $80C2B6,x[$80:C2C2] A:000C X:000C Y:0006 P:envmxdizc
    // x is one of {6, 8, A, C, 10}
    if (x == 0x6)
    {
        DebugPrint("$80/C297 BF B6 C2 80 LDA $80C2B6,x[$80:C2BC]", a, x, y);
        a = 0x4;
    }
    else if (x == 0x8)
    {
        DebugPrint("$80/C297 BF B6 C2 80 LDA $80C2B6,x[$80:C2BE]", a, x, y);
        a = 0xC;
    }
    else if (x == 0xA)
    {
        DebugPrint("$80/C297 BF B6 C2 80 LDA $80C2B6,x[$80:C2C0]", a, x, y);
        a = 0x1C;
    }
    else if (x == 0xC)
    {
        DebugPrint("$80/C297 BF B6 C2 80 LDA $80C2B6,x[$80:C2C2]", a, x, y);
        a = 0x3C;
    }
    else if (x == 0xE)
    {
        DebugPrint("$80/C297 BF B6 C2 80 LDA $80C2B6,x[$80:C2C4]", a, x, y);
        a = 0x7C;
    }
    else
    {
        __debugbreak(); // notimpl
    }

    // $80/C29B BB          TYX                     A:003C X:000C Y:0006 P:envmxdizc
    DebugPrint("$80/C29B BB          TYX                    ", a, x, y);
    x = y;

    // $80/C29C 18          CLC                     A:003C X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C29C 18          CLC                    ", a, x, y);

    // $80/C29D 65 6F       ADC $6F    [$00:006F]   A:003C X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C29D 65 6F       ADC $6F    [$00:006F]  ", a, x, y);
    a += mem6f;

    // $80/C29F 85 6F       STA $6F    [$00:006F]   A:003E X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C29F 85 6F       STA $6F    [$00:006F]  ", a, x, y);
    mem6f = a;
    z = a == 0; // Caller expects this.

    // $80/C2A1 60          RTS                     A:003E X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C2A1 60          RTS                    ", a, x, y);
    return;
}

void Fn_80C2DC()
{

label_C2DC_Start:
    // $80/C2DC 0A          ASL A                   A:9420 X:0008 Y:0006 P:envmxdizc
    DebugPrint("$80/C2DC 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $80/C2DD CA          DEX                     A:2840 X:0008 Y:0006 P:envmxdizc
    DebugPrint("$80/C2DD CA          DEX                    ", a, x, y);
    --x;

    // $80/C2DE CA          DEX                     A:2840 X:0007 Y:0006 P:envmxdizc
    DebugPrint("$80/C2DE CA          DEX                    ", a, x, y);
    --x;
    z = x == 0;

    // $80/C2DF F0 04       BEQ $04    [$C2E5]      A:2840 X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C2DF F0 04       BEQ $04    [$C2E5]     ", a, x, y);
    if (z)
    {
        goto label_C2E5;
    }

    // $80/C2E1 88          DEY                     A:2840 X:0006 Y:0006 P:envmxdizc
    DebugPrint("$80/C2E1 88          DEY                    ", a, x, y);
    y--;
    z = y == 0;

    // $80/C2E2 D0 F8       BNE $F8    [$C2DC]      A:2840 X:0006 Y:0005 P:envmxdizc
    DebugPrint("$80/C2E2 D0 F8       BNE $F8    [$C2DC]     ", a, x, y);
    if (!z)
    {
        goto label_C2DC_Start;
    }

    // $80/C2E4 60          RTS                     A:085C X:000C Y:0000 P:envmxdizc
    DebugPrint("$80/C2E4 60          RTS                    ", a, x, y);
    return;

label_C2E5:

    LoadNextFrom0CInc(0xC2E5);

    // $80/C2ED A2 10       LDX #$10                A:4217 X:0000 Y:0003 P:envmxdizc
    DebugPrint("$80/C2ED A2 10       LDX #$10               ", a, x, y);
    x = 0x10;

    // $80/C2EF 88          DEY                     A:4217 X:0010 Y:0003 P:envmxdizc
    DebugPrint("$80/C2EF 88          DEY                    ", a, x, y);
    --y;
    z = y == 0;

    // $80/C2F0 D0 EA       BNE $EA    [$C2DC]      A:4217 X:0010 Y:0002 P:envmxdizc
    DebugPrint("$80/C2F0 D0 EA       BNE $EA    [$C2DC]     ", a, x, y);
    if (!z)
    {
        goto label_C2DC_Start;
    }

    DebugPrint("$80/C2F2 60          RTS                    ", a, x, y);
    return;
}


void Fn_9B85C2()
{
    // The profile images are loaded in "backwards" order. The earliest memory offset one gets loaded last.
    // The output is saved out 2 bytes at a time.
    //
    // This function is called 10 times at the GAME SETUP screen.
    // The 1st invocation writes to somewhere.
    // The 6th invocation writes to 7F5100-7F7380. <-- this run goes through here. This is Kirk Muller's profile image
    //
    // At a high level, this function reads from RAM at 7F0000-7F0484 and writes the final output.
    // The required data must be present in RAM.

    /*
    int pointer0C = 0x7F0000;
    int pointer10 = 0x7F5100;   // For output
    unsigned short mem00 = 0x480;
    unsigned short mem04 = 1;
    unsigned short mem14 = 0;
    unsigned short a = 0x0;
    unsigned short x = 0x0;
    unsigned short y = 0x0;
    bool n = false;
    bool z = false;
    bool c = false;
    bool willCarry = false;
    */

    // Impl
    // $9B/85C2 A6 00       LDX $00    [$00:0000]   A:0000 X:0000 Y:0000 P:envmxdiZc
    DebugPrint("$9B/85C2 A6 00       LDX $00    [$00:0000]  ", a, x, y);
    x = mem00.Data16;

    // $9B/85C4 8B          PHB                     A:0000 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85C4 8B          PHB                    ", a, x, y);

    // $9B/85C5 E2 20       SEP #$20                A:0000 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85C5 E2 20       SEP #$20               ", a, x, y);

    // $9B/85C7 A5 0E       LDA $0E    [$00:000E]   A:0000 X:0480 Y:0000 P:envMxdizc
    DebugPrint("$9B/85C7 A5 0E       LDA $0E    [$00:000E]  ", a, x, y);
    a = mem0e;

    // $9B/85C9 48          PHA                     A:007F X:0480 Y:0000 P:envMxdizc
    DebugPrint("$9B/85C9 48          PHA                    ", a, x, y);

    // $9B/85CA C2 20       REP #$20                A:007F X:0480 Y:0000 P:envMxdizc
    DebugPrint("$9B/85CA C2 20       REP #$20               ", a, x, y);

    // $9B/85CC AB          PLB                     A:007F X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85CC AB          PLB                    ", a, x, y);

    // $9B/85CD 64 04       STZ $04    [$00:0004]   A:007F X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85CD 64 04       STZ $04    [$00:0004]  ", a, x, y);
    mem04 = 0;

    // $9B/85CF A9 FE FF    LDA #$FFFE              A:007F X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85CF A9 FE FF    LDA #$FFFE             ", a, x, y);
    a = 0xFFFE;

    // $9B/85D2 85 06       STA $06    [$00:0006]   A:FFFE X:0480 Y:0000 P:eNvmxdizc
    DebugPrint("$9B/85D2 85 06       STA $06    [$00:0006]  ", a, x, y);
    mem06 = a;

    // $9B/85D4 A5 00       LDA $00    [$00:0000]   A:FFFE X:0480 Y:0000 P:eNvmxdizc
    DebugPrint("$9B/85D4 A5 00       LDA $00    [$00:0000]  ", a, x, y);
    a = mem00.Data16;

    // $9B/85D6 4A          LSR A                   A:0480 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85D6 4A          LSR A                  ", a, x, y);
    a >>= 1;

    // $9B/85D7 4A          LSR A                   A:0240 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85D7 4A          LSR A                  ", a, x, y);
    a >>= 1;

    // $9B/85D8 85 00       STA $00    [$00:0000]   A:0120 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85D8 85 00       STA $00    [$00:0000]  ", a, x, y);
    mem00.Data16 = a;

    // $9B/85DA 80 0C       BRA $0C    [$85E8]      A:0120 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85DA 80 0C       BRA $0C    [$85E8]     ", a, x, y);
    goto label_85E8;

label_85DC:
    // $9B/85DC 8A          TXA                     A:0000 X:0080 Y:0004 P:envmxdiZc
    // $9B/85DD 4A          LSR A                   A:0080 X:0080 Y:0004 P:envmxdizc
    // $9B/85DE 4A          LSR A                   A:0040 X:0080 Y:0004 P:envmxdizc
    // $9B/85DF 4A          LSR A                   A:0020 X:0080 Y:0004 P:envmxdizc
    // $9B/85E0 4A          LSR A                   A:0010 X:0080 Y:0004 P:envmxdizc
    DebugPrint("$9B/85DC 8A          TXA                    ", a, x, y);
    willCarry = x < 16;
    a = x;
    z = a == 0;

    DebugPrint("$9B/85DD 4A          LSR A                  ", a, x, y);
    a >>= 1;

    DebugPrint("$9B/85DE 4A          LSR A                  ", a, x, y);
    a >>= 1;

    DebugPrint("$9B/85DF 4A          LSR A                  ", a, x, y);
    a >>= 1;

    DebugPrint("$9B/85E0 4A          LSR A                  ", a, x, y);
    a >>= 1;

    // $9B/85E1 F0 54       BEQ $54    [$8637]      A:0008 X:0080 Y:0004 P:envmxdizc
    z = a == 0;
    c = willCarry;
    DebugPrint("$9B/85E1 F0 54       BEQ $54    [$8637]     ", a, x, y);

    if (x / 16 == 0)
    {
        goto label_8637;
    }

    DebugPrint("$9B/85E3 AA          TAX                    ", a, x, y);
    x = a;

    DebugPrint("$9B/85E4 C8          INY                    ", a, x, y);
    y++;

    DebugPrint("$9B/85E5 C8          INY                    ", a, x, y);
    y++;

    DebugPrint("$9B/85E6 80 0C       BRA $0C    [$85F4]     ", a, x, y);

    goto label_85F4;

label_85E8:
    // $9B/85E8 A2 80 00    LDX #$0080              A:0000 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/85E8 A2 80 00    LDX #$0080             ", a, x, y);
    x = 0x80;

    // $9B/85EB 64 14       STZ $14    [$00:0014]   A:0000 X:0080 Y:0012 P:envmxdizc
    DebugPrint("$9B/85EB 64 14       STZ $14    [$00:0014]  ", a, x, y);
    mem14 = 0;

    // $9B/85ED 64 16       STZ $16    [$00:0016]   A:0120 X:0080 Y:0000 P:envmxdizc
    DebugPrint("$9B/85ED 64 16       STZ $16    [$00:0016]  ", a, x, y);
    mem16 = 0;
    {
        // $9B/85EF A5 04       LDA $04    [$00:0004]   A:0000 X:0080 Y:0010 P:envmxdizc
        // $9B/85F1 0A          ASL A                   A:0001 X:0080 Y:0010 P:envmxdizc
        // $9B/85F2 0A          ASL A                   A:0002 X:0080 Y:0010 P:envmxdizc
        // $9B/85F3 A8          TAY                     A:0004 X:0080 Y:0010 P:envmxdizc
        DebugPrint("$9B/85EF A5 04       LDA $04    [$00:0004]  ", a, x, y);
        a = mem04;

        DebugPrint("$9B/85F1 0A          ASL A                  ", a, x, y);
        a *= 2;

        DebugPrint("$9B/85F2 0A          ASL A                  ", a, x, y);
        a *= 2;

        DebugPrint("$9B/85F3 A8          TAY                    ", mem04 * 4, x, y);
        y = a;
    }

label_85F4:
    {
        // $9B/85F4 B1 0C       LDA ($0C),y[$7F:0006]   A:0008 X:0008 Y:0006 P:envmxdizc
        DebugPrintWithIndex("$9B/85F4 B1 0C       LDA ($0C),y[$7F:0006]  ", mem0c, a, x, y);
        loaded16.Low8 = cache7F0000[mem0c];
        loaded16.High8 = cache7F0000[mem0c+1];
        a = loaded16.Data16;
        z = a == 0;

        // $9B/85F6 F0 E4       BEQ $E4    [$85DC]      A:0000 X:0008 Y:0006 P:envmxdiZc
        DebugPrint("$9B/85F6 F0 E4       BEQ $E4    [$85DC]     ", a, x, y);
        if (z)
        {
            goto label_85DC;
        }
    }

    // $9B/85F8 EB          XBA                     A:0800 X:0008 Y:003E P:envmxdizc
    DebugPrint("$9B/85F8 EB          XBA                    ", a, x, y);
    a = ExchangeShortHighAndLow(a);

    // $9B/85F9 A8          TAY                     A:0008 X:0008 Y:003E P:envmxdizc
    DebugPrint("$9B/85F9 A8          TAY                    ", a, x, y);
    y = a;

label_85FA:
    // $9B/85FA 98          TYA                     A:0008 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/85FA 98          TYA                    ", a, x, y);
    a = y;
    n = false;

    // $9B/85FB 0A          ASL A                   A:0008 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/85FB 0A          ASL A                  ", a, x, y);
    willCarry = a >= 0x8000;
    a *= 2;
    c = willCarry;

    // $9B/85FC 90 05       BCC $05    [$8603]      A:0010 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/85FC 90 05       BCC $05    [$8603]     ", a, x, y);
    if (c == false)
    {
        goto label_8603;
    }

    // $9B/85FE A8          TAY                     A:0000 X:0001 Y:8000 P:envmxdizc
    DebugPrint("$9B/85FE A8          TAY                    ", a, x, y);
    y = a;

    // $9B/85FF 8A          TXA                     A:0000 X:0001 Y:0000 P:envmxdizc
    DebugPrint("$9B/85FF 8A          TXA                    ", a, x, y);
    a = x;

    // $9B/8600 04 15       TSB $15    [$00:0015]   A:0001 X:0001 Y:0000 P:envmxdizc
    DebugPrint("$9B/8600 04 15       TSB $15    [$00:0015]  ", a, x, y);
    mem14 |= (a << 8); // This is a bit sketchy

    // $9B/8602 98          TYA                     A:0001 X:0001 Y:0000 P:envmxdizc
    DebugPrint("$9B/8602 98          TYA                    ", a, x, y);
    a = y;

label_8603:
    // $9B/8603 0A          ASL A                   A:0000 X:0001 Y:0000 P:envmxdizc
    DebugPrint("$9B/8603 0A          ASL A                  ", a, x, y);
    willCarry = a >= 0x8000;
    a *= 2;
    c = willCarry;

    // $9B/8604 90 05       BCC $05    [$860B]      A:0020 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/8604 90 05       BCC $05    [$860B]     ", a, x, y);
    if (c == false)
    {
        goto label_860B;
    }

    // $9B/8606 A8          TAY                     A:2E6C X:0008 Y:9736 P:envmxdizc
    DebugPrint("$9B/8606 A8          TAY                    ", a, x, y);
    y = a;

    // $9B/8607 8A          TXA                     A:2E6C X:0008 Y:2E6C P:envmxdizc
    DebugPrint("$9B/8607 8A          TXA                    ", a, x, y);
    a = x;

    // $9B/8608 04 14       TSB $14    [$00:0014]   A:0008 X:0008 Y:2E6C P:envmxdizc
    DebugPrint("$9B/8608 04 14       TSB $14    [$00:0014]  ", a, x, y);
    mem14 |= a;

    // $9B/860A 98          TYA                     A:0008 X:0008 Y:2E6C P:envmxdizc
    DebugPrint("$9B/860A 98          TYA                    ", a, x, y);
    a = y;

label_860B:
    // $9B/860B 0A          ASL A                   A:0010 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/860B 0A          ASL A                  ", a, x, y);
    willCarry = a >= 0x8000;
    a *= 2;
    c = willCarry;

    // $9B/860C 90 05       BCC $05    [$8613]      A:0040 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/860C 90 05       BCC $05    [$8613]     ", a, x, y);
    if (c == false)
    {
        goto label_8613;
    }

    // $9B/860E A8          TAY                     A:DDC8 X:0008 Y:7772 P:envmxdizc
    DebugPrint("$9B/860E A8          TAY                    ", a, x, y);
    y = a;

    // $9B/860F 8A          TXA                     A:DDC8 X:0008 Y:DDC8 P:envmxdizc
    DebugPrint("$9B/860F 8A          TXA                    ", a, x, y);
    a = x;

    // $9B/8610 04 17       TSB $17    [$00:0017]   A:0008 X:0008 Y:DDC8 P:envmxdizc
    DebugPrint("$9B/8610 04 17       TSB $17    [$00:0017]  ", a, x, y);
    mem16 |= (a << 8);

    // $9B/8612 98          TYA                     A:0008 X:0008 Y:DDC8 P:envmxdizc
    DebugPrint("$9B/8612 98          TYA                    ", a, x, y);
    a = y;

label_8613:
    // $9B/8613 0A          ASL A                   A:0040 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/8613 0A          ASL A                  ", a, x, y);
    willSetNegative = a >= 0x4000;
    willCarry = a >= 0x8000;
    a *= 2;
    n = willSetNegative;
    c = willCarry;

    // $9B/8614 90 05       BCC $05    [$861B]      A:0080 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/8614 90 05       BCC $05    [$861B]     ", a, x, y);
    if (c == false)
    {
        goto label_861B;
    }

    // $9B/8616 A8          TAY                     A:BB90 X:0008 Y:DDC8 P:envmxdizc
    DebugPrint("$9B/8616 A8          TAY                    ", a, x, y);
    y = a;

    // $9B/8617 8A          TXA                     A:BB90 X:0008 Y:BB90 P:envmxdizc
    DebugPrint("$9B/8617 8A          TXA                    ", a, x, y);
    a = x;

    // $9B/8618 04 16       TSB $16    [$00:0016]   A:0008 X:0008 Y:BB90 P:envmxdizc
    DebugPrint("$9B/8618 04 16       TSB $16    [$00:0016]  ", a, x, y);
    mem16 |= a;

    // $9B/861A 98          TYA                     A:0008 X:0008 Y:BB90 P:envmxdizc
    DebugPrint("$9B/861A 98          TYA                    ", a, x, y);
    a = y;

label_861B:
    // $9B/861B A8          TAY                     A:0080 X:0008 Y:0008 P:envmxdizc
    DebugPrint("$9B/861B A8          TAY                    ", a, x, y);
    y = a;

    // $9B/861C 8A          TXA                     A:0080 X:0008 Y:0080 P:envmxdizc
    DebugPrint("$9B/861C 8A          TXA                    ", a, x, y);
    a = x;
    n = false;

    // $9B/861D 4A          LSR A                   A:0008 X:0008 Y:0080 P:envmxdizc
    DebugPrint("$9B/861D 4A          LSR A                  ", a, x, y);
    a >>= 1;

    // $9B/861E AA          TAX                     A:0004 X:0008 Y:0080 P:envmxdizc
    DebugPrint("$9B/861E AA          TAX                    ", a, x, y);
    x = a;

    // $9B/861F E0 10 00    CPX #$0010              A:0004 X:0004 Y:0080 P:envmxdizc
    DebugPrint("$9B/861F E0 10 00    CPX #$0010             ", a, x, y);
    c = x >= 0x10;
    n = x < 0x10;

    // $9B/8622 B0 D6       BCS $D6    [$85FA]      A:0004 X:0004 Y:0080 P:eNvmxdizc
    DebugPrint("$9B/8622 B0 D6       BCS $D6    [$85FA]     ", a, x, y);
    if (c)
    {
        goto label_85FA;
    }

    // $9B/8624 E0 00 00    CPX #$0000              A:0004 X:0004 Y:0080 P:eNvmxdizc
    DebugPrint("$9B/8624 E0 00 00    CPX #$0000             ", a, x, y);
    z = x == 0;
    c = x >= 0x0;
    n = x < 0x0;

    // $9B/8627 F0 0E       BEQ $0E    [$8637]      A:0004 X:0004 Y:0080 P:envmxdizC
    DebugPrint("$9B/8627 F0 0E       BEQ $0E    [$8637]     ", a, x, y);
    if (z)
    {
        goto label_8637;
    }

    // $9B/8629 E0 08 00    CPX #$0008              A:0004 X:0004 Y:0080 P:envmxdizC
    DebugPrint("$9B/8629 E0 08 00    CPX #$0008             ", a, x, y);
    c = x >= 0x8;
    n = x < 0x8;

    // $9B/862C 90 CC       BCC $CC    [$85FA]      A:0004 X:0004 Y:0080 P:eNvmxdizc
    DebugPrint("$9B/862C 90 CC       BCC $CC    [$85FA]     ", a, x, y);
    if (c == false)
    {
        goto label_85FA;
    }

    // $9B/862E A5 04       LDA $04    [$00:0004]   A:0008 X:0008 Y:0000 P:envmxdizc
    DebugPrint("$9B/862E A5 04       LDA $04    [$00:0004]  ", a, x, y);
    a = mem04;

    // $9B/8630 0A          ASL A                   A:0014 X:0008 Y:0000 P:envmxdizc
    DebugPrint("$9B/8630 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $9B/8631 0A          ASL A                   A:0028 X:0008 Y:0000 P:envmxdizc
    DebugPrint("$9B/8631 0A          ASL A                  ", a, x, y);
    a *= 2;

    // $9B/8632 A8          TAY                     A:0050 X:0008 Y:0000 P:envmxdizc
    DebugPrint("$9B/8632 A8          TAY                    ", a, x, y);
    y = a;

    // $9B/8633 C8          INY                     A:0050 X:0008 Y:0050 P:envmxdizc
    DebugPrint("$9B/8633 C8          INY                    ", a, x, y);
    ++y;

    // $9B/8634 C8          INY                     A:0050 X:0008 Y:0051 P:envmxdizc
    DebugPrint("$9B/8634 C8          INY                    ", a, x, y);
    ++y;

    // $9B/8635 80 BD       BRA $BD    [$85F4]      A:0050 X:0008 Y:0052 P:envmxdizc
    DebugPrint("$9B/8635 80 BD       BRA $BD    [$85F4]     ", a, x, y);
    goto label_85F4;

label_8637:
    // $9B/8637 A5 06       LDA $06    [$00:0006]   A:0000 X:0008 Y:0006 P:envmxdiZC
    DebugPrint("$9B/8637 A5 06       LDA $06    [$00:0006]  ", a, x, y);
    a = mem06;
    z = a == 0;

    // $9B/8639 1A          INC A                   A:0000 X:0008 Y:0006 P:envmxdiZC
    DebugPrint("$9B/8639 1A          INC A                  ", a, x, y);
    ++a;
    z = a == 0;

    // $9B/863A 1A          INC A                   A:0001 X:0008 Y:0006 P:envmxdizC
    DebugPrint("$9B/863A 1A          INC A                  ", a, x, y);
    ++a;
    z = a == 0;

    // $9B/863B A8          TAY                     A:0002 X:0008 Y:0006 P:envmxdizC
    DebugPrint("$9B/863B A8          TAY                    ", a, x, y);
    y = a;

    y = mem06 + 2;

    // $9B/863C 29 10 00    AND #$0010              A:0002 X:0008 Y:0002 P:envmxdizC
    DebugPrint("$9B/863C 29 10 00    AND #$0010             ", a, x, y);
    a = a & 0x10;
    z = a == 0;

    unsigned short check = y & 0x10;

    // $9B/863F F0 06       BEQ $06    [$8647]      A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/863F F0 06       BEQ $06    [$8647]     ", a, x, y);

    if (check == 0)
    {
        goto label_8647;
    }

    // $9B/8641 98          TYA                     A:0010 X:0008 Y:0010 P:envmxdizC
    DebugPrint("$9B/8641 98          TYA                    ", a, x, y);
    a = y;

    // $9B/8642 18          CLC                     A:0010 X:0008 Y:0010 P:envmxdizC
    DebugPrint("$9B/8642 18          CLC                    ", a, x, y);
    c = false;

    // $9B/8643 69 10 00    ADC #$0010              A:0010 X:0008 Y:0010 P:envmxdizc
    DebugPrint("$9B/8643 69 10 00    ADC #$0010             ", a, x, y);
    a += 0x10;

    // $9B/8646 A8          TAY                     A:0020 X:0008 Y:0010 P:envmxdizc
    DebugPrint("$9B/8646 A8          TAY                    ", a, x, y);
    y = a;

label_8647:
    // $9B/8647 84 06       STY $06    [$00:0006]   A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/8647 84 06       STY $06    [$00:0006]  ", a, x, y);
    mem06 = y;

    // $9B/8649 A5 16       LDA $16    [$00:0016]   A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/8649 A5 16       LDA $16    [$00:0016]  ", a, x, y);
    a = mem16;
    z = a == 0;

    // $9B/864B 91 10       STA ($10),y[$7F:5102]   A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint864B(a, x, y);

    //WriteDecompressedOutput(pointer10 + y, mem16); // xxx

    // $9B/864D 98          TYA                     A:0000 X:0008 Y:0002 P:envmxdiZC
    DebugPrint("$9B/864D 98          TYA                    ", a, x, y);
    a = y;
    z = a == 0;

    // $9B/864E 18          CLC                     A:0002 X:0008 Y:0002 P:envmxdizC
    DebugPrint("$9B/864E 18          CLC                    ", a, x, y);
    c = false;

    // $9B/864F 69 10 00    ADC #$0010              A:0002 X:0008 Y:0002 P:envmxdizc
    DebugPrint("$9B/864F 69 10 00    ADC #$0010             ", a, x, y);
    a += 0x10;

    // $9B/8652 A8          TAY                     A:0012 X:0008 Y:0002 P:envmxdizc
    DebugPrint("$9B/8652 A8          TAY                    ", a, x, y);
    y = a;

    // $9B/8653 A5 14       LDA $14    [$00:0014]   A:0012 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/8653 A5 14       LDA $14    [$00:0014]  ", a, x, y);
    a = mem14;
    z = a == 0;
    c = false;

    // $9B/8655 91 10       STA ($10),y[$7F:5112]   A:0000 X:0008 Y:0012 P:envmxdiZc
    DebugPrint8655(a, x, y);

    //WriteDecompressedOutput(pointer10 + y, mem14); // xxx

    // $9B/8657 E6 04       INC $04    [$00:0004]   A:0000 X:0008 Y:0012 P:envmxdiZc
    DebugPrint("$9B/8657 E6 04       INC $04    [$00:0004]  ", a, x, y);
    mem04++;
    z = mem04 == 0;

    // $9B/8659 C6 00       DEC $00    [$00:0000]   A:0000 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/8659 C6 00       DEC $00    [$00:0000]  ", a, x, y);
    mem00.Data16--;
    n = mem00.Data16 >= 0x8000;

    // $9B/865B 10 8B       BPL $8B    [$85E8]      A:0000 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/865B 10 8B       BPL $8B    [$85E8]     ", a, x, y);

    if (!n)
    {
        goto label_85E8;
    }

    // $9B/865D AB          PLB                     A:0000 X:0008 Y:0490 P:envmxdizc
    DebugPrint("$9B/865D AB          PLB                    ", a, x, y);

    // $9B/865E 6B          RTL                     A:0000 X:0008 Y:0490 P:envmxdizc
    DebugPrint("$9B/865E 6B          RTL                    ", a, x, y);
}

void Filler()
{
    // There's a PLB that does this
    x = 0x480;

    DebugPrint("$9D/CC7F 86 00       STX $00    [$00:0000]  ", a, x, y);
    mem00.Data16 = x;

    DebugPrint("$9D/CC81 A3 01       LDA $01,s  [$00:1FF6]  ", a, x, y);
    a = 0; // Need to check what actually gets pushed

    DebugPrint("$9D/CC83 AA          TAX                    ", a, x, y);
    x = a;

    DebugPrint("$9D/CC84 A5 91       LDA $91    [$00:0091]  ", a, x, y);
    a = 0; // Need to check it's actually this value
    z = a == 0;

    DebugPrint("$9D/CC86 F0 05       BEQ $05    [$CC8D]     ", a, x, y);
    if (z)
    {
        goto label_CC8D;
    }
    __debugbreak(); // notimpl

label_CC8D:
    DebugPrint("$9D/CC8D A9 00 51    LDA #$5100             ", a, x, y);
    a = 0x5100;

    DebugPrint("$9D/CC90 18          CLC                    ", a, x, y);

    DebugPrint("$9D/CC91 7F AE CC 9D ADC $9DCCAE,x[$9D:CCAE]", a, x, y);
    if (x == 0)
    {
        a += 0;
    }
    else
    {
        __debugbreak();
    }

    DebugPrint("$9D/CC95 85 10       STA $10    [$00:0010]  ", a, x, y);
    mem10 = a;

    DebugPrint("$9D/CC97 A9 7F 00    LDA #$007F             ", a, x, y);
    a = 0x7F;

    DebugPrint("$9D/CC9A 85 0E       STA $0E    [$00:000E]  ", a, x, y);
    mem0e = a;

    DebugPrint("$9D/CC9C A9 00 00    LDA #$0000             ", a, x, y);
    a = 0;

    DebugPrint("$9D/CC9F 85 0C       STA $0C    [$00:000C]  ", a, x, y);
    mem0c = a;
}

void InitializeCaches()
{
    cache7E0100.resize(0x100);
    memset(cache7E0100.data(), 0, cache7E0100.size());

    // Initialize output.
    mem7E0500_7E0700.resize(0x200);
    memset(mem7E0500_7E0700.data(), 0, mem7E0500_7E0700.size());

    cache7E0700.resize(0x14);
    memset(cache7E0700.data(), 0, cache7E0700.size());

    cache7E0720.resize(0x20);
    memset(cache7E0720.data(), 0, cache7E0720.size());

    cache7E0740.resize(0x20);
    memset(cache7E0740.data(), 0, cache7E0740.size());

    cache7E0760.resize(0x2);
    memset(cache7E0760.data(), 0, cache7E0760.size());

    cache7F0000.resize(0x1000);
    memset(cache7F0000.data(), 0, cache7F0000.size());
}

int main()
{
    OpenDebugLog();

    // Load ROM file
    romFile = LoadBinaryFile8("nhl94.sfc");

    InitializeCaches();

    Fn_80BBB3();

    Filler();

    DebugPrint("$9D/CCA1 22 C2 85 9B JSL $9B85C2[$9B:85C2]  ", a, x, y);
    Fn_9B85C2();

    return 0;
}