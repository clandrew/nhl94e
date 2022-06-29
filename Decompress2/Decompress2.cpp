// Decompress2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <vector>
#include <fstream>
#include "Util.h"

std::vector<unsigned short> expected;
int validationLength = 2;

unsigned short IncLow8(unsigned short s)
{
    unsigned char ch = s & 0xFF;
    ++ch;
    s = ch;
    return s;
}

void Fn_80BBB3()
{
    unsigned short a = 0xFB30;
    unsigned short x = 0x0480;
    unsigned short y = 0xF8AE;
    bool n = false;
    bool z = false;
    bool c = false;
    unsigned short mem0c = 0xF8AC;
    unsigned short mem14 = 0;
    unsigned short mem6c = 0;
    unsigned short mem73 = 0;
    unsigned short mem75 = 0;
    unsigned short mem77 = 0;

    // This function writes to 7F0000-7F0484.
    // This is a sizeable function, a.k.a. 'the monstrosity'.
    // It does a buch of sequential-ish (skipping a couple bytes) starting from ROM offset 99F8B1, ending at 99FAB4.
    // That corresponds to file offset 0xCF8B1, through to 0xCFAB4.

    // Use 8bit X and Y
    // $80/BBB3 E2 10       SEP #$10                A:FB30 X:0480 Y:F8AE P:envmxdizc
    DebugPrint("$80/BBB3 E2 10       SEP #$10               ", a, x, y, n, z, c);
    x &= 0xFF;
    y &= 0xFF;
    
    // $80/BBB5 A5 0C       LDA $0C    [$00:000C]   A:FB30 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBB5 A5 0C       LDA $0C    [$00:000C]  ", a, x, y, n, z, c);
    a = mem0c;

    // $80/BBB7 18          CLC                     A:F8AC X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBB7 18          CLC                    ", a, x, y, n, z, c);
    c = false;

    // $80/BBB8 69 05 00    ADC #$0005              A:F8AC X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBB8 69 05 00    ADC #$0005             ", a, x, y, n, z, c);
    a += 5;

    // $80/BBBB 85 0C       STA $0C    [$00:000C]   A:F8B1 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBBB 85 0C       STA $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c = a;

    // $80/BBBD B2 0C       LDA ($0C)  [$99:F8B1]   A:F8B1 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBBD B2 0C       LDA ($0C)  [$99:F8B1]  ", a, x, y, n, z, c);
    a = LoadFromROMFragment(0x990000 | mem0c);

    // $80/BBBF 85 73       STA $73    [$00:0073]   A:960F X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBBF 85 73       STA $73    [$00:0073]  ", a, x, y, n, z, c);
    mem73 = a;

    // $80/BBC1 E6 0C       INC $0C    [$00:000C]   A:960F X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC1 E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/BBC3 B2 0C       LDA ($0C)  [$99:F8B2]   A:960F X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC3 B2 0C       LDA ($0C)  [$99:F8B2]  ", a, x, y, n, z, c);
    a = LoadFromROMFragment(0x990000 | mem0c);

    // $80/BBC5 E6 0C       INC $0C    [$00:000C]   A:6596 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC5 E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/BBC7 E6 0C       INC $0C    [$00:000C]   A:6596 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC7 E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/BBC9 EB          XBA                     A:6596 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBC9 EB          XBA                    ", a, x, y, n, z, c);
    a = ExchangeShortHighAndLow(a);

    // $80/BBCA 85 6C       STA $6C    [$00:006C]   A:9665 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBCA 85 6C       STA $6C    [$00:006C]  ", a, x, y, n, z, c);
    mem6c = a;

    // $80/BBCC A0 08       LDY #$08                A:9665 X:0080 Y:00AE P:envmxdizc
    DebugPrint("$80/BBCC A0 08       LDY #$08               ", a, x, y, n, z, c);
    y = 8;

    // $80/BBCE 64 77       STZ $77    [$00:0077]   A:9665 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBCE 64 77       STZ $77    [$00:0077]  ", a, x, y, n, z, c);
    mem77 = 0;

    // $80/BBD0 64 75       STZ $75    [$00:0075]   A:9665 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBD0 64 75       STZ $75    [$00:0075]  ", a, x, y, n, z, c);
    mem75 = 0;

    // $80/BBD2 A9 10 00    LDA #$0010              A:9665 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBD2 A9 10 00    LDA #$0010             ", a, x, y, n, z, c);
    a = 0x10;

    // $80/BBD5 85 14       STA $14    [$00:0014]   A:0010 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBD5 85 14       STA $14    [$00:0014]  ", a, x, y, n, z, c);
    mem14 = a;

    // $80/BBD7 A2 FE       LDX #$FE                A:0010 X:0080 Y:0008 P:envmxdizc
    DebugPrint("$80/BBD7 A2 FE       LDX #$FE               ", a, x, y, n, z, c);
    x = 0xFE;

    // $80/BBD9 E8          INX                     A:0010 X:00FE Y:0008 P:envmxdizc
    DebugPrint("$80/BBD9 E8          INX                    ", a, x, y, n, z, c);
    x = IncLow8(x);

    // $80/BBDA E8          INX                     A:0010 X:00FF Y:0008 P:envmxdizc
    DebugPrint("$80/BBDA E8          INX                    ", a, x, y, n, z, c);
    x = IncLow8(x);

    // $80/BBDB C6 14       DEC $14    [$00:0014]   A:0010 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBDB C6 14       DEC $14    [$00:0014]  ", a, x, y, n, z, c);
    mem14--;

    // $80/BBDD 06 75       ASL $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBDD 06 75       ASL $75    [$00:0075]  ", a, x, y, n, z, c);
    mem75 *= 2;

    // $80/BBDF A5 75       LDA $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBDF A5 75       LDA $75    [$00:0075]  ", a, x, y, n, z, c);
    a = mem75;

    // $80/BBE1 38          SEC                     A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBE1 38          SEC                    ", a, x, y, n, z, c);
    c = true;

    // $80/BBE2 E5 77       SBC $77    [$00:0077]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBE2 E5 77       SBC $77    [$00:0077]  ", a, x, y, n, z, c);
    a -= mem77;

    // $80/BBE4 9D 20 07    STA $0720,x[$99:0720]   A:0000 X:0000 Y:0008 P:envmxdizc
    // $80/BBE7 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0000 X:0000 Y:0008 P:envmxdizc
}


int main()
{
    OpenDebugLog();
    LoadROMFragment();

    Fn_80BBB3();

    return 0;

}