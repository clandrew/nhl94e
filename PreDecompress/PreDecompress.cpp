// PreDecompress.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <assert.h>

int token = 0;
int counter = 8;
int pointer = 0;
unsigned char compressedData[] = { 
	0x30, 0xFB, 0x00, 0x01, 0x00, 0xA1, 0x93, 0xE2, 0x90, 0x21, 0x88, 0xF8, 0x40, 0x04, 0x4B, 0xD6, 0xAF, 0xFF, 0xFD, 0xAB, 0x7F, 0x82, 0x20, 0x07, 0x0E, 0xFD, 0xAE, 0xB7, 0xD9, 0x65, 0x61, 0x5C,
	0xD8, 0xD5, 0x51, 0x4D, 0x48, 0xC1, 0x44, 0x38, 0x38, 0x83, 0xC8, 0x09, 0x00 };

unsigned char array80C2B6[] = { 0xA2, 0x10, 0x80, 0xD0, 0x00, 0x00, 0x04, 0x00, 0x0C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x7C, 0x00 };

int var00 = 0;
int var14 = 0;
int var6C = 0;
int var6F = 0;
int var75 = 0;
int var77 = 0;
int var79 = 0;

int MultiplyBy2WithOverflowCheck(int n, bool* overflow)
{
	int result = n * 2;
	if (result < 0x10000)
	{
		*overflow = false;
		return result;
	}
	else
	{
		*overflow = true;
		return result - 0x10000;
	}
}

void DecrementCounter_LoadNextTokenIfZero()
{
	counter--;
	if (counter == 0)
	{
		token = compressedData[pointer];
		pointer++;
		counter = 8;
	}
}

int RotateLeft(int n)
{
	bool c = n & 0x8000;
	n <<= 1;
	n &= 0xFFFF;
	if (c) n |= 1;
	return n;
}

int PreDecompress()
{
	bool overflow = false;
	token = var6C;

	token = MultiplyBy2WithOverflowCheck(token, &overflow);
	DecrementCounter_LoadNextTokenIfZero();

	if (!overflow)
	{
		int index = 2;
		do
		{
			token = MultiplyBy2WithOverflowCheck(token, &overflow);
			DecrementCounter_LoadNextTokenIfZero();
			index++;
		} while (!overflow);

		int var04 = index;

		do
		{
			token = MultiplyBy2WithOverflowCheck(token, &overflow);
			var6F = RotateLeft(var6F);
			DecrementCounter_LoadNextTokenIfZero();
			index--;
		} while (index > 0);

		var6C = token;
		var04 *= 2;
		index = var04;
		int result = array80C2B6[index] + var6F;
		return result;
	}
	else
	{
		token = MultiplyBy2WithOverflowCheck(token, &overflow);
		var6F = RotateLeft(var6F);
		DecrementCounter_LoadNextTokenIfZero();

		token = MultiplyBy2WithOverflowCheck(token, &overflow);
		var6F = RotateLeft(var6F);
		DecrementCounter_LoadNextTokenIfZero();
		if (counter == 8)
		{
			var6C = token;
			return token;
		}
		else
		{
			// Store result into 6C
			// Load A out of 6F
			var6C = token;
			int result = var6F;
			return result;
		}
	}
}

int main()
{
	int index = 0;
	int initialToken = compressedData[6];
	initialToken <<= 8;
	initialToken |= compressedData[7];
	var6C = initialToken;
	pointer = 8;

	std::vector<int> results;
	std::vector<int> results2;

	for (int j = 0; j < 5; j++)
	{
		int a = PreDecompress();
		results.push_back(a);
		var77 += a;

		a = var6F;
		var75 += a;

		a = var6F;
		if (a == 0)
		{
			results2.push_back(0);
			// PreDecompress again
		}
		else
		{
			var00 = var75;
			if (var14 == 0)
			{
				assert(false);
			}
			else
			{
				bool overflow = false;
				for (int i = 0; i < var14; ++i)
				{
					var00 = MultiplyBy2WithOverflowCheck(var00, &overflow);
				}
				results2.push_back(var00);
				assert(!overflow); // Assuming no overflow here

				a = index;
				a /= 2;
				var79 = a;
				index = 0x3E;
			}
		}
	}
}

/*

Function: PreDecompress()

	Postcondition: compressed data gets loaded into an intermediate buffer
		The long ptr $0C-$0E gets incremented depending on how much gets loaded.
		$6C-$6F contains compression intermediate result
		A contains another result.

$80/C1B0 64 6F       STZ $6F    [$00:006F]   A:0000 X:0000 Y:0008 P:envmXdiZC
$80/C1B2 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0000 Y:0008 P:envmXdiZC	; Load the swapped-bytes of token
$80/C1B4 0A          ASL A                   A:92DA X:0000 Y:0008 P:eNvmXdizC	; *= 2 with overflow. E.g., 27C4 for Montreal

$80/C1B5 88          DEY                     A:25B4 X:0000 Y:0008 P:envmXdizC
$80/C1B6 F0 13       BEQ $13    [$C1CB]      A:25B4 X:0000 Y:0007 P:envmXdizC	; Check to go to CounterIsZero_LoadCompressedByte

$80/C1B8 90 38       BCC $38    [$C1F2]      A:25B4 X:0000 Y:0007 P:envmXdizC	; If the shift didn't overflow, goto TokenDidntOverflow

$80/C1BA 0A          ASL A                   A:25B4 X:0000 Y:0007 P:envmXdizC	; *= 2 with overflow. E.g., 4F88 for Montreal
$80/C1BB 26 6F       ROL $6F    [$00:006F]   A:4B68 X:0000 Y:0007 P:envmXdizc

$80/C1BD 88          DEY                     A:4B68 X:0000 Y:0007 P:envmXdiZc
$80/C1BE F0 17       BEQ $17    [$C1D7]      A:4B68 X:0000 Y:0006 P:envmXdizc	; Check to go to CounterIsZero_LoadCompressedByte2

$80/C1C0 0A          ASL A                   A:4B68 X:0000 Y:0006 P:envmXdizc	; *= 2 with overflow. E.g., 9F10 for Montreal
$80/C1C1 26 6F       ROL $6F    [$00:006F]   A:96D0 X:0000 Y:0006 P:eNvmXdizc
$80/C1C3 88          DEY                     A:96D0 X:0000 Y:0006 P:envmXdiZc
$80/C1C4 F0 1D       BEQ $1D    [$C1E3]      A:96D0 X:0000 Y:0005 P:envmXdizc	; Check to go to CounterIsZero_LoadCompressedByte3AndReturn

$80/C1C6 85 6C       STA $6C    [$00:006C]   A:96D0 X:0000 Y:0005 P:envmXdizc	; Stash the shifted token
$80/C1C8 A5 6F       LDA $6F    [$00:006F]   A:96D0 X:0000 Y:0005 P:envmXdizc	; load result A
$80/C1CA 60          RTS                     A:0000 X:0000 Y:0005 P:envmXdiZc

CounterIsZero_LoadCompressedByte:
$80/C1CB E2 20       SEP #$20                A:AF00 X:003C Y:0000 P:envmXdiZc
$80/C1CD B2 0C       LDA ($0C)  [$9A:E983]   A:AF00 X:003C Y:0000 P:envMXdiZc
$80/C1CF C2 20       REP #$20                A:AFFF X:003C Y:0000 P:eNvMXdizc
$80/C1D1 E6 0C       INC $0C    [$00:000C]   A:AFFF X:003C Y:0000 P:eNvmXdizc
$80/C1D3 A0 08       LDY #$08                A:AFFF X:003C Y:0000 P:eNvmXdizc
$80/C1D5 80 E1       BRA $E1    [$C1B8]      A:AFFF X:003C Y:0008 P:envmXdizc

CounterIsZero_LoadCompressedByte2:
$80/C1D7 E2 20       SEP #$20
$80/C1D9 B2 0C       LDA ($0C)  [$9A:E97A]   A:E200 X:0004 Y:0000 P:envMXdiZc	; Load a byte of decompressed data
$80/C1DB C2 20       REP #$20                A:E290 X:0004 Y:0000 P:eNvMXdizc
$80/C1DD E6 0C       INC $0C    [$00:000C]   A:E290 X:0004 Y:0000 P:eNvmXdizc	; Move ptr to the next
$80/C1DF A0 08       LDY #$08                A:E290 X:0004 Y:0000 P:eNvmXdizc
$80/C1E1 80 DD       BRA $DD    [$C1C0]      A:E290 X:0004 Y:0008 P:envmXdizc

CounterIsZero_LoadCompressedByte3AndReturn:
$80/C1E3 E2 20       SEP #$20                A:0000 X:0000 Y:0008 P:envmXdiZC
$80/C1E5 B2 0C       LDA ($0C)  [$9A:E97A]   A:0000 X:0000 Y:0008 P:envMXdiZC	; Load a byte of decompressed data
$80/C1E7 C2 20       REP #$20                A:0000 X:0000 Y:0008 P:envMXdiZC
$80/C1E9 E6 0C       INC $0C    [$00:000C]   A:0000 X:0000 Y:0008 P:envmXdiZC	; Move ptr to the next
$80/C1EB A0 08       LDY #$08                A:0000 X:0000 Y:0008 P:envmXdiZC
$80/C1ED 85 6C       STA $6C    [$00:006C]   A:0000 X:0000 Y:0008 P:envmXdiZC
$80/C1EF A5 6F       LDA $6F    [$00:006F]   A:0000 X:0000 Y:0008 P:envmXdiZC
$80/C1F1 60          RTS                     A:0000 X:0000 Y:0008 P:envmXdiZC

TokenDidntOverflow:
$80/C1F2 86 00       STX $00    [$00:0000]   A:1080 X:0000 Y:0001 P:envmXdizc
$80/C1F4 A2 02       LDX #$02                A:1080 X:0000 Y:0001 P:envmXdizc

$80/C1F6 0A          ASL A                   A:1080 X:0002 Y:0001 P:envmXdizc
$80/C1F7 88          DEY                     A:2100 X:0002 Y:0001 P:envmXdizc
$80/C1F8 F0 20       BEQ $20    [$C21A]      A:2100 X:0002 Y:0000 P:envmXdiZc	; Check to go to CounterIsZero_LoadCompressedByte4

$80/C1FA E8          INX                     A:2188 X:0002 Y:0008 P:envmXdizc
$80/C1FB 90 F9       BCC $F9    [$C1F6]      A:2188 X:0003 Y:0008 P:envmXdizc	; Check if token didn't overflow, iterate again if so
$80/C1FD 86 04       STX $04    [$00:0004]   A:0C40 X:0006 Y:0005 P:envmXdizC

$80/C1FF 0A          ASL A                   A:9021 X:0003 Y:0008 P:envmXdizc
$80/C200 26 6F       ROL $6F    [$00:006F]   A:2042 X:0003 Y:0008 P:envmXdizC
$80/C202 88          DEY                     A:2042 X:0003 Y:0008 P:envmXdizc
$80/C203 F0 21       BEQ $21    [$C226]      A:2042 X:0003 Y:0007 P:envmXdizc	; Check to go to CounterIsZero_LoadCompressedByte5

$80/C205 CA          DEX                     A:2042 X:0003 Y:0007 P:envmXdizc
$80/C206 D0 F7       BNE $F7    [$C1FF]      A:2042 X:0002 Y:0007 P:envmXdizc

$80/C208 85 6C       STA $6C    [$00:006C]   A:8108 X:0000 Y:0005 P:envmXdiZc
$80/C20A 06 04       ASL $04    [$00:0004]   A:8108 X:0000 Y:0005 P:envmXdiZc
$80/C20C A6 04       LDX $04    [$00:0004]   A:8108 X:0000 Y:0005 P:envmXdizc
$80/C20E BF B6 C2 80 LDA $80C2B6,x[$80:C2BE] A:8108 X:0008 Y:0005 P:envmXdizc
$80/C212 A6 00       LDX $00    [$00:0000]   A:000C X:0008 Y:0005 P:envmXdizc
$80/C214 18          CLC                     A:000C X:0008 Y:0005 P:envmXdizc
$80/C215 65 6F       ADC $6F    [$00:006F]   A:000C X:0008 Y:0005 P:envmXdizc
$80/C217 85 6F       STA $6F    [$00:006F]   A:0010 X:0008 Y:0005 P:envmXdizc
$80/C219 60          RTS                     A:0010 X:0008 Y:0005 P:envmXdizc

CounterIsZero_LoadCompressedByte4:
$80/C21A E2 20       SEP #$20                A:2100 X:0002 Y:0000 P:envmXdiZc
$80/C21C B2 0C       LDA ($0C)  [$9A:E97C]   A:2100 X:0002 Y:0000 P:envMXdiZc
$80/C21E C2 20       REP #$20                A:2188 X:0002 Y:0000 P:eNvMXdizc
$80/C220 E6 0C       INC $0C    [$00:000C]   A:2188 X:0002 Y:0000 P:eNvmXdizc
$80/C222 A0 08       LDY #$08                A:2188 X:0002 Y:0000 P:eNvmXdizc
$80/C224 80 D4       BRA $D4    [$C1FA]      A:2188 X:0002 Y:0008 P:envmXdizc

CounterIsZero_LoadCompressedByte5:
$80/C226 E2 20       SEP #$20                A:7F00 X:0001 Y:0000 P:envmXdiZc
$80/C228 B2 0C       LDA ($0C)  [$9A:E987]   A:7F00 X:0001 Y:0000 P:envMXdiZc
$80/C22A C2 20       REP #$20                A:7F82 X:0001 Y:0000 P:eNvMXdizc
$80/C22C E6 0C       INC $0C    [$00:000C]   A:7F82 X:0001 Y:0000 P:eNvmXdizc
$80/C22E A0 08       LDY #$08                A:7F82 X:0001 Y:0000 P:eNvmXdizc
$80/C230 80 D3       BRA $D3    [$C205]      A:7F82 X:0001 Y:0008 P:envmXdizc

*/