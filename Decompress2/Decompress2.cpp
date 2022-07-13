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

// WRAM data
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
unsigned short mem0c = 0xF8AC;
unsigned short mem10 = 0;
unsigned short mem12 = 0x007F;
unsigned short mem14 = 0;
unsigned short mem6c = 0;
unsigned short mem6f = 0;
unsigned short mem71 = 0;
unsigned short mem73 = 0;
unsigned short mem75 = 0;
unsigned short mem77 = 0;
unsigned short mem79 = 0;
unsigned short mem7b = 0;
unsigned short mem7d = 0;
std::vector<unsigned char> ram7E0100_7E0200;
std::vector<unsigned char> mem7E0500_7E0700;
std::vector<unsigned char> ram7E0700_7E1000;

int indirectRAMAccess = 0;
std::vector<unsigned short> indirectStores7E0100;

std::vector<unsigned char> rom80BC7B;

void Fn_C1B0()
{
    bool willCarry = false;
    unsigned short loaded = 0;
    unsigned char low = 0;

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
        goto label_C1CB;
    }

label_C1B8:

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
        goto label_C1E3;
    }

    // $80/C1C6 85 6C       STA $6C    [$00:006C]   A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C6 85 6C       STA $6C    [$00:006C]  ", a, x, y, n, z, c);
    mem6c = a;

    // $80/C1C8 A5 6F       LDA $6F    [$00:006F]   A:B328 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1C8 A5 6F       LDA $6F    [$00:006F]  ", a, x, y, n, z, c);
    a = mem6f;
    
    // $80/C1CA 60          RTS                     A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/C1CA 60          RTS                    ", a, x, y, n, z, c);
    return;

label_C1CB:

    // $80/C1CB E2 20       SEP #$20                A:2200 X:0056 Y:0000 P:envmxdizc
    DebugPrint("$80/C1CB E2 20       SEP #$20               ", a, x, y, n, z, c); // 8bit acc

    // $80/C1CD B2 0C       LDA ($0C)  [$99:F8C7]   A:2200 X:0056 Y:0000 P:envmxdizc
    DebugPrintWithAddress("$80/C1CD B2 0C       LDA ($0C)  [$99:", mem0c, a, x, y, n, z, c);
    loaded = LoadFromROMFragment(0x990000 | mem0c);
    a &= 0xFF00;
    a |= loaded & 0xFF;

    // $80/C1CF C2 20       REP #$20                A:2212 X:0056 Y:0000 P:envmxdizc
    DebugPrint("$80/C1CF C2 20       REP #$20               ", a, x, y, n, z, c);

    // $80/C1D1 E6 0C       INC $0C    [$00:000C]   A:2212 X:0056 Y:0000 P:envmxdizc
    DebugPrint("$80/C1D1 E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/C1D3 A0 08       LDY #$08                A:2212 X:0056 Y:0000 P:envmxdizc
    DebugPrint("$80/C1D3 A0 08       LDY #$08               ", a, x, y, n, z, c);
    y = 0x8;

    // $80/C1D5 80 E1       BRA $E1    [$C1B8]      A:2212 X:0056 Y:0008 P:envmxdizc
    DebugPrint("$80/C1D5 80 E1       BRA $E1    [$C1B8]     ", a, x, y, n, z, c);
    goto label_C1B8;
    
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

    __debugbreak();

label_C1E3:

    // $80/C1E3 E2 20       SEP #$20                A:8100 X:0012 Y:0000 P:envmxdizc
    DebugPrint("$80/C1E3 E2 20       SEP #$20               ", a, x, y, n, z, c);

    // $80/C1E5 B2 0C       LDA ($0C)  [$99:F8B9]   A:8100 X:0012 Y:0000 P:envmxdizc
    DebugPrintWithAddress("$80/C1E5 B2 0C       LDA ($0C)  [$99:", mem0c, a, x, y, n, z, c);
    loaded = LoadFromROMFragment(0x990000 | mem0c);
    a &= 0xFF00;
    a |= loaded & 0xFF;

    // $80/C1E7 C2 20       REP #$20                A:811C X:0012 Y:0000 P:envmxdizc
    DebugPrint("$80/C1E7 C2 20       REP #$20               ", a, x, y, n, z, c);

    // $80/C1E9 E6 0C       INC $0C    [$00:000C]   A:811C X:0012 Y:0000 P:envmxdizc
    DebugPrint("$80/C1E9 E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/C1EB A0 08       LDY #$08                A:811C X:0012 Y:0000 P:envmxdizc
    DebugPrint("$80/C1EB A0 08       LDY #$08               ", a, x, y, n, z, c);
    y = 8;

    // $80/C1ED 85 6C       STA $6C    [$00:006C]   A:811C X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/C1ED 85 6C       STA $6C    [$00:006C]  ", a, x, y, n, z, c);
    mem6c = a;

    // $80/C1EF A5 6F       LDA $6F    [$00:006F]   A:811C X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/C1EF A5 6F       LDA $6F    [$00:006F]  ", a, x, y, n, z, c);
    a = mem6f;
   
    // $80/C1F1 60          RTS                     A:0002 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/C1F1 60          RTS                    ", a, x, y, n, z, c);
    return;

label_C1F2:

    // $80/C1F2 86 00       STX $00    [$00:0000]   A:A780 X:0008 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F2 86 00       STX $00    [$00:0000]  ", a, x, y, n, z, c);
    mem00.Data16 = x;

    // $80/C1F4 A2 02       LDX #$02                A:A780 X:0008 Y:0003 P:envmxdizc
    DebugPrint("$80/C1F4 A2 02       LDX #$02               ", a, x, y, n, z, c);
    x = 0x2;

label_C1F6:
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
        goto label_C21A;
    }

label_C1FA:
    // $80/C1FA E8          INX                     A:4F00 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FA E8          INX                    ", a, x, y, n, z, c);
    ++x;

    // $80/C1FB 90 F9       BCC $F9    [$C1F6]      A:4F00 X:0003 Y:0002 P:envmxdizc
    DebugPrint("$80/C1FB 90 F9       BCC $F9    [$C1F6]     ", a, x, y, n, z, c);
    if (!c)
    {
        goto label_C1F6;
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
    x = mem00.Data16;
   
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
    return;

    __debugbreak(); // notimpl

label_C21A:

    // $80/C21A E2 20       SEP #$20                A:9100 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C21A E2 20       SEP #$20               ", a, x, y, n, z, c);

    // $80/C21C B2 0C       LDA ($0C)  [$99:F8B6]   A:9100 X:0002 Y:0000 P:envmxdizc
    DebugPrintWithAddress("$80/C21C B2 0C       LDA ($0C)  [$99:", mem0c, a, x, y, n, z, c);
    loaded = LoadFromROMFragment(0x990000 | mem0c);
    a &= 0xFF00;
    a |= loaded & 0xFF;

    // $80/C21E C2 20       REP #$20                A:91D1 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C21E C2 20       REP #$20               ", a, x, y, n, z, c);

    // $80/C220 E6 0C       INC $0C    [$00:000C]   A:91D1 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C220 E6 0C       INC $0C    [$00:000C]  ", a, x, y, n, z, c);
    mem0c++;

    // $80/C222 A0 08       LDY #$08                A:91D1 X:0002 Y:0000 P:envmxdizc
    DebugPrint("$80/C222 A0 08       LDY #$08               ", a, x, y, n, z, c);
    y = 0x8;

    // $80/C224 80 D4       BRA $D4    [$C1FA]      A:91D1 X:0002 Y:0008 P:envmxdizc
    DebugPrint("$80/C224 80 D4       BRA $D4    [$C1FA]     ", a, x, y, n, z, c);
goto label_C1FA;
    __debugbreak();

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
    mem00.Data16 = a;

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
    c = mem00.Data16 >= 0x8000;
    mem00.Data16 *= 2;

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
    a = mem00.Data16;

    // $80/BC12 9D 40 07    STA $0740,x[$99:0742]   A:4000 X:0002 Y:0002 P:envmxdizc
    DebugPrintWithAddress("$80/BC12 9D 40 07    STA $0740,x[$99:", 0x740 + x, a, x, y, n, z, c);
    WriteStagingOutput(0x7E0740 + x, a); 

    // $80/BC15 90 C2       BCC $C2    [$BBD9]      A:4000 X:0002 Y:0002 P:envmxdizc
    DebugPrint("$80/BC15 90 C2       BCC $C2    [$BBD9]     ", a, x, y, n, z, c);
    if (!c)
    {
        goto label_BBD9;
    }

    // Can fall through-- cary can be set by ASL on mem00 earlier

    // $80/BC17 8A          TXA                     A:0000 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC17 8A          TXA                    ", a, x, y, n, z, c);
    a = x;

    // $80/BC18 4A          LSR A                   A:0012 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC18 4A          LSR A                  ", a, x, y, n, z, c);
    a >>= 1;

    // $80/BC19 85 79       STA $79    [$00:0079]   A:0009 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC19 85 79       STA $79    [$00:0079]  ", a, x, y, n, z, c);
    mem79 = a;

    // $80/BC1B A2 3E       LDX #$3E                A:0009 X:0012 Y:0008 P:envmxdizc
    DebugPrint("$80/BC1B A2 3E       LDX #$3E               ", a, x, y, n, z, c);
    x = 0x3E;

label_BC1D:
    // $80/BC1D 9E 00 05    STZ $0500,x[$99:053E]   A:0009 X:003E Y:0008 P:envmxdizc
    // X = 0x3E, 0x3C, 0x3A, ... 6, 4, 2, 0
    DebugPrintWithAddress("$80/BC1D 9E 00 05    STZ $0500,x[$99:", 0x990500 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[x] = 0;
    mem7E0500_7E0700[x+1] = 0;

    // $80/BC20 9E 40 05    STZ $0540,x[$99:057E]   A:0009 X:003E Y:0008 P:envmxdizc
    DebugPrintWithAddress("$80/BC20 9E 40 05    STZ $0540,x[$99:", 0x990540 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[0x40 + x] = 0;
    mem7E0500_7E0700[0x40 + x+1] = 0;

    // $80/BC23 9E 80 05    STZ $0580,x[$99:05BE]   A:0009 X:003E Y:0008 P:envmxdizc
    DebugPrintWithAddress("$80/BC23 9E 80 05    STZ $0580,x[$99:", 0x990580 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[0x80 + x] = 0;
    mem7E0500_7E0700[0x80 + x+1] = 0;

    // $80/BC26 9E C0 05    STZ $05C0,x[$99:05FE]   A:0009 X:003E Y:0008 P:envmxdizc
    DebugPrintWithAddress("$80/BC26 9E C0 05    STZ $05C0,x[$99:", 0x9905C0 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[0xC0 + x] = 0;
    mem7E0500_7E0700[0xC0 + x+1] = 0;
    
    // $80/BC29 CA          DEX                     A:0009 X:003E Y:0008 P:envmxdizc
    DebugPrint("$80/BC29 CA          DEX                    ", a, x, y, n, z, c);
    --x;
    x &= 0xFF;

    // $80/BC2A CA          DEX                     A:0009 X:003D Y:0008 P:envmxdizc
    DebugPrint("$80/BC2A CA          DEX                    ", a, x, y, n, z, c);
    --x;
    x &= 0xFF;
    n = x >= 0x80;

    // $80/BC2B 10 F0       BPL $F0    [$BC1D]      A:0009 X:003C Y:0008 P:envmxdizc
    DebugPrint("$80/BC2B 10 F0       BPL $F0    [$BC1D]     ", a, x, y, n, z, c);
    if (!n)
    {
        goto label_BC1D;
    }

    // Indirect RAM access -- hardcoded access 0x7E0100
    // $80/BC2D A2 7E       LDX #$7E                A:0009 X:00FE Y:0008 P:envmxdizc
    DebugPrint("$80/BC2D A2 7E       LDX #$7E               ", a, x, y, n, z, c);
    x = 0x7E;

    // $80/BC2F 8E 83 21    STX $2183  [$99:2183]   A:0009 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC2F 8E 83 21    STX $2183  [$99:2183]  ", a, x, y, n, z, c);

    // $80/BC32 A9 00 01    LDA #$0100              A:0009 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC32 A9 00 01    LDA #$0100             ", a, x, y, n, z, c);
    a = 0x100;

    // $80/BC35 8D 81 21    STA $2181  [$99:2181]   A:0100 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC35 8D 81 21    STA $2181  [$99:2181]  ", a, x, y, n, z, c);

    // $80/BC38 A2 FF       LDX #$FF                A:0100 X:007E Y:0008 P:envmxdizc
    DebugPrint("$80/BC38 A2 FF       LDX #$FF               ", a, x, y, n, z, c);
    x = 0xFF;

    indirectRAMAccess = 0x7E0100;

label_BC3A:

    // $80/BC3A 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0100 X:00FF Y:0008 P:envmxdizc
    DebugPrint("$80/BC3A 20 B0 C1    JSR $C1B0  [$80:C1B0]  ", a, x, y, n, z, c);
    Fn_C1B0();

    // $80/BC3D E2 20       SEP #$20                A:0000 X:00FF Y:0005 P:envmxdizc
    DebugPrint("$80/BC3D E2 20       SEP #$20               ", a, x, y, n, z, c);

    // $80/BC3F 1A          INC A                   A:0000 X:00FF Y:0005 P:envmxdizc
    DebugPrint("$80/BC3F 1A          INC A                  ", a, x, y, n, z, c);
    ++a;

label_BC40:

    // $80/BC40 E8          INX                     A:0001 X:00FF Y:0005 P:envmxdizc
    DebugPrint("$80/BC40 E8          INX                    ", a, x, y, n, z, c);
    ++x;
    x &= 0xFF;

    // $80/BC41 3C 00 05    BIT $0500,x[$99:0500]   A:0001 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithAddress("$80/BC41 3C 00 05    BIT $0500,x[$99:", 0x500 + x, a, x, y, n, z, c);
    n = mem7E0500_7E0700[x] >= 0x80;

    // $80/BC44 30 FA       BMI $FA    [$BC40]      A:0012 X:0022 Y:0006 P:envMXdiZc
    DebugPrint("$80/BC44 30 FA       BMI $FA    [$BC40]     ", a, x, y, n, z, c);
    if (n)
    {
        goto label_BC40;
    }

    // $80/BC46 3A          DEC A                   A:0001 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC46 3A          DEC A                  ", a, x, y, n, z, c);
    --a;
    z = a == 0;

    // $80/BC47 D0 F7       BNE $F7    [$BC40]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC47 D0 F7       BNE $F7    [$BC40]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_BC40;
    }

    // $80/BC49 DE 00 05    DEC $0500,x[$99:0500]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrintWithAddress("$80/BC49 DE 00 05    DEC $0500,x[$99:", 0x500 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[x]--;

    // $80/BC4C C2 20       REP #$20                A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC4C C2 20       REP #$20               ", a, x, y, n, z, c);

    // $80/BC4E 8E 80 21    STX $2180  [$99:2180]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC4E 8E 80 21    STX $2180  [$99:2180]  ", a, x, y, n, z, c);
    indirectRAMAccess += 2;
    indirectStores7E0100.push_back(x);

    // $80/BC51 C6 77       DEC $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC51 C6 77       DEC $77    [$00:0077]  ", a, x, y, n, z, c);
    mem77--;
    z = mem77 == 0;

    // $80/BC53 D0 E5       BNE $E5    [$BC3A]      A:0000 X:0000 Y:0005 P:envmxdizc
    DebugPrint("$80/BC53 D0 E5       BNE $E5    [$BC3A]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_BC3A;
    }

    // $80/BC55 98          TYA                     A:0000 X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC55 98          TYA                    ", a, x, y, n, z, c);
    a = y;

    // $80/BC56 0A          ASL A                   A:0006 X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC56 0A          ASL A                  ", a, x, y, n, z, c);
    a *= 2;

    // $80/BC57 85 71       STA $71    [$00:0071]   A:000C X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC57 85 71       STA $71    [$00:0071]  ", a, x, y, n, z, c);
    mem71 = a;

    // $80/BC59 E2 20       SEP #$20                A:000C X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC59 E2 20       SEP #$20               ", a, x, y, n, z, c);

    // $80/BC5B A2 00       LDX #$00                A:000C X:00AA Y:0006 P:envmxdizc
    DebugPrint("$80/BC5B A2 00       LDX #$00               ", a, x, y, n, z, c);
    x = 0;

    // $80/BC5D 9B          TXY                     A:000C X:0000 Y:0006 P:envmxdizc
    DebugPrint("$80/BC5D 9B          TXY                    ", a, x, y, n, z, c);
    y = x;

    // $80/BC5E 64 7B       STZ $7B    [$00:007B]   A:000C X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC5E 64 7B       STZ $7B    [$00:007B]  ", a, x, y, n, z, c);
    mem7b = 0;

label_BC60:

    // $80/BC60 86 00       STX $00    [$00:0000]   A:000C X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC60 86 00       STX $00    [$00:0000]  ", a, x, y, n, z, c);
    mem00.Data16 = x;

    // $80/BC62 A5 7B       LDA $7B    [$00:007B]   A:000C X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC62 A5 7B       LDA $7B    [$00:007B]  ", a, x, y, n, z, c);
    a = mem7b;

    // $80/BC64 0A          ASL A                   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC64 0A          ASL A                  ", a, x, y, n, z, c);
    a *= 2;

    // $80/BC65 AA          TAX                     A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC65 AA          TAX                    ", a, x, y, n, z, c);
    x = a;

    // $80/BC66 E0 10       CPX #$10                A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC66 E0 10       CPX #$10               ", a, x, y, n, z, c);
    z = x == 0x10;

    // $80/BC68 F0 55       BEQ $55    [$BCBF]      A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC68 F0 55       BEQ $55    [$BCBF]     ", a, x, y, n, z, c);
    if (z)
    {
        goto label_BCBF;
    }

    // $80/BC6A BD 00 07    LDA $0700,x[$99:0700]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrintWithAddress("$80/BC6A BD 00 07    LDA $0700,x[$99:", 0x700 + x, a, x, y, n, z, c);
    a = ram7E0700_7E1000[x]; // X = 0x0-0x50
    
    // $80/BC6D 85 77       STA $77    [$00:0077]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC6D 85 77       STA $77    [$00:0077]  ", a, x, y, n, z, c);
    mem77 = a;

    // $80/BC6F A6 7B       LDX $7B    [$00:007B]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC6F A6 7B       LDX $7B    [$00:007B]  ", a, x, y, n, z, c);
    x = mem7b;

    // $80/BC71 BF 7B BC 80 LDA $80BC7B,x[$80:BC7B] A:0000 X:0000 Y:0000 P:envmxdizc
    // x=0..25
    // Load from ROM. 8bit load
    DebugPrintWithAddress("$80/BC71 BF 7B BC 80 LDA $80BC7B,x[$80:", 0xBC7B + x, a, x, y, n, z, c, true);
    a = rom80BC7B[x];
    
    // $80/BC75 85 7D       STA $7D    [$00:007D]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC75 85 7D       STA $7D    [$00:007D]  ", a, x, y, n, z, c);
    mem7d = a;

    // $80/BC77 A6 00       LDX $00    [$00:0000]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC77 A6 00       LDX $00    [$00:0000]  ", a, x, y, n, z, c);
    x = mem00.Data16;

    // $80/BC79 80 36       BRA $36    [$BCB1]      A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC79 80 36       BRA $36    [$BCB1]     ", a, x, y, n, z, c);
    goto label_BCB1;

    __debugbreak();

label_BC83:

    // $80/BC83 A5 7B       LDA $7B    [$00:007B]   A:0041 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC83 A5 7B       LDA $7B    [$00:007B]  ", a, x, y, n, z, c);
    a = mem7b;

    // $80/BC85 0A          ASL A                   A:0001 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC85 0A          ASL A                  ", a, x, y, n, z, c);
    a *= 2;

    // $80/BC86 85 00       STA $00    [$00:0000]   A:0002 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC86 85 00       STA $00    [$00:0000]  ", a, x, y, n, z, c);
    mem00.Data16 = a;

    // $80/BC88 B9 00 01    LDA $0100,y[$99:0100]   A:0002 X:0000 Y:0000 P:envmxdizc
    // 8bit load of RAM
    DebugPrintWithAddress("$80/BC88 B9 00 01    LDA $0100,y[$99:", 0x100 + y, a, x, y, n, z, c);
    a = ram7E0100_7E0200[y];

    // $80/BC8B 85 01       STA $01    [$00:0001]   A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC8B 85 01       STA $01    [$00:0001]  ", a, x, y, n, z, c);
    mem00.High8 = a;

    // $80/BC8D C8          INY                     A:0000 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BC8D C8          INY                    ", a, x, y, n, z, c);
    ++y;

    // $80/BC8E C5 73       CMP $73    [$00:0073]   A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC8E C5 73       CMP $73    [$00:0073]  ", a, x, y, n, z, c);
    z = a == (mem73 & 0xFF); // we are in 8bit mode

    // $80/BC90 D0 09       BNE $09    [$BC9B]      A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC90 D0 09       BNE $09    [$BC9B]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_BC9B; 
    }

    // $80/BC92 A5 7B       LDA $7B    [$00:007B]   A:000F X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC92 A5 7B       LDA $7B    [$00:007B]  ", a, x, y, n, z, c);
    a = mem7b;

    // $80/BC94 1A          INC A                   A:0005 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC94 1A          INC A                  ", a, x, y, n, z, c);
    ++a;

    // $80/BC95 85 74       STA $74    [$00:0074]   A:0006 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC95 85 74       STA $74    [$00:0074]  ", a, x, y, n, z, c);
    mem73 &= 0x00FF; // Keep the first, lower byte
    mem73 |= (a << 8); // Replace the upper byte, second byte

    // $80/BC97 A9 12       LDA #$12                A:0006 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC97 A9 12       LDA #$12               ", a, x, y, n, z, c);
    a = 0x12;

    // $80/BC99 85 00       STA $00    [$00:0000]   A:0012 X:0094 Y:000B P:envmxdizc
    DebugPrint("$80/BC99 85 00       STA $00    [$00:0000]  ", a, x, y, n, z, c);
    mem00.Low8 = a;

label_BC9B:

    // $80/BC9B 84 04       STY $04    [$00:0004]   A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC9B 84 04       STY $04    [$00:0004]  ", a, x, y, n, z, c);
    mem04 = y;

    // $80/BC9D A4 7D       LDY $7D    [$00:007D]   A:0000 X:0000 Y:0001 P:envmxdizc
    DebugPrint("$80/BC9D A4 7D       LDY $7D    [$00:007D]  ", a, x, y, n, z, c);
    y = mem7d;

    // $80/BC9F 80 0B       BRA $0B    [$BCAC]      A:0000 X:0000 Y:0041 P:envmxdizc
    DebugPrint("$80/BC9F 80 0B       BRA $0B    [$BCAC]     ", a, x, y, n, z, c);
    goto label_BCAC;

label_BCA1:

    // $80/BCA1 A5 01       LDA $01    [$00:0001]   A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCA1 A5 01       LDA $01    [$00:0001]  ", a, x, y, n, z, c);
    // Select the upper byte of mem0
    // xxx we load 00 but we should load 0f
    a = mem00.High8;

    // $80/BCA3 9D 00 05    STA $0500,x[$99:0500]   A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrintWithAddress("$80/BCA3 9D 00 05    STA $0500,x[$99:", 0x500 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[x] = (a & 0xFF); // Is this right?

    // $80/BCA6 A5 00       LDA $00    [$00:0000]   A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCA6 A5 00       LDA $00    [$00:0000]  ", a, x, y, n, z, c);
    a = mem00.Low8;

    // $80/BCA8 9D 00 06    STA $0600,x[$99:0600]   A:0002 X:0000 Y:0040 P:envmxdizc
    DebugPrintWithAddress("$80/BCA8 9D 00 06    STA $0600,x[$99:", 0x600 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[0x100 + x] = (a & 0xFF);

    // $80/BCAB E8          INX                     A:0002 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCAB E8          INX                    ", a, x, y, n, z, c);
    ++x;

label_BCAC:
    // $80/BCAC 88          DEY                     A:0000 X:0000 Y:0041 P:envmxdizc
    DebugPrint("$80/BCAC 88          DEY                    ", a, x, y, n, z, c);
    --y;
    z = y == 0;

    // $80/BCAD D0 F2       BNE $F2    [$BCA1]      A:0000 X:0000 Y:0040 P:envmxdizc
    DebugPrint("$80/BCAD D0 F2       BNE $F2    [$BCA1]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_BCA1;
    }

    // $80/BCAF A4 04       LDY $04    [$00:0004]   A:0002 X:0040 Y:0000 P:envmxdizc
    DebugPrint("$80/BCAF A4 04       LDY $04    [$00:0004]  ", a, x, y, n, z, c);
    y = mem04;

label_BCB1:

    // $80/BCB1 C6 77       DEC $77    [$00:0077]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB1 C6 77       DEC $77    [$00:0077]  ", a, x, y, n, z, c);
    mem77--;
    n = mem77 >= 0x8000;

    // $80/BCB3 10 CE       BPL $CE    [$BC83]      A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB3 10 CE       BPL $CE    [$BC83]     ", a, x, y, n, z, c);
    if (!n)
    {
        goto label_BC83;
    }

    // $80/BCB5 E6 7B       INC $7B    [$00:007B]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB5 E6 7B       INC $7B    [$00:007B]  ", a, x, y, n, z, c);
    mem7b++;

    // $80/BCB7 A5 79       LDA $79    [$00:0079]   A:0081 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB7 A5 79       LDA $79    [$00:0079]  ", a, x, y, n, z, c);
    a = mem79;

    // $80/BCB9 C5 7B       CMP $7B    [$00:007B]   A:0009 X:0000 Y:0000 P:envmxdizc
    DebugPrint("$80/BCB9 C5 7B       CMP $7B    [$00:007B]  ", a, x, y, n, z, c);
    c = a >= mem7b;

    DebugPrint("$80/BCBB B0 A3       BCS $A3    [$BC60]     ", a, x, y, n, z, c);
    // $80/BCBB B0 A3       BCS $A3    [$BC60]      A:0009 X:0000 Y:0000 P:envmxdizc
    if (c)
    {
        goto label_BC60;
    }
    
    __debugbreak();

label_BCBF:

    // $80/BCBF A6 00       LDX $00    [$00:0000]   A:0010 X:0010 Y:003B P:envmxdizc
    DebugPrint("$80/BCBF A6 00       LDX $00    [$00:0000]  ", a, x, y, n, z, c);
    x = mem00.Low8;
    z = x == 0;

    // $80/BCC1 F0 08       BEQ $08    [$BCCB]      A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrint("$80/BCC1 F0 08       BEQ $08    [$BCCB]     ", a, x, y, n, z, c);
    if (z)
    {
        __debugbreak();
        //goto label_BCCB;
    }

    // $80/BCC3 A9 10       LDA #$10                A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrint("$80/BCC3 A9 10       LDA #$10               ", a, x, y, n, z, c);
    a = 0x10;

label_BCC5:

    // $80/BCC5 9D 00 06    STA $0600,x[$99:06F1]   A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrintWithAddress("$80/BCC5 9D 00 06    STA $0600,x[$99:", 0x600 + x, a, x, y, n, z, c);
    mem7E0500_7E0700[0x100 + x] = a;

    // $80/BCC8 E8          INX                     A:0010 X:00F1 Y:003B P:envmxdizc
    DebugPrint("$80/BCC8 E8          INX                    ", a, x, y, n, z, c);
    ++x;
    x &= 0x00FF;
    z = x == 0;

    // $80/BCC9 D0 FA       BNE $FA    [$BCC5]      A:0010 X:00F2 Y:003B P:envmxdizc
    DebugPrint("$80/BCC9 D0 FA       BNE $FA    [$BCC5]     ", a, x, y, n, z, c);
    if (!z)
    {
        goto label_BCC5;
    }

    // $80/BCCB A5 79       LDA $79    [$00:0079]   A:0010 X:0000 Y:003B P:envmxdizc
    DebugPrint("$80/BCCB A5 79       LDA $79    [$00:0079]  ", a, x, y, n, z, c);
    a = mem79;

    // $80/BCCD 0A          ASL A                   A:0009 X:0000 Y:003B P:envmxdizc
    DebugPrint("$80/BCCD 0A          ASL A                  ", a, x, y, n, z, c);
    a *= 2;

    // $80/BCCE AA          TAX                     A:0012 X:0000 Y:003B P:envmxdizc
    DebugPrint("$80/BCCE AA          TAX                    ", a, x, y, n, z, c);
    x = a;

    // $80/BCCF C2 20       REP #$20                A:0012 X:0012 Y:003B P:envmxdizc
    DebugPrint("$80/BCCF C2 20       REP #$20               ", a, x, y, n, z, c);

    // $80/BCD1 BF D9 BC 80 LDA $80BCD9,x[$80:BCEB] A:0012 X:0012 Y:003B P:envmxdizc
    DebugPrintWithAddress("$80/BCD1 BF D9 BC 80 LDA $80BCD9,x[$80:", 0xBCD9 + x, a, x, y, n, z, c, true);
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
    DebugPrint("$80/BCD5 8D 60 07    STA $0760  [$99:0760]  ", a, x, y, n, z, c);
    ram7E0700_7E1000[0x60] = a;

    // $80/BCD8 A4 12       LDY $12    [$00:0012]   A:BFC8 X:0012 Y:003B P:envmxdizc
    DebugPrint("$80/BCD8 A4 12       LDY $12    [$00:0012]  ", a, x, y, n, z, c);
    y = mem12;

    // $80/BCDA 8C 83 21    STY $2183  [$99:2183]   A:BFC8 X:0012 Y:007F P:envmxdizc    
    DebugPrint("$80/BCDA 8C 83 21    STY $2183  [$99:2183]  ", a, x, y, n, z, c); // Indirect wram high bit

    // $80/BCDD A5 10       LDA $10    [$00:0010]   A:BFC8 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCDD A5 10       LDA $10    [$00:0010]  ", a, x, y, n, z, c);
    a = mem10;

    // $80/BCDF 8D 81 21    STA $2181  [$99:2181]   A:0000 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCDF 8D 81 21    STA $2181  [$99:2181]  ", a, x, y, n, z, c);   // Indirect wram low and mid byte

    // $80/BCE2 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCE2 A5 6C       LDA $6C    [$00:006C]  ", a, x, y, n, z, c);
    a = mem6c;

    // $80/BCE4 A6 71       LDX $71    [$00:0071]   A:2508 X:0012 Y:007F P:envmxdizc
    DebugPrint("$80/BCE4 A6 71       LDX $71    [$00:0071]  ", a, x, y, n, z, c);
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
        goto label_BDBE;
    }
    else if (x == 0xE)
    {
        // goto 80:BD70
        __debugbreak();
    }

    __debugbreak();

label_BDBE:

    // $80/BDBE 85 6C       STA $6C    [$00:006C]   A:2508 X:000C Y:007F P:envmxdizc
    DebugPrint("$80/BDBE 85 6C       STA $6C    [$00:006C]  ", a, x, y, n, z, c);
    mem6c = a;

    // $80/BDC0 A4 6D       LDY $6D    [$00:006D]   A:2508 X:000C Y:007F P:envmxdizc
    DebugPrint("$80/BDC0 A4 6D       LDY $6D    [$00:006D]  ", a, x, y, n, z, c);
    y = mem6c >> 8;

    // $80/BDC2 BE 00 06    LDX $0600,y[$99:0625]   A:2508 X:000C Y:0025 P:envmxdizc
    // $80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BE53] A:2508 X:0002 Y:0025 P:envmxdizc

    __debugbreak();
    // notimpl
}


int main()
{
    OpenDebugLog();
    LoadROMFragment();
    rom80BC7B = LoadBinaryFile8("rom80BC7B.bin");

    mem7E0500_7E0700.resize(0x200);
    memset(mem7E0500_7E0700.data(), 0, mem7E0500_7E0700.size());

    // Have to initialize this from snapshot-- not 0.
    ram7E0100_7E0200 = LoadBinaryFile8("ram_stagex_7E0100.bin");
    ram7E0700_7E1000 = LoadBinaryFile8("ram_stagex_7E0700.bin");

    Fn_80BBB3();

    return 0;

}