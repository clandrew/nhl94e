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

void RotateLeft(unsigned short* pS, bool* pC)
{
    bool c = *pS >= 0x8000;
    *pS = *pS << 1;
    if (*pC)
    {
        *pS |= 0x1;
    }
    *pC = c;
}

unsigned short a = 0xFB30;
unsigned short x = 0x0480;
unsigned short y = 0xF8AE;
bool n = false;
bool z = false;
bool c = false;
unsigned short mem00 = 0;
unsigned short mem04 = 0;
unsigned short mem0c = 0xF8AC;
unsigned short mem14 = 0;
unsigned short mem6c = 0;
unsigned short mem6f = 0;
unsigned short mem73 = 0;
unsigned short mem75 = 0;
unsigned short mem77 = 0;

void Fn_C1B0()
{
    DebugPrintNewline();

    bool willCarry = false;
    unsigned short loaded = 0;

    // $80/C1B0 64 6F       STZ $6F    [$00:006F]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B0 64 6F       STZ $6F    [$00:006F]  ", a, x, y, n, z, c);
    mem6f = 0;

    // $80/C1B2 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B2 A5 6C       LDA $6C    [$00:006C]  ", a, x, y, n, z, c);
    a = mem6c;

    // $80/C1B4 0A          ASL A                   A:9665 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B4 0A          ASL A                  ", a, x, y, n, z, c);
    c = a >= 0x8000;
    a *= 2;

    // $80/C1B5 88          DEY                     A:2CCA X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/C1B5 88          DEY                    ", a, x, y, n, z, c);
    --y;
    z = y == 0;

    // $80/C1B6 F0 13       BEQ $13    [$C1CB]      A:2CCA X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C1B6 F0 13       BEQ $13    [$C1CB]     ", a, x, y, n, z, c);    
    if (z)
    {
        __debugbreak(); // notimpl
    }

    // $80/C1B8 90 38       BCC $38    [$C1F2]      A:2CCA X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C1B8 90 38       BCC $38    [$C1F2]     ", a, x, y, n, z, c);
    if (!c)
    {
        goto label_C1F2;
    }

    // $80/C1BA 0A          ASL A                   A:2CCA X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C1BA 0A          ASL A                  ", a, x, y, n, z, c);
    c = a >= 0x8000;
    a *= 2;

    // $80/C1BB 26 6F       ROL $6F    [$00:006F]   A:5994 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C1BB 26 6F       ROL $6F    [$00:006F]  ", a, x, y, n, z, c);
    RotateLeft(&mem6f, &c);

    // $80/C1BD 88          DEY                     A:5994 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C1BD 88          DEY                    ", a, x, y, n, z, c);
    --y;
    z = y == 0;

    // $80/C1BE F0 17       BEQ $17    [$C1D7]      A:5994 X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/C1BE F0 17       BEQ $17    [$C1D7]     ", a, x, y, n, z, c);
    if (z)
    {
        goto label_C1D7;
    }

label_C1C0:
    // $80/C1C0 0A          ASL A                   A:5994 X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/C1C0 0A          ASL A                  ", a, x, y, n, z, c);
    c = a >= 0x8000;
    a *= 2;

    // $80/C1C1 26 6F       ROL $6F    [$00:006F]   A:B328 X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/C1C1 26 6F       ROL $6F    [$00:006F]  ", a, x, y, n, z, c);
    RotateLeft(&mem6f, &c);

    // $80/C1C3 88          DEY                     A:B328 X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/C1C3 88          DEY                    ", a, x, y, n, z, c);
    --y;
    z = y == 0;

    // $80/C1C4 F0 1D       BEQ $1D    [$C1E3]      A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C4 F0 1D       BEQ $1D    [$C1E3]     ", a, x, y, n, z, c);
    if (z)
    {
        __debugbreak(); // notimpl
        //goto label_C1C4;
    }

    // $80/C1C6 85 6C       STA $6C    [$00:006C]   A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C6 85 6C       STA $6C    [$00:006C]  ", a, x, y, n, z, c);
    mem6c = a;

    // $80/C1C8 A5 6F       LDA $6F    [$00:006F]   A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C8 A5 6F       LDA $6F    [$00:006F]  ", a, x, y, n, z, c);
    a = mem6f;
    
    // $80/C1CA 60          RTS                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1CA 60          RTS                    ", a, x, y, n, z, c);
    DebugPrintNewline();
    DebugPrintNewline();
    return;
    
label_C1D7:

    // $80/C1D7 E2 20       SEP #$20                A:6500 X:0004 Y:0000 P:envmxdizc
    DebugPrint("$80/C1D7 E2 20       SEP #$20               ", a, x, y, n, z, c);
    
    // $80/C1D9 B2 0C       LDA ($0C)  [$99:F8B4]   A:6500 X:0004 Y:0000 P:envmxdizc
    DebugPrintWithAddress("$80/C1D9 B2 0C       LDA ($0C)  [$99:", mem0c, a, x, y, n, z, c);
    loaded = LoadFromROMFragment(0x990000 | mem0c);
    a &= 0xFF00;
    a |= loaded & 0xFF;

    // $80/C1DB C2 20       REP #$20                A:653C X:0004 Y:0000 P:envmxdizc
    DebugPrint("$80/C1DB C2 20       REP #$20               ", a, x, y, n, z, c);
   
    // $80/C1DD E6 0C       INC $0C    [$00:000C]   A:653C X:0004 Y:0000 P:envmxdizc
    DebugPrint("$80/C1DD E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/C1DF A0 08       LDY #$08                A:653C X:0004 Y:0000 P:envmxdizc
    DebugPrint("$80/C1DF A0 08       LDY #$08               ", a, x, y, n, z, c);
    y = 0x8;
   
    // $80/C1E1 80 DD       BRA $DD    [$C1C0]      A:653C X:0004 Y:0008 P:envmxdizc
    DebugPrint("$80/C1E1 80 DD       BRA $DD    [$C1C0]     ", a, x, y, n, z, c);
    goto label_C1C0;

label_C1F2:

    // $80/C1F2 86 00       STX $00    [$00:0000]   A:A780 X:0008 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F2 86 00       STX $00    [$00:0000]  ", a, x, y, n, z, c);
    mem00 = x;

    // $80/C1F4 A2 02       LDX #$02                A:A780 X:0008 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F4 A2 02       LDX #$02               ", a, x, y, n, z, c);
    x = 0x2;

    // $80/C1F6 0A          ASL A                   A:A780 X:0002 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F6 0A          ASL A                  ", a, x, y, n, z, c);
    c = a >= 0x8000;
    a *= 2;

    // $80/C1F7 88          DEY                     A:4F00 X:0002 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F7 88          DEY                    ", a, x, y, n, z, c);
    --y;
    z = y == 0;

    // $80/C1F8 F0 20       BEQ $20    [$C21A]      A:4F00 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/C1F8 F0 20       BEQ $20    [$C21A]     ", a, x, y, n, z, c);
    if (z)
    {
        __debugbreak(); // notimpl
    }

    // $80/C1FA E8          INX                     A:4F00 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FA E8          INX                    ", a, x, y, n, z, c);
    ++x;

    // $80/C1FB 90 F9       BCC $F9    [$C1F6]      A:4F00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FB 90 F9       BCC $F9    [$C1F6]     ", a, x, y, n, z, c);
    if (!c)
    {
        __debugbreak(); // notimpl
    }

    // $80/C1FD 86 04       STX $04    [$00:0004]   A:4F00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FD 86 04       STX $04    [$00:0004]  ", a, x, y, n, z, c);
    mem04 = x;

label_C1FF:

    // $80/C1FF 0A          ASL A                   A:4F00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FF 0A          ASL A                  ", a, x, y, n, z, c);
    c = a >= 0x8000;
    a *= 2;

    // $80/C200 26 6F       ROL $6F    [$00:006F]   A:9E00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C200 26 6F       ROL $6F    [$00:006F]  ", a, x, y, n, z, c);
    RotateLeft(&mem6f, &c);

    // $80/C202 88          DEY                     A:9E00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C202 88          DEY                    ", a, x, y, n, z, c);
    --y;
    z = y == 0;

    // $80/C203 F0 21       BEQ $21    [$C226]      A:9E00 X:0003 Y:0001 P:envmxdizc
    DebugPrint("$80/C203 F0 21       BEQ $21    [$C226]     ", a, x, y, n, z, c);
    if (z)
    {
        goto label_C226;
    }

label_C205:
    // $80/C205 CA          DEX                     A:9E00 X:0003 Y:0001 P:envmxdizc
    DebugPrint("$80/C205 CA          DEX                    ", a, x, y, n, z, c);
    --x;
    z = x == 0;

    // $80/C206 D0 F7       BNE $F7    [$C1FF]      A:9E00 X:0002 Y:0001 P:envmxdizc
    DebugPrint("$80/C206 D0 F7       BNE $F7    [$C1FF]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_C1FF;
    }

    // $80/C208 85 6C       STA $6C    [$00:006C]   A:7922 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C208 85 6C       STA $6C    [$00:006C]  ", a, x, y, n, z, c);
    mem6c = a;

    // $80/C20A 06 04       ASL $04    [$00:0004]   A:7922 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C20A 06 04       ASL $04    [$00:0004]  ", a, x, y, n, z, c);
    mem04 *= 2;

    // $80/C20C A6 04       LDX $04    [$00:0004]   A:7922 X:0000 Y:0007 P:envmxdizc
    DebugPrint("$80/C20C A6 04       LDX $04    [$00:0004]  ", a, x, y, n, z, c);
    x = mem04;
    
    // $80/C20E BF B6 C2 80 LDA $80C2B6,x[$80:C2BC] A:7922 X:0006 Y:0007 P:envmxdizc
    DebugPrintWithAddress("$80/C20E BF B6 C2 80 LDA $80C2B6,x[$80:", 0xC2B6 + x, a, x, y, n, z, c, true);
    // X is one of {6,8,A,C,E,10} here. The ROM values are
    static unsigned short s_ROMValueTable_80C2B6[] = {0, 0, 0, 0x4, 0xC, 0x1C, 0x3C, 0x7C, 0xFC};
    a = s_ROMValueTable_80C2B6[x / 2];
    
    // $80/C212 A6 00       LDX $00    [$00:0000]   A:0004 X:0006 Y:0007 P:envmxdizc
    DebugPrint("$80/C212 A6 00       LDX $00    [$00:0000]  ", a, x, y, n, z, c);
    x = mem00;
   
    // $80/C214 18          CLC                     A:0004 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C214 18          CLC                    ", a, x, y, n, z, c);
    c = false;
    
    // $80/C215 65 6F       ADC $6F    [$00:006F]   A:0004 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C215 65 6F       ADC $6F    [$00:006F]  ", a, x, y, n, z, c);
    a += mem6f;
    
    // $80/C217 85 6F       STA $6F    [$00:006F]   A:0006 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C217 85 6F       STA $6F    [$00:006F]  ", a, x, y, n, z, c);
    mem6f = a;
    
    // $80/C219 60          RTS                     A:0006 X:0008 Y:0007 P:envmxdizc
    DebugPrint("$80/C219 60          RTS                    ", a, x, y, n, z, c);
    DebugPrintNewline();
    return;

    __debugbreak(); // notimpl

label_C226:

    // $80/C226 E2 20       SEP #$20                A:3C00 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C226 E2 20       SEP #$20               ", a, x, y, n, z, c);

    // $80/C228 B2 0C       LDA ($0C)  [$99:F8B5]   A:3C00 X:0002 Y:0000 P:envmxdizc
    DebugPrintWithAddress("$80/C228 B2 0C       LDA ($0C)  [$99:", mem0c, a, x, y, n, z, c);
    loaded = LoadFromROMFragment(0x990000 | mem0c);
    a &= 0xFF00;
    a |= loaded & 0xFF;

    // $80/C22A C2 20       REP #$20                A:3C91 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C22A C2 20       REP #$20               ", a, x, y, n, z, c);

    // $80/C22C E6 0C       INC $0C    [$00:000C]   A:3C91 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C22C E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/C22E A0 08       LDY #$08                A:3C91 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C22E A0 08       LDY #$08               ", a, x, y, n, z, c);
    y = 0x8;

    // $80/C230 80 D3       BRA $D3    [$C205]      A:3C91 X:0002 Y:0008 P:envmxdizc
    DebugPrint("$80/C230 80 D3       BRA $D3    [$C205]     ", a, x, y, n, z, c);
    goto label_C205;
}

void Fn_80BBB3()
{
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

label_BBD9:
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
    DebugPrintWithAddress("$80/BBE4 9D 20 07    STA $0720,x[$99:", 0x720 + x, a, x, y, n, z, c);
    WriteStagingOutput(0x7E0720 + x, a); // Write A to 99/0720 -- this is 7E0720 (it's shadowed).

    // $80/BBE7 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0000 X:0000 Y:0008 P:envmxdizc
    DebugPrint("$80/BBE7 20 B0 C1    JSR $C1B0  [$80:C1B0]  ", a, x, y, n, z, c);
    DebugPrintNewline();
    Fn_C1B0();

    // $80/BBEA 9D 00 07    STA $0700,x[$99:0700]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithAddress("$80/BBEA 9D 00 07    STA $0700,x[$99:", 0x700 + x, a, x, y, n, z, c);
    WriteStagingOutput(0x7E0700 + x, a);

    // $80/BBED 18          CLC                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBED 18          CLC                    ", a, x, y, n, z, c);
    c = false;

    // $80/BBEE 65 77       ADC $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBEE 65 77       ADC $77    [$00:0077]  ", a, x, y, n, z, c);
    a += mem77;

    // $80/BBF0 85 77       STA $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF0 85 77       STA $77    [$00:0077]  ", a, x, y, n, z, c);
    mem77 = a;

    // $80/BBF2 A5 6F       LDA $6F    [$00:006F]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF2 A5 6F       LDA $6F    [$00:006F]  ", a, x, y, n, z, c);
    a = mem6f;

    // $80/BBF4 18          CLC                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF4 18          CLC                    ", a, x, y, n, z, c);
    c = false;

    // $80/BBF5 65 75       ADC $75    [$00:0075]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF5 65 75       ADC $75    [$00:0075]  ", a, x, y, n, z, c);
    a += mem75;

    // $80/BBF7 85 75       STA $75    [$00:0075]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF7 85 75       STA $75    [$00:0075]  ", a, x, y, n, z, c);
    mem75 = a;

    // $80/BBF9 A5 6F       LDA $6F    [$00:006F]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBF9 A5 6F       LDA $6F    [$00:006F]  ", a, x, y, n, z, c);
    a = mem6f;
    z = a == 0;

    // $80/BBFB D0 05       BNE $05    [$BC02]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BBFB D0 05       BNE $05    [$BC02]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_BC02;
    }

    // $80/BBFD 9E 40 07    STZ $0740,x[$99:0740]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithAddress("$80/BBFD 9E 40 07    STZ $0740,x[$99:", 0x740+x, a, x, y, n, z, c);
    WriteStagingOutput(0x7E0740 + x, a);

    // $80/BC00 80 D7       BRA $D7    [$BBD9]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC00 80 D7       BRA $D7    [$BBD9]     ", a, x, y, n, z, c);
    goto label_BBD9;

    __debugbreak(); // notimpl

label_BC02:
    // $80/BC02 A5 75       LDA $75    [$00:0075]   A:0001 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC02 A5 75       LDA $75    [$00:0075]  ", a, x, y, n, z, c);
    a = mem75;

    // $80/BC04 85 00       STA $00    [$00:0000]   A:0001 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC04 85 00       STA $00    [$00:0000]  ", a, x, y, n, z, c);
    mem00 = a;

    // $80/BC06 A5 14       LDA $14    [$00:0014]   A:0001 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC06 A5 14       LDA $14    [$00:0014]  ", a, x, y, n, z, c);
    a = mem14;
    z = a == 0;

    // $80/BC08 18          CLC                     A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC08 18          CLC                    ", a, x, y, n, z, c);
    c = false;

    // $80/BC09 F0 05       BEQ $05    [$BC10]      A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC09 F0 05       BEQ $05    [$BC10]     ", a, x, y, n, z, c);
    if (z)
    {
        __debugbreak(); // notimpl
        //goto label_BC10;
    }

label_BC0B:
    // $80/BC0B 06 00       ASL $00    [$00:0000]   A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC0B 06 00       ASL $00    [$00:0000]  ", a, x, y, n, z, c);
    c = a >= 0x8000;
    mem00 *= 2;

    // $80/BC0D 3A          DEC A                   A:000E X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC0D 3A          DEC A                  ", a, x, y, n, z, c);
    --a;
    z = a == 0;

    // $80/BC0E D0 FB       BNE $FB    [$BC0B]      A:000D X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC0E D0 FB       BNE $FB    [$BC0B]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_BC0B;
    }

    // $80/BC10 A5 00       LDA $00    [$00:0000]   A:0000 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC10 A5 00       LDA $00    [$00:0000]  ", a, x, y, n, z, c);
    a = mem00;

    // $80/BC12 9D 40 07    STA $0740,x[$99:0742]   A:4000 X:0002 Y:0002 P:envmxdizc
    DebugPrintWithAddress("$80/BC12 9D 40 07    STA $0740,x[$99:", 0x740 + x, a, x, y, n, z, c);
    WriteStagingOutput(0x7E0740 + x, a); 

    // $80/BC15 90 C2       BCC $C2    [$BBD9]      A:4000 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC15 90 C2       BCC $C2    [$BBD9]     ", a, x, y, n, z, c);
    if (!c)
    {
        goto label_BBD9;
    }

    __debugbreak(); // notimpl
}


int main()
{
    OpenDebugLog();
    LoadROMFragment();

    Fn_80BBB3();

    return 0;

}