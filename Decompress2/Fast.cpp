#include <vector>
#include <fstream>
#include <sstream>
#include "Util.h"
#include "Decompress2.h"

namespace Fast
{
    void Fn_80C1B0();
    void Fn_80C232();
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

    void LoadNextFrom0CInc(unsigned short pc)
    {
        // $80/BD13 E2 20       SEP #$20                A:9400 X:0002 Y:0025 P:envmxdizc

        pc += 2;

        // This runs in 8 bit mode.

        loaded16 = Load16FromAddress(dbr, mem0c);
        a &= 0xFF00;
        a |= loaded16.Low8;
        pc += 2;

        // $80/BD17 C2 20       REP #$20                A:948C X:0002 Y:0025 P:envmxdizc

        pc += 2;

        // $80/BD19 E6 0C       INC $0C    [$00:000C]   A:948C X:0002 Y:0025 P:envmxdizc

        mem0c++;
    }

    void LoadNextFrom0500(unsigned short pc)
    {
        // 16bit A, 8bit index

        x = mem7E0500_7E0700[y];
        pc += 3;

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
        pc += 3;

        // $80/BD21 86 08       STX $08    [$00:0008]   A:948C X:0000 Y:0025 P:envmxdizc

        mem08 = x;
    }

    void LoadNextFrom0600(unsigned short pc)
    {
        // $80/BE5D 85 6C       STA $6C    [$00:006C]   A:9420 X:0000 Y:0025 P:envmxdizc

        mem6c = a;
        pc += 2;

        // $80/BE5F A4 6D       LDY $6D    [$00:006D]   A:9420 X:0000 Y:0025 P:envmxdizc

        y = mem6c >> 8;
        pc += 2;

        x = mem7E0500_7E0700[0x100 + y];
    }

    void LoadNextFrom0CMaskAndShift(unsigned short pc, unsigned char nextX, int shifts)
    {

        x = nextX;
        pc += 2;

        // $80/BED3 64 6A       STZ $6A    [$00:006A]   A:F180 X:0006 Y:00F1 P:envmxdizc

        mem6a = 0;
        pc += 2;

        loaded16 = Load16FromAddress(dbr, mem0c);
        a = loaded16.Data16;
        pc += 2;

        // $80/BED7 29 FF 00    AND #$00FF              A:8C94 X:0006 Y:00F1 P:envmxdizc

        a &= 0xFF;
        pc += 3;

        for (int i = 0; i < shifts; ++i)
        {

            a *= 2;
            pc++;
        }

        loaded16 = LoadMem6b();
        a |= loaded16.Data16;
        pc += 2;

        loaded16.Data16 = a;
        SaveMem6b(loaded16);
        pc += 2;

        a = mem6c;
    }

    void ShiftThenLoad100ThenCompare(unsigned short pc, int shifts, int subtractDataAddress, int nextY)
    {
        for (int i = 0; i < shifts; ++i)
        {
            // $80/BF8F 4A          LSR A                   A:F192 X:0006 Y:00F1 P:envmxdizc

            a >>= 1;
            ++pc;
        }

        // $80/BF96 38          SEC                     A:01E3 X:0006 Y:00F1 P:envmxdizc

        ++pc;

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
        pc += 3;

        // $80/BF9A A8          TAY                     A:003C X:0006 Y:00F1 P:envmxdizc

        y = a;
        ++pc;

        // $80/BF9B E2 20       SEP #$20                A:003C X:0006 Y:003C P:envmxdizc

        pc += 2;

        a = cache7E0100[y];
        pc += 3;

        if (nextY == 1)
        {

        }
        else if (nextY == 2)
        {

        }
        else
        {
            __debugbreak();
        }
        y = nextY;
        pc += 2;

        // $80/BFA2 C5 73       CMP $73    [$00:0073]   A:0008 X:0006 Y:0001 P:envmxdizc

        z = a == (mem73 & 0xFF); // we are in 8bit mode
    }

    void ShiftRotateDecrement(unsigned short pc, int xDecAmt, int yDecAmt)
    {

        c = a >= 0x8000;
        a *= 2;
        pc++;

        RotateLeft(&mem6f, &c);
        pc += 2;

        for (int i = 0; i < xDecAmt; ++i)
        {

            --x;
            z = x == 0;
            ++pc;
        }

        for (int i = 0; i < yDecAmt; ++i)
        {

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
        //     When loading the GAME SETUP screen, this function is called 17 times, with the following values
        //     Call#            Mem0C-F
        //     -----            -----
        //     0                81ABDE
        //     1                9AC1F3
        //     2                938000
        //     3                9AB7A1
        //     4                9AE972
        //     5                9A862E <-- Montreal 0
        //     6                9988EC <-- Montreal 1
        //     7                99DFCF <-- Montreal 2
        //     8                98CAC0 <-- Montreal 3
        //     9                97D557 <-- Montreal 4
        //     10               99F8AC <-- Montreal 5
        //     11               97E40B <-- LA 0
        //     12               97C8FE <-- LA 1
        //     13               97A887 <-- LA 2
        //     14               97D7CC <-- LA 3
        //     15               98D62E <-- LA 4
        //     16               97DF28 <-- LA 5
        //
        // In local testing for now, we execute compared to a trace through of Montreal 0 (9A862E).

        // Use 8bit X and Y

        x &= 0xFF;
        y &= 0xFF;

        a = mem0c;

        c = false;

        a += 5;

        mem0c = a;

        loaded16 = Load16FromAddress(dbr, mem0c);
        a = loaded16.Data16;

        // $80/BBBF 85 73       STA $73    [$00:0073]   A:960F X:0080 Y:00AE P:envmxdizc

        mem73 = a;

        // $80/BBC1 E6 0C       INC $0C    [$00:000C]   A:960F X:0080 Y:00AE P:envmxdizc

        mem0c++;

        loaded16 = Load16FromAddress(dbr, mem0c);
        a = loaded16.Data16;

        // $80/BBC5 E6 0C       INC $0C    [$00:000C]   A:6596 X:0080 Y:00AE P:envmxdizc

        mem0c++;

        // $80/BBC7 E6 0C       INC $0C    [$00:000C]   A:6596 X:0080 Y:00AE P:envmxdizc

        mem0c++;

        // $80/BBC9 EB          XBA                     A:6596 X:0080 Y:00AE P:envmxdizc

        a = ExchangeShortHighAndLow(a);

        // $80/BBCA 85 6C       STA $6C    [$00:006C]   A:9665 X:0080 Y:00AE P:envmxdizc

        mem6c = a;

        // $80/BBCC A0 08       LDY #$08                A:9665 X:0080 Y:00AE P:envmxdizc

        y = 8;

        // $80/BBCE 64 77       STZ $77    [$00:0077]   A:9665 X:0080 Y:0008 P:envmxdizc

        mem77 = 0;

        // $80/BBD0 64 75       STZ $75    [$00:0075]   A:9665 X:0080 Y:0008 P:envmxdizc

        mem75 = 0;

        a = 0x10;

        mem14 = 0x10;

        // $80/BBD7 A2 FE       LDX #$FE                A:0010 X:0080 Y:0008 P:envmxdizc

        x = 0xFE;

    label_BBD9:
        // $80/BBD9 E8          INX                     A:0010 X:00FE Y:0008 P:envmxdizc

        x = IncLow8(x);

        // $80/BBDA E8          INX                     A:0010 X:00FF Y:0008 P:envmxdizc

        x = IncLow8(x);

        // $80/BBDB C6 14       DEC $14    [$00:0014]   A:0010 X:0000 Y:0008 P:envmxdizc

        mem14--;

        // $80/BBDD 06 75       ASL $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmxdizc

        mem75 *= 2;

        // $80/BBDF A5 75       LDA $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmxdizc

        a = mem75;

        // $80/BBE1 38          SEC                     A:0000 X:0000 Y:0008 P:envmxdizc

        c = true;

        // $80/BBE2 E5 77       SBC $77    [$00:0077]   A:0000 X:0000 Y:0008 P:envmxdizc

        a -= mem77;

        loaded16.Data16 = a;
        cache7E0720[x] = loaded16.Low8; // Write A to 99/0720 -- this is 7E0720 (it's shadowed).
        cache7E0720[x + 1] = loaded16.High8;

        // $80/BBE7 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0000 X:0000 Y:0008 P:envmxdizc

        Fn_80C1B0();

        // 8bit index

        loaded16.Data16 = a;
        cache7E0700[x] = loaded16.Low8;

        // $80/BBED 18          CLC                     A:0000 X:0000 Y:0005 P:envmxdizc

        c = false;

        // $80/BBEE 65 77       ADC $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc

        a += mem77;

        // $80/BBF0 85 77       STA $77    [$00:0077]   A:0000 X:0000 Y:0005 P:envmxdizc

        mem77 = a;

        // $80/BBF2 A5 6F       LDA $6F    [$00:006F]   A:0000 X:0000 Y:0005 P:envmxdizc

        a = mem6f;

        // $80/BBF4 18          CLC                     A:0000 X:0000 Y:0005 P:envmxdizc

        c = false;

        // $80/BBF5 65 75       ADC $75    [$00:0075]   A:0000 X:0000 Y:0005 P:envmxdizc

        a += mem75;

        // $80/BBF7 85 75       STA $75    [$00:0075]   A:0000 X:0000 Y:0005 P:envmxdizc

        mem75 = a;

        // $80/BBF9 A5 6F       LDA $6F    [$00:006F]   A:0000 X:0000 Y:0005 P:envmxdizc

        a = mem6f;
        z = a == 0;

        // $80/BBFB D0 05       BNE $05    [$BC02]      A:0000 X:0000 Y:0005 P:envmxdizc

        if (!z)
        {
            goto label_BC02;
        }

        loaded16.Data16 = a;
        cache7E0740[x] = loaded16.Low8;
        cache7E0740[x + 1] = loaded16.High8;

        // $80/BC00 80 D7       BRA $D7    [$BBD9]      A:0000 X:0000 Y:0005 P:envmxdizc

        goto label_BBD9;

        __debugbreak(); // notimpl

    label_BC02:
        // $80/BC02 A5 75       LDA $75    [$00:0075]   A:0001 X:0002 Y:0002 P:envmxdizc

        a = mem75;

        // $80/BC04 85 00       STA $00    [$00:0000]   A:0001 X:0002 Y:0002 P:envmxdizc

        mem00.Data16 = a;

        // $80/BC06 A5 14       LDA $14    [$00:0014]   A:0001 X:0002 Y:0002 P:envmxdizc

        a = mem14;
        z = a == 0;

        // $80/BC08 18          CLC                     A:000E X:0002 Y:0002 P:envmxdizc

        c = false;

        // $80/BC09 F0 05       BEQ $05    [$BC10]      A:000E X:0002 Y:0002 P:envmxdizc

        if (z)
        {
            __debugbreak(); // notimpl
            //goto label_BC10;
        }

    label_BC0B:
        // $80/BC0B 06 00       ASL $00    [$00:0000]   A:000E X:0002 Y:0002 P:envmxdizc

        c = mem00.Data16 >= 0x8000;
        mem00.Data16 *= 2;

        // $80/BC0D 3A          DEC A                   A:000E X:0002 Y:0002 P:envmxdizc

        --a;
        z = a == 0;

        // $80/BC0E D0 FB       BNE $FB    [$BC0B]      A:000D X:0002 Y:0002 P:envmxdizc

        if (!z)
        {
            goto label_BC0B;
        }

        // $80/BC10 A5 00       LDA $00    [$00:0000]   A:0000 X:0002 Y:0002 P:envmxdizc

        a = mem00.Data16;

        loaded16.Data16 = a;
        cache7E0740[x] = loaded16.Low8;
        cache7E0740[x + 1] = loaded16.High8;

        // $80/BC15 90 C2       BCC $C2    [$BBD9]      A:4000 X:0002 Y:0002 P:envmxdizc

        if (!c)
        {
            goto label_BBD9;
        }

        // Can fall through-- cary can be set by ASL on mem00 earlier

        // $80/BC17 8A          TXA                     A:0000 X:0012 Y:0008 P:envmxdizc

        a = x;

        // $80/BC18 4A          LSR A                   A:0012 X:0012 Y:0008 P:envmxdizc

        a >>= 1;

        // $80/BC19 85 79       STA $79    [$00:0079]   A:0009 X:0012 Y:0008 P:envmxdizc

        mem79 = a;

        // $80/BC1B A2 3E       LDX #$3E                A:0009 X:0012 Y:0008 P:envmxdizc

        x = 0x3E;

    label_BC1D:
        // X = 0x3E, 0x3C, 0x3A, ... 6, 4, 2, 0

        mem7E0500_7E0700[x] = 0;
        mem7E0500_7E0700[x + 1] = 0;

        mem7E0500_7E0700[0x40 + x] = 0;
        mem7E0500_7E0700[0x40 + x + 1] = 0;

        mem7E0500_7E0700[0x80 + x] = 0;
        mem7E0500_7E0700[0x80 + x + 1] = 0;

        mem7E0500_7E0700[0xC0 + x] = 0;
        mem7E0500_7E0700[0xC0 + x + 1] = 0;

        --x;
        x &= 0xFF;

        --x;
        x &= 0xFF;
        n = x >= 0x80;

        if (!n)
        {
            goto label_BC1D;
        }

        // Indirect RAM access -- hardcoded access 0x7E0100
        // $80/BC2D A2 7E       LDX #$7E                A:0009 X:00FE Y:0008 P:envmxdizc

        x = 0x7E;

        indirectHigh = x;

        // $80/BC32 A9 00 01    LDA #$0100              A:0009 X:007E Y:0008 P:envmxdizc

        a = 0x100;

        indirectLow = a;

        // $80/BC38 A2 FF       LDX #$FF                A:0100 X:007E Y:0008 P:envmxdizc

        x = 0xFF;

    label_BC3A:

        // $80/BC3A 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0100 X:00FF Y:0008 P:envmxdizc

        Fn_80C1B0();

        // $80/BC3D E2 20       SEP #$20                A:0000 X:00FF Y:0005 P:envmxdizc

        // $80/BC3F 1A          INC A                   A:0000 X:00FF Y:0005 P:envmxdizc

        ++a;

    label_BC40:

        // $80/BC40 E8          INX                     A:0001 X:00FF Y:0005 P:envmxdizc

        ++x;
        x &= 0xFF;

        n = mem7E0500_7E0700[x] >= 0x80;

        // $80/BC44 30 FA       BMI $FA    [$BC40]      A:0012 X:0022 Y:0006 P:envMXdiZc

        if (n)
        {
            goto label_BC40;
        }

        // $80/BC46 3A          DEC A                   A:0001 X:0000 Y:0005 P:envmxdizc

        --a;
        z = a == 0;

        // $80/BC47 D0 F7       BNE $F7    [$BC40]      A:0000 X:0000 Y:0005 P:envmxdizc

        if (!z)
        {
            goto label_BC40;
        }

        mem7E0500_7E0700[x]--;

        // $80/BC4C C2 20       REP #$20                A:0000 X:0000 Y:0005 P:envmxdizc

        // This is running in 8 bit index mode.

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

        mem77--;
        z = mem77 == 0;

        // $80/BC53 D0 E5       BNE $E5    [$BC3A]      A:0000 X:0000 Y:0005 P:envmxdizc

        if (!z)
        {
            goto label_BC3A;
        }

        // $80/BC55 98          TYA                     A:0000 X:00AA Y:0006 P:envmxdizc

        a = y;

        // $80/BC56 0A          ASL A                   A:0006 X:00AA Y:0006 P:envmxdizc

        a *= 2;

        // $80/BC57 85 71       STA $71    [$00:0071]   A:000C X:00AA Y:0006 P:envmxdizc

        mem71 = a;

        // $80/BC59 E2 20       SEP #$20                A:000C X:00AA Y:0006 P:envmxdizc

        // $80/BC5B A2 00       LDX #$00                A:000C X:00AA Y:0006 P:envmxdizc

        x = 0;

        // $80/BC5D 9B          TXY                     A:000C X:0000 Y:0006 P:envmxdizc

        y = x;

        // $80/BC5E 64 7B       STZ $7B    [$00:007B]   A:000C X:0000 Y:0000 P:envmxdizc

        mem7b = 0;

    label_BC60:

        // $80/BC60 86 00       STX $00    [$00:0000]   A:000C X:0000 Y:0000 P:envmxdizc

        mem00.Data16 = x;

        // $80/BC62 A5 7B       LDA $7B    [$00:007B]   A:000C X:0000 Y:0000 P:envmxdizc

        a = mem7b;

        // $80/BC64 0A          ASL A                   A:0000 X:0000 Y:0000 P:envmxdizc

        a *= 2;

        // $80/BC65 AA          TAX                     A:0000 X:0000 Y:0000 P:envmxdizc

        x = a;

        // $80/BC66 E0 10       CPX #$10                A:0000 X:0000 Y:0000 P:envmxdizc

        z = x == 0x10;

        // $80/BC68 F0 55       BEQ $55    [$BCBF]      A:0000 X:0000 Y:0000 P:envmxdizc

        if (z)
        {
            goto label_BCBF;
        }

        a = cache7E0700[x]; // X = 0x0-0x50

        // $80/BC6D 85 77       STA $77    [$00:0077]   A:0000 X:0000 Y:0000 P:envmxdizc

        mem77 = a;

        // $80/BC6F A6 7B       LDX $7B    [$00:007B]   A:0000 X:0000 Y:0000 P:envmxdizc

        x = mem7b;

        // $80/BC71 BF 7B BC 80 LDA $80BC7B,x[$80:BC7B] A:0000 X:0000 Y:0000 P:envmxdizc
        // x=0..25
        // Load from ROM. 8bit load

        a = romFile[0x3C7B + x];

        // $80/BC75 85 7D       STA $7D    [$00:007D]   A:0081 X:0000 Y:0000 P:envmxdizc

        mem7d = a;

        // $80/BC77 A6 00       LDX $00    [$00:0000]   A:0081 X:0000 Y:0000 P:envmxdizc

        x = mem00.Data16;

        // $80/BC79 80 36       BRA $36    [$BCB1]      A:0081 X:0000 Y:0000 P:envmxdizc

        goto label_BCB1;

        __debugbreak();

    label_BC83:

        // $80/BC83 A5 7B       LDA $7B    [$00:007B]   A:0041 X:0000 Y:0000 P:envmxdizc

        a = mem7b;

        // $80/BC85 0A          ASL A                   A:0001 X:0000 Y:0000 P:envmxdizc

        a *= 2;

        // $80/BC86 85 00       STA $00    [$00:0000]   A:0002 X:0000 Y:0000 P:envmxdizc

        mem00.Data16 = a;

        // This is running in 8 bit accumulator and index mode.

        loaded16.Data16 = a;
        loaded16.Low8 = cache7E0100[y];
        a = loaded16.Data16;

        // $80/BC8B 85 01       STA $01    [$00:0001]   A:0000 X:0000 Y:0000 P:envmxdizc

        loaded16.Data16 = a;
        mem00.High8 = loaded16.Low8;

        // $80/BC8D C8          INY                     A:0000 X:0000 Y:0000 P:envmxdizc

        ++y;

        // $80/BC8E C5 73       CMP $73    [$00:0073]   A:0000 X:0000 Y:0001 P:envmxdizc

        z = a == (mem73 & 0xFF); // we are in 8bit mode

        // $80/BC90 D0 09       BNE $09    [$BC9B]      A:0000 X:0000 Y:0001 P:envmxdizc

        if (!z)
        {
            goto label_BC9B;
        }

        // $80/BC92 A5 7B       LDA $7B    [$00:007B]   A:000F X:0094 Y:000B P:envmxdizc

        a = mem7b;

        // $80/BC94 1A          INC A                   A:0005 X:0094 Y:000B P:envmxdizc

        ++a;

        // $80/BC95 85 74       STA $74    [$00:0074]   A:0006 X:0094 Y:000B P:envmxdizc

        mem73 &= 0x00FF; // Keep the first, lower byte
        mem73 |= (a << 8); // Replace the upper byte, second byte

        // $80/BC97 A9 12       LDA #$12                A:0006 X:0094 Y:000B P:envmxdizc

        a = 0x12;

        // $80/BC99 85 00       STA $00    [$00:0000]   A:0012 X:0094 Y:000B P:envmxdizc

        loaded16.Data16 = a;
        mem00.Low8 = loaded16.Low8;

    label_BC9B:

        // $80/BC9B 84 04       STY $04    [$00:0004]   A:0000 X:0000 Y:0001 P:envmxdizc

        mem04 = y;

        // $80/BC9D A4 7D       LDY $7D    [$00:007D]   A:0000 X:0000 Y:0001 P:envmxdizc

        y = mem7d;

        // $80/BC9F 80 0B       BRA $0B    [$BCAC]      A:0000 X:0000 Y:0041 P:envmxdizc

        goto label_BCAC;

    label_BCA1:

        // $80/BCA1 A5 01       LDA $01    [$00:0001]   A:0000 X:0000 Y:0040 P:envmxdizc

        // Select the upper byte of mem0
        a = mem00.High8;

        mem7E0500_7E0700[x] = (a & 0xFF); // Is this right?

        // $80/BCA6 A5 00       LDA $00    [$00:0000]   A:0000 X:0000 Y:0040 P:envmxdizc

        a = mem00.Low8;

        mem7E0500_7E0700[0x100 + x] = (a & 0xFF);

        // $80/BCAB E8          INX                     A:0002 X:0000 Y:0040 P:envmxdizc

        ++x;

    label_BCAC:
        // $80/BCAC 88          DEY                     A:0000 X:0000 Y:0041 P:envmxdizc

        --y;
        z = y == 0;

        // $80/BCAD D0 F2       BNE $F2    [$BCA1]      A:0000 X:0000 Y:0040 P:envmxdizc

        if (!z)
        {
            goto label_BCA1;
        }

        // $80/BCAF A4 04       LDY $04    [$00:0004]   A:0002 X:0040 Y:0000 P:envmxdizc

        y = mem04;

    label_BCB1:

        // $80/BCB1 C6 77       DEC $77    [$00:0077]   A:0081 X:0000 Y:0000 P:envmxdizc

        mem77--;
        n = mem77 >= 0x8000;

        // $80/BCB3 10 CE       BPL $CE    [$BC83]      A:0081 X:0000 Y:0000 P:envmxdizc

        if (!n)
        {
            goto label_BC83;
        }

        // $80/BCB5 E6 7B       INC $7B    [$00:007B]   A:0081 X:0000 Y:0000 P:envmxdizc

        mem7b++;

        // $80/BCB7 A5 79       LDA $79    [$00:0079]   A:0081 X:0000 Y:0000 P:envmxdizc

        a = mem79;

        // $80/BCB9 C5 7B       CMP $7B    [$00:007B]   A:0009 X:0000 Y:0000 P:envmxdizc

        c = a >= mem7b;

        // $80/BCBB B0 A3       BCS $A3    [$BC60]      A:0009 X:0000 Y:0000 P:envmxdizc
        if (c)
        {
            goto label_BC60;
        }

        __debugbreak();

    label_BCBF:

        // $80/BCBF A6 00       LDX $00    [$00:0000]   A:0010 X:0010 Y:003B P:envmxdizc

        x = mem00.Low8;
        z = x == 0;

        // $80/BCC1 F0 08       BEQ $08    [$BCCB]      A:0010 X:00F1 Y:003B P:envmxdizc

        if (z)
        {
            __debugbreak();
            //goto label_BCCB;
        }

        // $80/BCC3 A9 10       LDA #$10                A:0010 X:00F1 Y:003B P:envmxdizc

        a = 0x10;

    label_BCC5:
        // 8bit acc

        loaded16.Data16 = a;
        mem7E0500_7E0700[0x100 + x] = loaded16.Low8;

        // $80/BCC8 E8          INX                     A:0010 X:00F1 Y:003B P:envmxdizc

        ++x;
        x &= 0x00FF;
        z = x == 0;

        // $80/BCC9 D0 FA       BNE $FA    [$BCC5]      A:0010 X:00F2 Y:003B P:envmxdizc

        if (!z)
        {
            goto label_BCC5;
        }

        // $80/BCCB A5 79       LDA $79    [$00:0079]   A:0010 X:0000 Y:003B P:envmxdizc

        a = mem79;

        // $80/BCCD 0A          ASL A                   A:0009 X:0000 Y:003B P:envmxdizc

        a *= 2;

        // $80/BCCE AA          TAX                     A:0012 X:0000 Y:003B P:envmxdizc

        x = a;

        // $80/BCCF C2 20       REP #$20                A:0012 X:0012 Y:003B P:envmxdizc

        // $80/BCD1 BF D9 BC 80 LDA $80BCD9,x[$80:BCEB] A:0012 X:0012 Y:003B P:envmxdizc

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

        mem0760 = a;

        // $80/BCD8 A4 12       LDY $12    [$00:0012]   A:BFC8 X:0012 Y:003B P:envmxdizc

        y = mem12;

        indirectHigh = mem12;

        // $80/BCDD A5 10       LDA $10    [$00:0010]   A:BFC8 X:0012 Y:007F P:envmxdizc

        a = mem10;

        indirectLow = mem10;

        // $80/BCE2 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0012 Y:007F P:envmxdizc

        a = mem6c;

        // $80/BCE4 A6 71       LDX $71    [$00:0071]   A:2508 X:0012 Y:007F P:envmxdizc

        x = mem71;

        // $80/BCE6 7C F9 BC    JMP ($BCF9,x)[$80:BDBE] A:2508 X:000C Y:007F P:envmxdizc
        // x = {2, 6, C, E}
        if (x == 2)
        {

            goto label_BF53;
        }
        else if (x == 6)
        {

            goto label_BEAE;
        }
        else if (x == 0xC)
        {

            goto label_BDBE;
        }
        else if (x == 0xE)
        {

            goto label_BD70;
        }
        else if (x == 0x10)
        {

            goto label_BD23;
        }
        else if (x == 0xA)
        {

            goto label_BE0D;
        }
        else if (x == 0x4)
        {

            goto label_BF00;
        }
        else if (x == 0x8)
        {

            goto label_BE5D;
        }
        else
        {
            __debugbreak();
        }

    label_BD0B:

        a *= 2;

    label_BD0C:

        a *= 2;

    label_BD0D:

        // $80/BD0D 0A          ASL A                   A:B8AC X:000A Y:00B8 P:envmxdizc

        a *= 2;

    label_BD0E:

        // $80/BD0E 0A          ASL A                   A:7158 X:000A Y:00B8 P:envmxdizc

        a *= 2;

    label_BD0F:
        // $80/BD0F 0A          ASL A                   A:E2B0 X:000A Y:00B8 P:envmxdizc

        a *= 2;

    label_BD10:

        a *= 2;

    label_BD11:

        // $80/BD11 0A          ASL A                   A:8AC0 X:000A Y:00B8 P:envmxdizc

        a *= 2;

        // $80/BD12 0A          ASL A                   A:1580 X:000A Y:00B8 P:envmxdizc

        a *= 2;

        LoadNextFrom0CInc(0xBD13);

        LoadNextFrom0500(0xBD1B);

    label_BD23:
        LoadNextFrom0600(0xBD23);

        // $80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BD41] A:948C X:0012 Y:0094 P:envmxdizc
        if (x == 0x12)
        {

            goto label_BD41;
        }
        else if (x == 0x2)
        {

            goto label_BDB4;
        }
        else if (x == 0x8)
        {

            goto label_BEA1;
        }
        else if (x == 0xA)
        {

            goto label_BEF2;
        }
        else if (x == 0xE)
        {

            goto label_BD0B;
        }
        else if (x == 0x10)
        {

            goto label_BD46;
        }
        else if (x == 0xC)
        {

            goto label_BF44;
        }
        else if (x == 6)
        {

            goto label_BE51;
        }
        else if (x == 4)
        {

            goto label_BE02;
        }
        else
        {
            __debugbreak();
        }

        __debugbreak();

    label_BD41:

        x = 0x10;

        goto label_C17C;

    label_BD46:

        LoadNextFrom0CMaskAndShift(0xBD46, 0x10, 0);

        goto label_JumpAbsolute760;

    label_BD58:

        a *= 2;

    label_BD59:

        a *= 2;

    label_BD5A:

        a *= 2;

    label_BD5B:

        a *= 2;

    label_BD5C:

        a *= 2;

    label_BD5D:

        a *= 2;

    label_BD5E:

        a *= 2;

        LoadNextFrom0CInc(0xBD5F);

        a *= 2;

        LoadNextFrom0500(0xBD68);

    label_BD70:
        LoadNextFrom0600(0xBD70);

        if (x == 0xE)
        {

            goto label_BD58;
        }
        else if (x == 2)
        {

            goto label_BE03;
        }
        else if (x == 0xA)
        {

            goto label_BF45;
        }
        else if (x == 8)
        {

            goto label_BEF3;
        }
        else if (x == 0x10)
        {

            goto label_BD93;
        }
        else if (x == 0xC)
        {

            goto label_BD0C;
        }
        else if (x == 0x12)
        {

            goto label_BD8E;
        }
        else if (x == 6)
        {

            goto label_BEA2;
        }
        else if (x == 4)
        {

            goto label_BE52;
        }
        else
        {
            __debugbreak();
        }

    label_BD8E:

        x = 0xE;

        goto label_C17C;

    label_BD93:
        LoadNextFrom0CMaskAndShift(0xBD93, 0xE, 1);

        goto label_JumpAbsolute760;

    label_BDA6:

        a *= 2;

    label_BDA7:

        a *= 2;

    label_BDA8:

        a *= 2;

    label_BDA9:
        // $80/BDA9 0A          ASL A                   A:85C0 X:0008 Y:0085 P:envmxdizc

        a *= 2;

    label_BDAA:
        // $80/BDAA 0A          ASL A                   A:0B80 X:0008 Y:0085 P:envmxdizc

        a *= 2;

    label_BDAB:
        // $80/BDAB 0A          ASL A                   A:1700 X:0008 Y:0085 P:envmxdizc

        a *= 2;

        LoadNextFrom0CInc(0xBDAC);

    label_BDB4:
        // $80/BDB4 0A          ASL A                   A:2E2B X:0008 Y:0085 P:envmxdizc

        a *= 2;

        // $80/BDB5 0A          ASL A                   A:5C56 X:0008 Y:0085 P:envmxdizc

        a *= 2;

        LoadNextFrom0500(0xBDB6);

    label_BDBE:

        LoadNextFrom0600(0xBDBE);

        // $80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BE53] A:2508 X:0002 Y:0025 P:envmxdizc
        if (x == 2)
        {

            goto label_BE53;
        }
        else if (x == 0xA)
        {

            goto label_BD0D;
        }
        else if (x == 8)
        {

            goto label_BF46;
        }
        else if (x == 0x12)
        {

            goto label_BDDC;
        }
        else if (x == 0xC)
        {

            goto label_BD59;
        }
        else if (x == 0xE)
        {

            goto label_BDA6;
        }
        else if (x == 6)
        {

            goto label_BEF4;
        }
        else if (x == 0x10)
        {

            goto label_BDE1;
        }
        else if (x == 4)
        {

            goto label_BEA3;
        }
        else
        {
            __debugbreak(); // notimpl
        }

        __debugbreak();

    label_BDDC:

        x = 0xC;

        goto label_C17C;

    label_BDE1:

        LoadNextFrom0CMaskAndShift(0xBDE1, 0xC, 2);

        goto label_JumpAbsolute760;

    label_BDF5:

        a *= 2;

    label_BDF6:

        a *= 2;

    label_BDF7:

        a *= 2;

    label_BDF8:

        a *= 2;

    label_BDF9:

        a *= 2;

        LoadNextFrom0CInc(0xBDFA);

    label_BE02:

        a *= 2;

    label_BE03:

        a *= 2;

        a *= 2;

        // Includes $80/BE08.
        LoadNextFrom0500(0xBE05);

    label_BE0D:
        LoadNextFrom0600(0xBE0D);

        if (x == 0x10)
        {

            goto label_BE30;
        }
        else if (x == 0xE)
        {

            goto label_BDF5;
        }
        else if (x == 0xC)
        {

            goto label_BDA7;
        }
        else if (x == 8)
        {

            goto label_BD0E;
        }
        else if (x == 0xA)
        {

            goto label_BD5A;
        }
        else if (x == 2)
        {

            goto label_BEA4;
        }
        else if (x == 6)
        {

            goto label_BF47;
        }
        else if (x == 0x12)
        {

            goto label_BE2B;
        }
        else if (x == 4)
        {

            goto label_BEF5;
        }
        else
        {
            __debugbreak();
        }

    label_BE2B:

        x = 0xA;

        goto label_C17C;

    label_BE30:
        // Includes B340
        LoadNextFrom0CMaskAndShift(0xBE30, 0xA, 3);

        goto label_JumpAbsolute760;

    label_BE45:

        a *= 2;

    label_BE46:

        a *= 2;

    label_BE47:

        a *= 2;

    label_BE48:

        a *= 2;

        LoadNextFrom0CInc(0xBE49);

    label_BE51:

        a *= 2;

    label_BE52:

        a *= 2;

    label_BE53:

        // $80/BE53 0A          ASL A                   A:2508 X:0002 Y:0025 P:envmxdizc

        a *= 2;

        // $80/BE54 0A          ASL A                   A:4A10 X:0002 Y:0025 P:envmxdizc

        a *= 2;

        LoadNextFrom0500(0xBE55);

    label_BE5D:
        LoadNextFrom0600(0xBE5D);

        // $80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BE7B] A:9420 X:0012 Y:0094 P:envmxdizc
        if (x == 0x12)
        {

            goto label_BE7B;
        }
        else if (x == 0xE)
        {

            goto label_BE45;
        }
        else if (x == 0xA)
        {

            goto label_BDA8;
        }
        else if (x == 0xC)
        {

            goto label_BDF6;
        }
        else if (x == 8)
        {

            goto label_BD5B;
        }
        else if (x == 0x10)
        {

            goto label_BE80;
        }
        else if (x == 6)
        {

            goto label_BD0F;
        }
        else if (x == 2)
        {

            goto label_BEF6;
        }
        else if (x == 4)
        {

            goto label_BF48;
        }
        else
        {
            __debugbreak(); // notimpl
        }

        __debugbreak();

    label_BE7B:
        // $80/BE7B A2 08       LDX #$08                A:9420 X:0012 Y:0094 P:envmxdizc

        x = 0x8;

        // $80/BE7D 4C 7C C1    JMP $C17C  [$80:C17C]   A:9420 X:0008 Y:0094 P:envmxdizc

        goto label_C17C;

    label_BE80:

        LoadNextFrom0CMaskAndShift(0xBE80, 8, 4);

        goto label_JumpAbsolute760;

    label_BE96:

        a *= 2;

    label_BE97:

        a *= 2;

    label_BE98:

        a *= 2;

        LoadNextFrom0CInc(0xBE99);

    label_BEA1:

        a *= 2;

    label_BEA2:

        a *= 2;

    label_BEA3:

        a *= 2;

    label_BEA4:

        a *= 2;

        a *= 2;

        LoadNextFrom0500(0xBEA6);

    label_BEAE:

        LoadNextFrom0600(0xBEAE);

        // $80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BED1] A:F180 X:0010 Y:00F1 P:envmxdizc
        if (x == 0x10)
        {

            goto label_BED1;
        }
        else if (x == 0x8)
        {

            goto label_BDA9;
        }
        else if (x == 0xA)
        {

            goto label_BDF7;
        }
        else if (x == 0xE)
        {

            goto label_BE96;
        }
        else if (x == 2)
        {

            goto label_BF49;
        }
        else if (x == 0xC)
        {

            goto label_BE46;
        }
        else if (x == 6)
        {

            goto label_BD5C;
        }
        else if (x == 0x12)
        {

            goto label_BECC;
        }
        else if (x == 4)
        {

            goto label_BD10;
        }
        else
        {
            __debugbreak(); // notimpl
        }

    label_BECC:

        x = 0x6;

        goto label_C17C;

        __debugbreak();

    label_BED1:

        LoadNextFrom0CMaskAndShift(0xBED1, 6, 5);

        goto label_JumpAbsolute760;

    label_BEE8:

        a *= 2;

    label_BEE9:

        a *= 2;

        LoadNextFrom0CInc(0xBEEA);

    label_BEF2:

        a *= 2;

    label_BEF3:

        a *= 2;

    label_BEF4:

        a *= 2;

    label_BEF5:

        a *= 2;

    label_BEF6:

        a *= 2;

        a *= 2;

        LoadNextFrom0500(0xBEF8);

    label_BF00:

        LoadNextFrom0600(0xBF00);

        // $80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BD11] A:2500 X:0002 Y:0025 P:envmxdizc
        if (x == 2)
        {

            goto label_BD11;
        }
        else if (x == 0xA)
        {

            goto label_BE47;
        }
        else if (x == 0xE)
        {

            goto label_BEE8;
        }
        else if (x == 0xC)
        {

            goto label_BE97;
        }
        else if (x == 8)
        {

            goto label_BDF8;
        }
        else if (x == 6)
        {

            goto label_BDAA;
        }
        else if (x == 0x10)
        {

            goto label_BF23;
        }
        else if (x == 0x12)
        {

            goto label_BF1E;
        }
        else if (x == 4)
        {

            goto label_BD5D;
        }
        else
        {
            __debugbreak();
        }

    label_BF1E:

        x = 4;

        goto label_C17C;

        __debugbreak();

    label_BF23:

        LoadNextFrom0CMaskAndShift(0xBF23, 4, 6);

        goto label_JumpAbsolute760;

    label_BF3B:

        a *= 2;

        LoadNextFrom0CInc(0xBF3C);

    label_BF44:

        a *= 2;

    label_BF45:

        a *= 2;

    label_BF46:

        a *= 2;

    label_BF47:

        a *= 2;

    label_BF48:

        a *= 2;

    label_BF49:

        a *= 2;

        a *= 2;

        LoadNextFrom0500(0xBF4B);

    label_BF53:
        LoadNextFrom0600(0xBF53);

        if (x == 8)
        {

            goto label_BE48;
        }
        else if (x == 2)
        {

            goto label_BD5E;
        }
        else if (x == 0xC)
        {

            goto label_BEE9;
        }
        else if (x == 0xE)
        {

            goto label_BF3B;
        }
        else if (x == 0xA)
        {

            goto label_BE98;
        }
        else if (x == 6)
        {

            goto label_BDF9;
        }
        else if (x == 0x10)
        {

            goto label_BF76;
        }
        else if (x == 0x12)
        {

            goto label_BF71;
        }
        else if (x == 4)
        {

            goto label_BDAB;
        }
        else
        {
            __debugbreak();
        }

    label_BF71:

        x = 2;

        goto label_C17C;

    label_BF76:

        LoadNextFrom0CMaskAndShift(0xBF76, 2, 7);

        goto label_JumpAbsolute760;

    label_BF8F:

        ShiftThenLoad100ThenCompare(0xBF8F, 7, 0x0730, 0x1);

        // $80/BFA4 F0 1C       BEQ $1C    [$BFC2]      A:0008 X:0006 Y:0001 P:envmxdizc

        if (z)
        {
            __debugbreak(); // notimpl
        }

        // $80/BFA6 4C E8 C0    JMP $C0E8  [$80:C0E8]   A:0008 X:0006 Y:0001 P:envmxdizc

        goto label_C0E8;

    label_BFA9:

        // Includes BFB0
        ShiftThenLoad100ThenCompare(0xBFA9, 6, 0x732, 2);

        // $80/BFBD F0 03       BEQ $03    [$BFC2]      A:00AA X:0006 Y:0002 P:envmxdizc

        if (z)
        {
            __debugbreak(); // notimpl
        }

        // $80/BFBF 4C E8 C0    JMP $C0E8  [$80:C0E8]   A:00AA X:0006 Y:0002 P:envmxdizc

        goto label_C0E8;

    label_BFC5:

        goto label_BF8F;

    label_BFC8:

        loaded16.Low8 = cache7E0740[0x10];
        loaded16.High8 = cache7E0740[0x11];
        c = a >= loaded16.Data16;

        // $80/BFCB 90 C2       BCC $C2    [$BF8F]      A:F192 X:0006 Y:00F1 P:envmxdizc

        if (!c)
        {
            goto label_BF8F;
        }

        goto label_BFA9;

    label_C0E8:

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
        else
        {
            __debugbreak();
        }
        indirectLow += 1;

        // $80/C0EB 85 08       STA $08    [$00:0008]   A:0008 X:0006 Y:0001 P:envmxdizc

        loaded16.Data16 = mem08;
        loaded16.Low8 = a & 0xFF;
        mem08 = loaded16.Data16;

        // $80/C0ED C2 20       REP #$20                A:0008 X:0006 Y:0001 P:envmxdizc

        //
        // $80/C0EF E6 0C       INC $0C    [$00:000C]   A:0008 X:0006 Y:0001 P:envmxdizc

        mem0c++;
        //
        // $80/C0F1 A5 6B       LDA $6B    [$00:006B]   A:0008 X:0006 Y:0001 P:envmxdizc

        loaded16 = LoadMem6b();
        a = loaded16.Data16;

        // $80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C11A] A:9280 X:0006 Y:0001 P:envmxdizc
        if (x == 6)
        {

            goto label_C11A;
        }
        else if (x == 0xA)
        {

            goto label_C112;
        }
        else if (x == 8)
        {

            goto label_C116;
        }
        else if (x == 0xE)
        {

            goto label_C10A;
        }
        else if (x == 0x10)
        {

            goto label_C106;
        }
        else if (x == 4)
        {

            goto label_C11E;
        }
        else if (x == 0xC)
        {

            goto label_C10E;
        }
        else if (x == 2)
        {

            goto label_C122;
        }
        else
        {
            __debugbreak(); // notimpl
        }

        __debugbreak();

    label_C106:

        a *= 2;

        y--;
        z = y == 0;

        if (z)
        {
            goto label_C159;
        }

    label_C10A:

        a *= 2;

        y--;
        z = y == 0;

        if (z)
        {
            goto label_C156;
        }

    label_C10E:

        a *= 2;

        y--;
        z = y == 0;

        if (z)
        {
            goto label_C153;
        }

    label_C112:

        // $80/C112 0A          ASL A                   A:9280 X:0006 Y:0001 P:envmxdizc

        a *= 2;

        // $80/C113 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc

        y--;
        z = y == 0;

        if (z)
        {
            goto label_C150;
        }

    label_C116:

        a *= 2;

        // $80/C117 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc

        y--;
        z = y == 0;

        if (z)
        {
            goto label_C14D;
        }

    label_C11A:

        // $80/C11A 0A          ASL A                   A:9280 X:0006 Y:0001 P:envmxdizc

        a *= 2;

        // $80/C11B 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc

        y--;
        z = y == 0;

        // $80/C11C F0 2C       BEQ $2C    [$C14A]      A:2500 X:0006 Y:0000 P:envmxdizc

        if (z)
        {
            goto label_C14A;
        }

    label_C11E:
        // $80/C11E 0A          ASL A                   A:9280 X:0006 Y:0001 P:envmxdizc

        a *= 2;

        // $80/C11F 88          DEY                     A:2500 X:0006 Y:0001 P:envmxdizc

        y--;
        z = y == 0;

        // $80/C120 F0 2C       BEQ $2C    [$C14A]      A:2500 X:0006 Y:0000 P:envmxdizc

        if (z)
        {
            goto label_C147;
        }

    label_C122:

        a *= 2;

        LoadNextFrom0CInc(0xC123);

        y--;
        z = y == 0;

        if (z)
        {
            goto label_C15C;
        }

        a *= 2;

        y--;
        z = y == 0;

        if (z)
        {
            goto label_C159;
        }

        __debugbreak();

    label_C147:

        goto label_BF53;

    label_C14A:
        // $80/C14A 4C 00 BF    JMP $BF00  [$80:BF00]   A:2500 X:0006 Y:0000 P:envmxdizc

        goto label_BF00;

    label_C14D:

        goto label_BEAE;

    label_C150:

        goto label_BE5D;

    label_C153:

        goto label_BE0D;

    label_C156:

        goto label_BDBE;

    label_C159:

        goto label_BD70;

    label_C15C:

        goto label_BD23;

    label_C17C:

        y = mem73 >> 8;

        Fn_80C2DC();

        mem6c = a;

        Fn_80C232();

        if (z)
        {
            goto label_C195;
        }

        y = mem08;

    label_C18A:

        // This is 8 bit index.
        loaded16.Data16 = y;
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
        indirectLow += 1;

        // $80/C18D 3A          DEC A                   A:003E X:0006 Y:0000 P:envmxdizc

        a--;
        z = a == 0;

        // $80/C18E D0 FA       BNE $FA    [$C18A]      A:003D X:0006 Y:0000 P:envmxdizc

        if (!z)
        {
            goto label_C18A;
        }

        // $80/C190 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0006 Y:0000 P:envmxdizc

        a = mem6c;

        // $80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BEAE] A:F180 X:0006 Y:0000 P:envmxdizc
        if (x == 6)
        {

            goto label_BEAE;
        }
        else if (x == 2)
        {

            goto label_BF53;
        }
        else if (x == 0xA)
        {

            goto label_BE0D;
        }
        else if (x == 0xC)
        {

            goto label_BDBE;
        }
        else if (x == 8)
        {

            goto label_BE5D;
        }
        else if (x == 0xE)
        {

            goto label_BD70;
        }
        else if (x == 0x10)
        {

            goto label_BD23;
        }
        else if (x == 4)
        {

            goto label_BF00;
        }
        else
        {
            __debugbreak(); // notimpl
        }

    label_C195:

        a = mem6c;
        n = mem6c >= 0x8000;

        if (n)
        {
            goto label_C1AF;
        }

        __debugbreak(); // notimpl

    label_C1AF:

        // Return from monstrosity

        return;

    label_JumpAbsolute760:
        switch (mem0760)
        {
        case 0xBFC5: goto label_BFC5;
        case 0xBFC8: goto label_BFC8;
        default: __debugbreak();
        }
    }

    void Fn_80C1B0()
    {
        // $80/C1B0 64 6F       STZ $6F    [$00:006F]   A:0000 X:0000 Y:0008 P:envmxdizc

        mem6f = 0;

        // $80/C1B2 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0000 Y:0008 P:envmxdizc

        a = mem6c;

        // $80/C1B4 0A          ASL A                   A:9665 X:0000 Y:0008 P:envmxdizc

        c = a >= 0x8000;
        a *= 2;

        // $80/C1B5 88          DEY                     A:2CCA X:0000 Y:0008 P:envmxdizc

        --y;
        z = y == 0;

        // $80/C1B6 F0 13       BEQ $13    [$C1CB]      A:2CCA X:0000 Y:0007 P:envmxdizc

        if (z)
        {
            goto label_C1CB;
        }

    label_C1B8:

        // $80/C1B8 90 38       BCC $38    [$C1F2]      A:2CCA X:0000 Y:0007 P:envmxdizc

        if (!c)
        {
            goto label_C1F2;
        }

        ShiftRotateDecrement(0xC1BA, 0, 1);

        // $80/C1BE F0 17       BEQ $17    [$C1D7]      A:5994 X:0000 Y:0006 P:envmxdizc

        if (z)
        {
            goto label_C1D7;
        }

    label_C1C0:

        ShiftRotateDecrement(0xC1C0, 0, 1);

        // $80/C1C4 F0 1D       BEQ $1D    [$C1E3]      A:B328 X:0000 Y:0005 P:envmxdizc

        if (z)
        {
            goto label_C1E3;
        }

        // $80/C1C6 85 6C       STA $6C    [$00:006C]   A:B328 X:0000 Y:0005 P:envmxdizc

        mem6c = a;

        // $80/C1C8 A5 6F       LDA $6F    [$00:006F]   A:B328 X:0000 Y:0005 P:envmxdizc

        a = mem6f;

        // $80/C1CA 60          RTS                     A:0000 X:0000 Y:0005 P:envmxdizc

        return;

    label_C1CB:

        LoadNextFrom0CInc(0xC1CB);

        // $80/C1D3 A0 08       LDY #$08                A:2212 X:0056 Y:0000 P:envmxdizc

        y = 0x8;

        // $80/C1D5 80 E1       BRA $E1    [$C1B8]      A:2212 X:0056 Y:0008 P:envmxdizc

        goto label_C1B8;

    label_C1D7:

        LoadNextFrom0CInc(0xC1D7);

        // $80/C1DF A0 08       LDY #$08                A:653C X:0004 Y:0000 P:envmxdizc

        y = 0x8;

        // $80/C1E1 80 DD       BRA $DD    [$C1C0]      A:653C X:0004 Y:0008 P:envmxdizc

        goto label_C1C0;

        __debugbreak();

    label_C1E3:

        LoadNextFrom0CInc(0xC1E3);

        // $80/C1EB A0 08       LDY #$08                A:811C X:0012 Y:0000 P:envmxdizc

        y = 8;

        // $80/C1ED 85 6C       STA $6C    [$00:006C]   A:811C X:0012 Y:0008 P:envmxdizc

        mem6c = a;

        // $80/C1EF A5 6F       LDA $6F    [$00:006F]   A:811C X:0012 Y:0008 P:envmxdizc

        a = mem6f;

        // $80/C1F1 60          RTS                     A:0002 X:0012 Y:0008 P:envmxdizc

        return;

    label_C1F2:

        // $80/C1F2 86 00       STX $00    [$00:0000]   A:A780 X:0008 Y:0003 P:envmxdizc

        mem00.Data16 = x;

        // $80/C1F4 A2 02       LDX #$02                A:A780 X:0008 Y:0003 P:envmxdizc

        x = 0x2;

    label_C1F6:
        // $80/C1F6 0A          ASL A                   A:A780 X:0002 Y:0003 P:envmxdizc

        c = a >= 0x8000;
        a *= 2;

        // $80/C1F7 88          DEY                     A:4F00 X:0002 Y:0003 P:envmxdizc

        --y;
        z = y == 0;

        // $80/C1F8 F0 20       BEQ $20    [$C21A]      A:4F00 X:0002 Y:0002 P:envmxdizc

        if (z)
        {
            LoadNextFrom0CInc(0xC21A);

            // $80/C222 A0 08       LDY #$08                A:91D1 X:0002 Y:0000 P:envmxdizc

            y = 0x8;

            // $80/C224 80 D4       BRA $D4    [$C1FA]      A:91D1 X:0002 Y:0008 P:envmxdizc

            goto label_C1FA;
        }

    label_C1FA:
        // $80/C1FA E8          INX                     A:4F00 X:0002 Y:0002 P:envmxdizc

        ++x;

        // $80/C1FB 90 F9       BCC $F9    [$C1F6]      A:4F00 X:0003 Y:0002 P:envmxdizc

        if (!c)
        {
            goto label_C1F6;
        }

        // $80/C1FD 86 04       STX $04    [$00:0004]   A:4F00 X:0003 Y:0002 P:envmxdizc

        mem04 = x;

    label_C1FF_Finish:

        ShiftRotateDecrement(0xC1FF, 0, 1);

        if (z)
        {
            LoadNextFrom0CInc(0xC226);

            y = 0x8;

        }

        // $80/C205 CA          DEX                     A:9E00 X:0003 Y:0001 P:envmxdizc

        --x;
        z = x == 0;

        // $80/C206 D0 F7       BNE $F7    [$C1FF]      A:9E00 X:0002 Y:0001 P:envmxdizc

        if (!z)
        {
            goto label_C1FF_Finish;
        }

        // $80/C208 85 6C       STA $6C    [$00:006C]   A:7922 X:0000 Y:0007 P:envmxdizc

        mem6c = a;

        // $80/C20A 06 04       ASL $04    [$00:0004]   A:7922 X:0000 Y:0007 P:envmxdizc

        mem04 *= 2;

        // $80/C20C A6 04       LDX $04    [$00:0004]   A:7922 X:0000 Y:0007 P:envmxdizc

        x = mem04;

        // $80/C20E BF B6 C2 80 LDA $80C2B6,x[$80:C2BC] A:7922 X:0006 Y:0007 P:envmxdizc

        // X is one of {6,8,A,C,E,10} here. The ROM values are
        static unsigned short s_ROMValueTable_80C2B6[] = { 0, 0, 0, 0x4, 0xC, 0x1C, 0x3C, 0x7C, 0xFC };
        a = s_ROMValueTable_80C2B6[x / 2];

        // $80/C212 A6 00       LDX $00    [$00:0000]   A:0004 X:0006 Y:0007 P:envmxdizc

        x = mem00.Data16;

        // $80/C214 18          CLC                     A:0004 X:0008 Y:0007 P:envmxdizc

        c = false;

        // $80/C215 65 6F       ADC $6F    [$00:006F]   A:0004 X:0008 Y:0007 P:envmxdizc

        a += mem6f;

        // $80/C217 85 6F       STA $6F    [$00:006F]   A:0006 X:0008 Y:0007 P:envmxdizc

        mem6f = a;

        // $80/C219 60          RTS                     A:0006 X:0008 Y:0007 P:envmxdizc

        return;
    }

    void Fn_80C232()
    {
        // $80/C232 64 6F       STZ $6F    [$00:006F]   A:085C X:000C Y:0000 P:envmxdizc

        mem6f = 0;

        // $80/C234 A5 6C       LDA $6C    [$00:006C]   A:085C X:000C Y:0000 P:envmxdizc

        a = mem6c;

        // $80/C236 0A          ASL A                   A:085C X:000C Y:0000 P:envmxdizc

        willCarry = a >= 0x8000;
        a *= 2;
        c = willCarry;

        // $80/C237 CA          DEX                     A:10B8 X:000C Y:0000 P:envmxdizc

        x--;

        // $80/C238 CA          DEX                     A:10B8 X:000B Y:0000 P:envmxdizc

        x--;
        z = x == 0;

        // $80/C239 F0 15       BEQ $15    [$C250]      A:10B8 X:000A Y:0000 P:envmxdizc

        if (z)
        {
            goto label_C250;
        }

    label_C23B:
        // $80/C23B 90 3A       BCC $3A    [$C277]      A:10B8 X:000A Y:0000 P:envmxdizc

        if (!c)
        {
            goto label_C277;
        }

        ShiftRotateDecrement(0xC23D, 2, 0);

        if (z)
        {
            goto label_C25C;
        }

    label_C244:
        ShiftRotateDecrement(0xC244, 2, 0);

        if (z)
        {
            goto label_C268;
        }

        mem6c = a;

        a = mem6f;
        z = a == 0;

        return;

    label_C250:

        LoadNextFrom0CInc(0xC250);

        x = 0x10;

        goto label_C23B;

    label_C25C:
        LoadNextFrom0CInc(0xC25C);

        x = 0x10;

        goto label_C244;

    label_C268:

        LoadNextFrom0CInc(0xC268);

        x = 0x10;

        mem6c = a;

        a = mem6f;
        z = a == 0;

        // $80/C1CA 60          RTS                     A:0000 X:0000 Y:0005 P:envmxdizc

        return;

    label_C277:

        // $80/C277 A0 02       LDY #$02                A:10B8 X:000A Y:0000 P:envmxdizc

        y = 2;

    label_C279:

        // $80/C279 0A          ASL A                   A:10B8 X:000A Y:0002 P:envmxdizc

        willCarry = a >= 0x8000;
        a *= 2;
        c = willCarry;

        // $80/C27A CA          DEX                     A:2170 X:000A Y:0002 P:envmxdizc

        --x;

        // $80/C27B CA          DEX                     A:2170 X:0009 Y:0002 P:envmxdizc

        --x;
        z = x == 0;

        // $80/C27C F0 24       BEQ $24    [$C2A2]      A:2170 X:0008 Y:0002 P:envmxdizc

        if (z)
        {
            LoadNextFrom0CInc(0xC2A2);

            // $80/C2AA A2 10       LDX #$10                A:8C2E X:0000 Y:0002 P:envmxdizc

            x = 0x10;

            // $80/C2AC 80 D0       BRA $D0    [$C27E]      A:8C2E X:0010 Y:0002 P:envmxdizc

        }

        // $80/C27E C8          INY                     A:2170 X:0008 Y:0002 P:envmxdizc

        y++;

        // $80/C27F 90 F8       BCC $F8    [$C279]      A:2170 X:0008 Y:0003 P:envmxdizc

        if (!c)
        {
            goto label_C279;
        }

        // $80/C281 84 14       STY $14    [$00:0014]   A:0B80 X:0002 Y:0006 P:envmxdizc

        mem14 = y;

    label_C283:

        ShiftRotateDecrement(0xC283, 2, 0);

        // $80/C288 F0 24       BEQ $24    [$C2AE]      A:1700 X:0000 Y:0006 P:envmxdizc

        if (z)
        {
            LoadNextFrom0CInc(0xC2AE);

            // $80/C2B6 A2 10       LDX #$10                A:178C X:0000 Y:0006 P:envmxdizc

            x = 0x10;

            // $80/C2B8 80 D0       BRA $D0    [$C28A]      A:178C X:0010 Y:0006 P:envmxdizc

        }

        // $80/C28A 88          DEY                     A:178C X:0010 Y:0006 P:envmxdizc

        y--;
        z = y == 0;

        // $80/C28B D0 F6       BNE $F6    [$C283]      A:178C X:0010 Y:0005 P:envmxdizc

        if (!z)
        {
            goto label_C283;
        }

        // $80/C28D 85 6C       STA $6C    [$00:006C]   A:F180 X:0006 Y:0000 P:envmxdizc

        mem6c = a;

        // $80/C28F 9B          TXY                     A:F180 X:0006 Y:0000 P:envmxdizc

        y = x;

        // $80/C290 A5 14       LDA $14    [$00:0014]   A:F180 X:0006 Y:0006 P:envmxdizc

        a = mem14;

        // $80/C292 29 FF 00    AND #$00FF              A:0006 X:0006 Y:0006 P:envmxdizc

        a &= 0xFF;

        // $80/C295 0A          ASL A                   A:0006 X:0006 Y:0006 P:envmxdizc

        a *= 2;

        // $80/C296 AA          TAX                     A:000C X:0006 Y:0006 P:envmxdizc

        x = a;

        // $80/C297 BF B6 C2 80 LDA $80C2B6,x[$80:C2C2] A:000C X:000C Y:0006 P:envmxdizc
        // x is one of {6, 8, A, C, 10}
        if (x == 0x6)
        {

            a = 0x4;
        }
        else if (x == 0x8)
        {

            a = 0xC;
        }
        else if (x == 0xA)
        {

            a = 0x1C;
        }
        else if (x == 0xC)
        {

            a = 0x3C;
        }
        else if (x == 0xE)
        {

            a = 0x7C;
        }
        else
        {
            __debugbreak(); // notimpl
        }

        // $80/C29B BB          TYX                     A:003C X:000C Y:0006 P:envmxdizc

        x = y;

        // $80/C29C 18          CLC                     A:003C X:0006 Y:0006 P:envmxdizc

        // $80/C29D 65 6F       ADC $6F    [$00:006F]   A:003C X:0006 Y:0006 P:envmxdizc

        a += mem6f;

        // $80/C29F 85 6F       STA $6F    [$00:006F]   A:003E X:0006 Y:0006 P:envmxdizc

        mem6f = a;
        z = a == 0; // Caller expects this.

        // $80/C2A1 60          RTS                     A:003E X:0006 Y:0006 P:envmxdizc

        return; // Return from WriteIndexed
    }

    void Fn_80C2DC()
    {

    label_C2DC_Start:
        // $80/C2DC 0A          ASL A                   A:9420 X:0008 Y:0006 P:envmxdizc

        a *= 2;

        // $80/C2DD CA          DEX                     A:2840 X:0008 Y:0006 P:envmxdizc

        --x;

        // $80/C2DE CA          DEX                     A:2840 X:0007 Y:0006 P:envmxdizc

        --x;
        z = x == 0;

        // $80/C2DF F0 04       BEQ $04    [$C2E5]      A:2840 X:0006 Y:0006 P:envmxdizc

        if (z)
        {
            goto label_C2E5;
        }

        // $80/C2E1 88          DEY                     A:2840 X:0006 Y:0006 P:envmxdizc

        y--;
        z = y == 0;

        // $80/C2E2 D0 F8       BNE $F8    [$C2DC]      A:2840 X:0006 Y:0005 P:envmxdizc

        if (!z)
        {
            goto label_C2DC_Start;
        }

        // $80/C2E4 60          RTS                     A:085C X:000C Y:0000 P:envmxdizc

        return;

    label_C2E5:

        LoadNextFrom0CInc(0xC2E5);

        // $80/C2ED A2 10       LDX #$10                A:4217 X:0000 Y:0003 P:envmxdizc

        x = 0x10;

        // $80/C2EF 88          DEY                     A:4217 X:0010 Y:0003 P:envmxdizc

        --y;
        z = y == 0;

        // $80/C2F0 D0 EA       BNE $EA    [$C2DC]      A:4217 X:0010 Y:0002 P:envmxdizc

        if (!z)
        {
            goto label_C2DC_Start;
        }

        return;
    }

    bool DoubleAndCheckCarry(unsigned short& sourceDataOffset)
    {
        bool carry = sourceDataOffset >= 0x8000;
        sourceDataOffset *= 2;
        return carry;
    }

    void GetIndexedColor(
        unsigned short resultComponent,
        unsigned short& sourceDataOffset,
        unsigned short& resultLow,
        unsigned short& resultHigh)
    {
        if (DoubleAndCheckCarry(sourceDataOffset))
        {
            resultHigh |= (resultComponent << 8);
        }

        if (DoubleAndCheckCarry(sourceDataOffset))
        {
            resultHigh |= resultComponent;
        }

        if (DoubleAndCheckCarry(sourceDataOffset))
        {
            resultLow |= (resultComponent << 8);
        }

        if (DoubleAndCheckCarry(sourceDataOffset))
        {
            resultLow |= resultComponent;
        }
    }

    bool FnLoadSourceElement(unsigned short& sourceDataAddressLow, unsigned short& sourceDataOffset, unsigned short& resultComponent)
    {
        // Two bytes are loaded at a time.
        while (true)
        {
            loaded16.Low8 = cache7F0000[sourceDataAddressLow + sourceDataOffset + 1];
            loaded16.High8 = cache7F0000[sourceDataAddressLow + sourceDataOffset];

            if (loaded16.Data16 != 0)
            {
                return true;
            }

            if (resultComponent < 16)
            {
                sourceDataOffset = 0;
                return false;
            }

            resultComponent >>= 4;
            sourceDataOffset += 2;
        }

        return true;
    }

    bool Foo(unsigned short& resultComponent, 
        unsigned short sourceDataElement,
        unsigned short& sourceDataOffset,
        unsigned short& resultLow,
        unsigned short& resultHigh)
    {
        while (true)
        {
            GetIndexedColor(resultComponent, sourceDataOffset, resultLow, resultHigh);

            if (resultComponent < 2)
                return false;

            resultComponent /= 2;

            if (resultComponent >= 0x8 && resultComponent < 0x10)
            {
                // resultComponent is [8..15]
                sourceDataOffset = (sourceDataElement * 4) + 2;

                return true;
            }
        }
    }

    void WriteIndexed()
    {
        // Precondition: compressed staging data is written in memory.
        //     Mem0C contains the low short of the source data address.
        //     Mem0E contains the high short of the source data address.  These are always hardcoded to be 0x7F0000.
        //
        //     Mem10 contains the destination offset. It's always within bank $7F.
        //
        // Postcondition: Final indexed data is written to the destination.
        //                It's all in WRAM. This function doesn't do any of the work of DMA transfer to VRAM.

        // The profile images are loaded in "backwards" order. The earliest memory offset one gets loaded last.
        // The output is saved out 2 bytes at a time.
        //
        // This function is called 10 times at the GAME SETUP screen.
        // The 1st invocation writes to somewhere.
        // The 6th invocation writes to 7F5100-7F7380. <-- this run goes through here. This is Kirk Muller's profile image
        //
        // At a high level, this function reads from RAM at 7F0000-7F0484 and writes the final output.

        // Figure out the destination offset based on profile index and whether we're home or away.
        x = 0xA - (currentProfileImageIndex * 2);
        unsigned short destDataAddressLow = mem91_HomeOrAway == 0 ? 0x5100 : 0x2D00;
        destDataAddressLow += Load16FromAddress(0x9D, 0xCCAE + x).Data16;

        unsigned short sourceDataAddressLow = 0;
        unsigned short sourceDataElement = 0; // Counted up. It's always multipled by four. It controls which element of the source data we load.        
        unsigned short lastWrittenElement = 0xFFFE;

        for (int iter = 0; iter < 0x240; ++iter)
        {
            unsigned short resultHigh = 0;
            unsigned short resultLow = 0;
            unsigned short resultComponent = 0x80;
            unsigned short sourceDataOffset = iter * 4;

        LoadSourceElement:
            bool formulateOutput = FnLoadSourceElement(sourceDataAddressLow, sourceDataOffset, resultComponent);

            if (formulateOutput)
            {                
                sourceDataOffset = loaded16.Data16; // We loaded a nonzero element. Save it

                bool loadSourceElement = Foo(resultComponent, sourceDataElement, sourceDataOffset, resultLow, resultHigh);

                if (loadSourceElement)
                {
                    goto LoadSourceElement;
                }
            }

            unsigned short destinationElement = lastWrittenElement + 2;

            if ((destinationElement & 0x10) != 0)
            {
                destinationElement += 0x10;
            }

            lastWrittenElement = destinationElement;

            // Write four bytes of output.

            loaded16.Data16 = resultLow;
            cache7F0000[destDataAddressLow + destinationElement] = loaded16.Low8;
            cache7F0000[destDataAddressLow + destinationElement + 1] = loaded16.High8;

            destinationElement += 0x10;

            loaded16.Data16 = resultHigh;
            cache7F0000[destDataAddressLow + destinationElement] = loaded16.Low8;
            cache7F0000[destDataAddressLow + destinationElement + 1] = loaded16.High8;

            sourceDataElement++;
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
        WriteIndexed();

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

void Decompress_Fast_Init()
{
    // Load ROM file
    Fast::romFile = LoadBinaryFile8("nhl94.sfc");

    Fast::CreateCaches();
}

void Decompress_Fast_Run(int teamIndex, int playerIndex)
{
    Fast::Decompress(teamIndex, playerIndex);
}