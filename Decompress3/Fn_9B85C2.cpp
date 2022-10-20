#include "Util.h"

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

    int pointer0C = 0x7F0000;
    int pointer10 = 0x7F5100;   // For output
    unsigned short mem00 = 0x480;
    unsigned short mem04 = 1;
    unsigned short mem06 = 0;
    unsigned short mem0e = 0x7f;
    unsigned short mem14 = 0;
    unsigned short mem16 = 0;
    unsigned short a = 0x0;
    unsigned short x = 0x0;
    unsigned short y = 0x0;
    bool n = false;
    bool z = false;
    bool c = false;
    bool willCarry = false;
    bool willSetNegative = false;

    // Impl
    // $9B/85C2 A6 00       LDX $00    [$00:0000]   A:0000 X:0000 Y:0000 P:envmxdiZc
    DebugPrint("$9B/85C2 A6 00       LDX $00    [$00:0000]  ", a, x, y);
    x = mem00;

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
    a = mem00;

    // $9B/85D6 4A          LSR A                   A:0480 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85D6 4A          LSR A                  ", a, x, y);
    a >>= 1;

    // $9B/85D7 4A          LSR A                   A:0240 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85D7 4A          LSR A                  ", a, x, y);
    a >>= 1;

    // $9B/85D8 85 00       STA $00    [$00:0000]   A:0120 X:0480 Y:0000 P:envmxdizc
    DebugPrint("$9B/85D8 85 00       STA $00    [$00:0000]  ", a, x, y);
    mem00 = a;

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

    // $9B/85E3 AA          TAX                     A:0008 X:0080 Y:0004 P:envmxdizc
    // $9B/85E4 C8          INY                     A:0008 X:0008 Y:0004 P:envmxdizc
    // $9B/85E5 C8          INY                     A:0008 X:0008 Y:0005 P:envmxdizc
    // $9B/85E6 80 0C       BRA $0C    [$85F4]      A:0008 X:0008 Y:0006 P:envmxdizc

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
        DebugPrint85F4(a, x, y);
        unsigned short token1 = LoadFromRAM(pointer0C + y);
        a = token1;
        z = token1 == 0;

        // $9B/85F6 F0 E4       BEQ $E4    [$85DC]      A:0000 X:0008 Y:0006 P:envmxdiZc
        DebugPrint("$9B/85F6 F0 E4       BEQ $E4    [$85DC]     ", a, x, y);
        if (token1 == 0)
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

    WriteDecompressedOutput(pointer10 + y, mem16);

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

    WriteDecompressedOutput(pointer10 + y, mem14);

    // $9B/8657 E6 04       INC $04    [$00:0004]   A:0000 X:0008 Y:0012 P:envmxdiZc
    DebugPrint("$9B/8657 E6 04       INC $04    [$00:0004]  ", a, x, y);
    mem04++;
    z = mem04 == 0;

    // $9B/8659 C6 00       DEC $00    [$00:0000]   A:0000 X:0008 Y:0012 P:envmxdizc
    DebugPrint("$9B/8659 C6 00       DEC $00    [$00:0000]  ", a, x, y);
    mem00--;
    n = mem00 >= 0x8000;

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