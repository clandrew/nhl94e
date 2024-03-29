// Important note about decompression- it's allowed- and, often, expected- to span multiple Vblank.
// So when debugging you may see control go into the NMI. Skip over these when disassembling.

// void LookupPlayerIndexForPlayerWithProfile() 9DC973
// Preconditions:	Acc is to a player index N going from 0 to 5.
//					Choice of home or away team is in $91. 0 means home, 2 means away
//					There is an array in system memory at 9DC9A2, called OrderOfProfileImagesForThisTeam
//						Each element is one byte.
//						Each element is a number from 0 to 5.
//						The first element determines which player gets their profile image displayed first, etc.
//
// Postconditions: $A5 contains the index of the Nth player on the team who gets a profile image.

$9D/C973 A8          TAY                     A:0005 X:000A Y:0400 P:envmxdizc

$9D/C974 A9 9D 00    LDA #$009D              A:0005 X:000A Y:0005 P:envmxdizc
$9D/C977 85 8F       STA $8F    [$00:008F]   A:009D X:000A Y:0005 P:envmxdizc
$9D/C979 A9 A2 C9    LDA #$C9A2              A:009D X:000A Y:0005 P:envmxdizc
$9D/C97C 85 8D       STA $8D    [$00:008D]   A:C9A2 X:000A Y:0005 P:eNvmxdizc	// $8D-$8F = 9DC9A2 hardcoded
$9D/C97E B7 8D       LDA [$8D],y[$9D:C9A7]   A:C9A2 X:000A Y:0005 P:eNvmxdizc	// 
$9D/C980 29 FF 00    AND #$00FF              A:8500 X:000A Y:0005 P:eNvmxdizc	// 
$9D/C983 48          PHA                     A:0000 X:000A Y:0005 P:envmxdiZc	// Load OrderOfProfileImagesForThisTeam[N] and push on stack.

$9D/C984 A4 91       LDY $91    [$00:0091]   A:0000 X:000A Y:0005 P:envmxdiZc	
$9D/C986 A9 9C 00    LDA #$009C              A:0000 X:000A Y:0000 P:envmxdiZc
$9D/C989 85 8F       STA $8F    [$00:008F]   A:009C X:000A Y:0000 P:envmxdizc
$9D/C98B B9 DC 1C    LDA $1CDC,y[$9F:1CDC]   A:009C X:000A Y:0000 P:envmxdizc
$9D/C98E 85 8D       STA $8D    [$00:008D]   A:C2DB X:000A Y:0000 P:eNvmxdizc	// $8D-$8F = "main table" for the current team.

$9D/C990 A0 0C 00    LDY #$000C              A:C2DB X:000A Y:0000 P:eNvmxdizc
$9D/C993 18          CLC                     A:C2DB X:000A Y:000C P:envmxdizc
$9D/C994 77 8D       ADC [$8D],y[$9C:C2E7]   A:C2DB X:000A Y:000C P:envmxdizc	// 
$9D/C996 85 8D       STA $8D    [$00:008D]   A:C2F0 X:000A Y:000C P:eNvmxdizc	// $8D-$8F += WhichPlayersHaveProfileImagesSeekOffset, 
																				//		which always is the 12th byte of "main table" data.
																				
																				// SeekOffset is the offset, within main table data, of
																				// a six-element array. 
																				// That array is 01 12 11 07 03 0C, for Montreal.
																				// The values in this array are 1-indexed, not 0-indexed :S
																				// This array is the set of players that have profile images.

$9D/C998 7A          PLY                     A:C2F0 X:000A Y:000C P:eNvmxdizc	// Pull OrderOfProfileImagesForThisTeam[N]
$9D/C999 B7 8D       LDA [$8D],y[$9C:C2F0]   A:C2F0 X:000A Y:0000 P:envmxdiZc	// Use it to select within the six-element array above.
$9D/C99B 29 FF 00    AND #$00FF              A:1201 X:000A Y:0000 P:envmxdizc

$9D/C99E 3A          DEC A                   A:0001 X:000A Y:0000 P:envmxdizc
$9D/C99F 85 A5       STA $A5    [$00:00A5]   A:0000 X:000A Y:0000 P:envmxdiZc	// Store the 0-indexed result in $A5.
$9D/C9A1 6B          RTL                     A:0000 X:000A Y:0000 P:envmxdiZc

///////////////////////////////////////////////////////////////////////////////////////////////

// void LoadTeamProfileImagesForPlayerCard() ?-9DC9A8

$9D/C9A8 85 A5       STA $A5    [$00:00A5]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9AA AF 74 34 7E LDA $7E3474[$7E:3474]   A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9AE F0 03       BEQ $03    [$C9B3]      A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9B0 4C 9F CB    JMP $CB9F  [$9D:CB9F]   A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9B3 5A          PHY                     A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9B4 DA          PHX                     A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9B5 A5 91       LDA $91    [$00:0091]   A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9B7 F0 08       BEQ $08    [$C9C1]      A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9B9 A9 00 24    LDA #$2400              A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9BC 8D 87 1E    STA $1E87  [$9F:1E87]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9BF 80 06       BRA $06    [$C9C7]      A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9C1 A9 00 34    LDA #$3400              A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9C4 8D 87 1E    STA $1E87  [$9F:1E87]   A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9C7 A5 A5       LDA $A5    [$00:00A5]   A:0000 X:0490 Y:0000 P:envmxdiZc	; Basically look up the right player's compressed data
$9D/C9C9 A4 91       LDY $91    [$00:0091]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9CB 85 A5       STA $A5    [$00:00A5]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9CD B9 98 1C    LDA $1C98,y[$9F:1C98]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9D0 A2 1A 00    LDX #$001A              A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9D3 22 4C 8F 80 JSL $808F4C[$80:8F4C]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9D7 18          CLC                     A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9D8 65 A5       ADC $A5    [$00:00A5]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9DA 0A          ASL A                   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9DB 0A          ASL A                   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9DC AA          TAX                     A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9DD BF 53 CD 9D LDA $9DCD53,x[$9D:D1E3] A:0000 X:0490 Y:0000 P:envmxdiZc	// DecompressActual1 src = e.g., 970490
$9D/C9E1 85 0C       STA $0C    [$00:000C]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9E3 BF 55 CD 9D LDA $9DCD55,x[$9D:D1E5] A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9E7 85 0E       STA $0E    [$00:000E]   A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9E9 A9 7F 00    LDA #$007F              A:0000 X:0490 Y:0000 P:envmxdiZc	// DecompressActual1 dest = 7F0000
$9D/C9EC 85 12       STA $12    [$00:0012]   A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9EE A9 00 00    LDA #$0000              A:0000 X:0490 Y:0000 P:envmxdiZc
$9D/C9F1 85 10       STA $10    [$00:0010]   A:0000 X:0490 Y:0000 P:envmxdiZc

$9D/C9F3 22 73 C3 80 JSL $80C373[$80:C373]   A:8000 X:0480 Y:0000 P:envmxdiZc	// call DecompressActual1()
$9D/C9F7 86 00       STX $00    [$00:0000]   A:8000 X:0480 Y:0000 P:envmxdiZc
$9D/C9F9 A5 91       LDA $91    [$00:0091]   A:8000 X:0480 Y:0000 P:envmxdiZc
$9D/C9FB F0 0B       BEQ $0B    [$CA08]      A:0000 X:0480 Y:0000 P:envmxdiZc

$9D/CA08 A9 00 6F    LDA #$6F00              A:0000 X:0480 Y:0000 P:envmxdiZc
$9D/CA0B 85 10       STA $10    [$00:0010]   A:6F00 X:0480 Y:0000 P:envmxdizc	// DecompressActual2 dst = 7F6F00

$9D/CA0D 8F 70 34 7E STA $7E3470[$7E:3470]   A:6F00 X:0480 Y:0000 P:envmxdizc

$9D/CA11 A9 7F 00    LDA #$007F              A:6F00 X:0480 Y:0000 P:envmxdizc	// DecompressActual2 src = 7F0000
$9D/CA14 85 0E       STA $0E    [$00:000E]   A:007F X:0480 Y:0000 P:envmxdizc
$9D/CA16 A9 00 00    LDA #$0000              A:007F X:0480 Y:0000 P:envmxdizc
$9D/CA19 85 0C       STA $0C    [$00:000C]   A:0000 X:0480 Y:0000 P:envmxdiZc

$9D/CA1B 22 C2 85 9B JSL $9B85C2[$9B:85C2]   A:0000 X:0480 Y:0000 P:envmxdiZc	// Call DecompressActual2()

$9D/CA1F 22 1C CB 9D JSL $9DCB1C[$9D:CB1C]   A:C0C0 X:0000 Y:0490 P:eNvmxdizc
$9D/CA23 D0 31       BNE $31    [$CA56]      A:0000 X:0000 Y:0002 P:envmxdiZC
$9D/CA25 A9 92 00    LDA #$0092              A:0000 X:0000 Y:0002 P:envmxdiZC
$9D/CA28 85 0E       STA $0E    [$00:000E]   A:0092 X:0000 Y:0002 P:envmxdizC
$9D/CA2A A9 B0 FF    LDA #$FFB0              A:0092 X:0000 Y:0002 P:envmxdizC
$9D/CA2D 85 0C       STA $0C    [$00:000C]   A:FFB0 X:0000 Y:0002 P:eNvmxdizC
$9D/CA2F A9 7E 00    LDA #$007E              A:FFB0 X:0000 Y:0002 P:eNvmxdizC
$9D/CA32 85 12       STA $12    [$00:0012]   A:007E X:0000 Y:0002 P:envmxdizC
$9D/CA34 A9 F4 37    LDA #$37F4              A:007E X:0000 Y:0002 P:envmxdizC
$9D/CA37 85 10       STA $10    [$00:0010]   A:37F4 X:0000 Y:0002 P:envmxdizC
$9D/CA39 A9 20 00    LDA #$0020              A:37F4 X:0000 Y:0002 P:envmxdizC
$9D/CA3C 85 24       STA $24    [$00:0024]   A:0020 X:0000 Y:0002 P:envmxdizC
$9D/CA3E AF D2 35 7E LDA $7E35D2[$7E:35D2]   A:0020 X:0000 Y:0002 P:envmxdizC
$9D/CA42 18          CLC                     A:0372 X:0000 Y:0002 P:envmxdizC
$9D/CA43 6D 87 1E    ADC $1E87  [$9F:1E87]   A:0372 X:0000 Y:0002 P:envmxdizc
$9D/CA46 A4 91       LDY $91    [$00:0091]   A:3772 X:0000 Y:0002 P:envmxdizc
$9D/CA48 F0 04       BEQ $04    [$CA4E]      A:3772 X:0000 Y:0000 P:envmxdiZc
$9D/CA4E FA          PLX                     A:3772 X:0000 Y:0000 P:envmxdiZc
$9D/CA4F 7A          PLY                     A:3772 X:000D Y:0000 P:envmxdizc
$9D/CA50 22 2D 8E 80 JSL $808E2D[$80:8E2D]   A:3772 X:000D Y:0004 P:envmxdizc
$9D/CA54 80 2F       BRA $2F    [$CA85]      A:FFFE X:0000 Y:000C P:envmxdiZc

$9D/CA85 A4 91       LDY $91    [$00:0091]   A:FFFE X:0000 Y:000C P:envmxdiZc
$9D/CA87 B9 98 1C    LDA $1C98,y[$9F:1C98]   A:FFFE X:0000 Y:0000 P:envmxdiZc
$9D/CA8A 0A          ASL A                   A:000B X:0000 Y:0000 P:envmxdizc
$9D/CA8B 0A          ASL A                   A:0016 X:0000 Y:0000 P:envmxdizc
$9D/CA8C 18          CLC                     A:002C X:0000 Y:0000 P:envmxdizc
$9D/CA8D 69 B3 D8    ADC #$D8B3              A:002C X:0000 Y:0000 P:envmxdizc
$9D/CA90 85 8D       STA $8D    [$00:008D]   A:D8DF X:0000 Y:0000 P:eNvmxdizc
$9D/CA92 A9 9D 00    LDA #$009D              A:D8DF X:0000 Y:0000 P:eNvmxdizc
$9D/CA95 85 8F       STA $8F    [$00:008F]   A:009D X:0000 Y:0000 P:envmxdizc
$9D/CA97 A7 8D       LDA [$8D]  [$9D:D8DF]   A:009D X:0000 Y:0000 P:envmxdizc
$9D/CA99 85 0C       STA $0C    [$00:000C]   A:EB9C X:0000 Y:0000 P:eNvmxdizc
$9D/CA9B E6 8D       INC $8D    [$00:008D]   A:EB9C X:0000 Y:0000 P:eNvmxdizc
$9D/CA9D E6 8D       INC $8D    [$00:008D]   A:EB9C X:0000 Y:0000 P:eNvmxdizc
$9D/CA9F A7 8D       LDA [$8D]  [$9D:D8E1]   A:EB9C X:0000 Y:0000 P:eNvmxdizc
$9D/CAA1 85 0E       STA $0E    [$00:000E]   A:009A X:0000 Y:0000 P:envmxdizc
$9D/CAA3 A5 91       LDA $91    [$00:0091]   A:009A X:0000 Y:0000 P:envmxdizc
$9D/CAA5 F0 05       BEQ $05    [$CAAC]      A:0000 X:0000 Y:0000 P:envmxdiZc
$9D/CAAC A0 50 00    LDY #$0050              A:0000 X:0000 Y:0000 P:envmxdiZc
$9D/CAAF A2 10 00    LDX #$0010              A:0000 X:0000 Y:0050 P:envmxdizc
$9D/CAB2 22 BD E0 9D JSL $9DE0BD[$9D:E0BD]   A:0000 X:0010 Y:0050 P:envmxdizc
$9D/CAB6 6B          RTL                     A:6000 X:00C0 Y:0020 P:envmxdiZc	// Ret

// There's more that comes after here, stuff in this fn jumps there

///////////////////////////////////////////////////////////////////////////////////////////////
void DecompressFB30()		80BBB3
// Here we goooo...
// Preconditions: $0C-$0E stores a pointer to compressed data.
//                The compressed data is of FB30 type, and starts with the bytes 'FB 30'.
//
//				  $10-$11 also points to the result location.
//				  Output vector $2181-$2182 has been initialized to the place where we write the result.
//				  Generally this is 7F0000 in practice.
//
//				  Argument: $75
//				  Argument: $77
//
// Postconditions:A decompressed intermediate is written to the output vector.
//                This doesn't move anything to VRAM, it works in system memory only.
//
//                9A0720 in RAM (shadowed to 7E0720) has been initialized with some argument type thing.
//				  It's an array. Each element is 2 bytes. There are about 12 elements.
//
//                9A0700 in RAM (shadowed to 7E0700) has been initialized with some argument type thing.
//				  It's an array, like the above. Each element is 2 bytes. 
//
//				  Scrambles $14.
//			      Scrambles $6C.
//		          Scrambles $6F.
//		          Scrambles $73.
//
// Comments: This function is an absolute monstrosity. About 850 ops.
//		

LoadCompressedGraphics_Begin:
$80/BBB3 E2 10       SEP #$10                A:FB30 X:0040 Y:E974 P:envmxdiZC	; Use 8-bit indexing mode
$80/BBB5 A5 0C       LDA $0C    [$00:000C]   A:FB30 X:0040 Y:0074 P:envmXdiZC
$80/BBB7 18          CLC                     A:E972 X:0040 Y:0074 P:eNvmXdizC
$80/BBB8 69 05 00    ADC #$0005              A:E972 X:0040 Y:0074 P:eNvmXdizc	; Seek to the 5th byte of the compressed data
$80/BBBB 85 0C       STA $0C    [$00:000C]   A:E977 X:0040 Y:0074 P:eNvmXdizc

$80/BBBD B2 0C       LDA ($0C)  [$9A:E977]   A:E977 X:0040 Y:0074 P:eNvmXdizc   ; Dereference
$80/BBBF 85 73       STA $73    [$00:0073]   A:93A1 X:0040 Y:0074 P:eNvmXdizc
$80/BBC1 E6 0C       INC $0C    [$00:000C]   A:93A1 X:0040 Y:0074 P:eNvmXdizc	; save it, the "lim", to $73


$80/BBC3 B2 0C       LDA ($0C)  [$9A:E978]   A:93A1 X:0040 Y:0074 P:eNvmXdizc	; Load the 2-byte "token". E.g., E293 for Montreal
$80/BBC5 E6 0C       INC $0C    [$00:000C]   A:E293 X:0040 Y:0074 P:eNvmXdizc
$80/BBC7 E6 0C       INC $0C    [$00:000C]   A:E293 X:0040 Y:0074 P:eNvmXdizc
$80/BBC9 EB          XBA                     A:E293 X:0040 Y:0074 P:eNvmXdizc	; -> 93E2
$80/BBCA 85 6C       STA $6C    [$00:006C]   A:93E2 X:0040 Y:0074 P:eNvmXdizc	; Save the 2-byte "token" to $6C

$80/BBCC A0 08       LDY #$08                A:93E2 X:0040 Y:0074 P:eNvmXdizc	; Set counter. This controls how many bytes of compressed data
                                                                                ; PreDecompress will read
$80/BBCE 64 77       STZ $77    [$00:0077]   A:93E2 X:0040 Y:0008 P:envmXdizc
$80/BBD0 64 75       STZ $75    [$00:0075]   A:93E2 X:0040 Y:0008 P:envmXdizc
$80/BBD2 A9 10 00    LDA #$0010              A:93E2 X:0040 Y:0008 P:envmXdizc

$80/BBD5 85 14       STA $14    [$00:0014]   A:0010 X:0040 Y:0008 P:envmXdizc
$80/BBD7 A2 FE       LDX #$FE                A:0010 X:0040 Y:0008 P:envmXdizc	; x=-2

BeforeDecompressLoop:
$80/BBD9 E8          INX                     A:0010 X:00FE Y:0008 P:eNvmXdizc
$80/BBDA E8          INX                     A:0010 X:00FF Y:0008 P:eNvmXdizc	; Increment the offset that PreDecompress will store its result to.
																				; E.g., this starts at 0 and is {0, 2, 4, 6, 8} for Montreal.

$80/BBDB C6 14       DEC $14    [$00:0014]   A:0010 X:0000 Y:0008 P:envmXdiZc	; 
$80/BBDD 06 75       ASL $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmXdizc	; $14 starts at 0x10 and is decremented by 1 each iteration
$80/BBDF A5 75       LDA $75    [$00:0075]   A:0010 X:0000 Y:0008 P:envmXdiZc
$80/BBE1 38          SEC                     A:0000 X:0000 Y:0008 P:envmXdiZc
$80/BBE2 E5 77       SBC $77    [$00:0077]   A:0000 X:0000 Y:0008 P:envmXdiZC

$80/BBE4 9D 20 07    STA $0720,x[$9A:0720]   A:0000 X:0000 Y:0008 P:envmXdiZC	; 9A0720 contains some intermediate thing


																				; For first iter for montreal, $0C = 9AE97A
$80/BBE7 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:0000 X:0000 Y:0008 P:envmXdiZC	; Call PreDecompress(). Eats $6F
																				; Part of result gets stored in $6C
																		
$80/BBEA 9D 00 07    STA $0700,x[$97:070C]   A:8004 X:000C Y:0000 P:eNvmXdizc	; Store result, shadowed at 7E0700. For Montreal, these are {0, 0, 3, 2, 0x10}

$80/BBED 18          CLC                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BBEE 65 77       ADC $77    [$00:0077]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BBF0 85 77       STA $77    [$00:0077]   A:8004 X:000C Y:0000 P:eNvmXdizc	

$80/BBF2 A5 6F       LDA $6F    [$00:006F]   A:8004 X:000C Y:0000 P:eNvmXdizc	
$80/BBF4 18          CLC                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BBF5 65 75       ADC $75    [$00:0075]   A:8004 X:000C Y:0000 P:eNvmXdizc	
$80/BBF7 85 75       STA $75    [$00:0075]   A:8004 X:000C Y:0000 P:eNvmXdizc	; $75 += decompressed intermediate

$80/BBF9 A5 6F       LDA $6F    [$00:006F]   A:8004 X:000C Y:0000 P:eNvmXdizc	; Load arg. If arg==0, PreDecompress gets called again.
$80/BBFB D0 05       BNE $05    [$BC02]      A:8004 X:000C Y:0000 P:eNvmXdizc

PreDecompressYieldedZero:
$80/BBFD 9E 40 07    STZ $0740,x[$97:074C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC00 80 D7       BRA $D7    [$BBD9]      A:8004 X:000C Y:0000 P:eNvmXdizc	; Pre-decompress again. goto BeforeDecompressLoop

PreDecompressYieldedNonZero:
$80/BC02 A5 75       LDA $75    [$00:0075]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC04 85 00       STA $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC06 A5 14       LDA $14    [$00:0014]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC08 18          CLC                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC09 F0 05       BEQ $05    [$BC10]      A:8004 X:000C Y:0000 P:eNvmXdizc	; check to goto DoneDecrementLoop

DecrementLoop:
$80/BC0B 06 00       ASL $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC0D 3A          DEC A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC0E D0 FB       BNE $FB    [$BC0B]      A:8004 X:000C Y:0000 P:eNvmXdizc	; Tight loop. $14 -= A

DoneDecrementLoop:
$80/BC10 A5 00       LDA $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BC12 9D 40 07    STA $0740,x[$97:074C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC15 90 C2       BCC $C2    [$BBD9]      A:8004 X:000C Y:0000 P:eNvmXdizc	; conditional branch to BeforeDecompressLoop
$80/BC17 8A          TXA                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC18 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC19 85 79       STA $79    [$00:0079]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC1B A2 3E       LDX #$3E                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC1D 9E 00 05    STZ $0500,x[$97:050C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC20 9E 40 05    STZ $0540,x[$97:054C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC23 9E 80 05    STZ $0580,x[$97:058C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC26 9E C0 05    STZ $05C0,x[$97:05CC]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC29 CA          DEX                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC2A CA          DEX                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC2B 10 F0       BPL $F0    [$BC1D]      A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BC2D A2 7E       LDX #$7E                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC2F 8E 83 21    STX $2183  [$97:2183]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC32 A9 00 01    LDA #$0100              A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC35 8D 81 21    STA $2181  [$97:2181]   A:8004 X:000C Y:0000 P:eNvmXdizc	; Set transfer address to $7E0100

$80/BC38 A2 FF       LDX #$FF                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC3A 20 B0 C1    JSR $C1B0  [$80:C1B0]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC3D E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC3F 1A          INC A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC40 E8          INX                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC41 3C 00 05    BIT $0500,x[$97:050C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC44 30 FA       BMI $FA    [$BC40]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC46 3A          DEC A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC47 D0 F7       BNE $F7    [$BC40]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC49 DE 00 05    DEC $0500,x[$97:050C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC4C C2 20       REP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC4E 8E 80 21    STX $2180  [$97:2180]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BC51 C6 77       DEC $77    [$00:0077]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC53 D0 E5       BNE $E5    [$BC3A]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC55 98          TYA                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC56 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC57 85 71       STA $71    [$00:0071]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC59 E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BC5B A2 00       LDX #$00                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC5D 9B          TXY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC5E 64 7B       STZ $7B    [$00:007B]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC60 86 00       STX $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC62 A5 7B       LDA $7B    [$00:007B]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC64 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC65 AA          TAX                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC66 E0 10       CPX #$10                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC68 F0 55       BEQ $55    [$BCBF]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC6A BD 00 07    LDA $0700,x[$97:070C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC6D 85 77       STA $77    [$00:0077]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC6F A6 7B       LDX $7B    [$00:007B]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC71 BF 7B BC 80 LDA $80BC7B,x[$80:BC87] A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC75 85 7D       STA $7D    [$00:007D]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC77 A6 00       LDX $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC79 80 36       BRA $36    [$BCB1]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BC7B 81 41       STA ($41,x)[$97:89D0]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC7D 21 11       AND ($11,x)[$97:0C00]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC7F 09 05       ORA #$05                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC81 03 02       ORA $02,s  [$00:1FF0]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC83 A5 7B       LDA $7B    [$00:007B]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC85 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC86 85 00       STA $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC88 B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC8B 85 01       STA $01    [$00:0001]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC8D C8          INY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC8E C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC90 D0 09       BNE $09    [$BC9B]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC92 A5 7B       LDA $7B    [$00:007B]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC94 1A          INC A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC95 85 74       STA $74    [$00:0074]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC97 A9 12       LDA #$12                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC99 85 00       STA $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC9B 84 04       STY $04    [$00:0004]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC9D A4 7D       LDY $7D    [$00:007D]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BC9F 80 0B       BRA $0B    [$BCAC]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCA1 A5 01       LDA $01    [$00:0001]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCA3 9D 00 05    STA $0500,x[$97:050C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCA6 A5 00       LDA $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCA8 9D 00 06    STA $0600,x[$97:060C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCAB E8          INX                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCAC 88          DEY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCAD D0 F2       BNE $F2    [$BCA1]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCAF A4 04       LDY $04    [$00:0004]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCB1 C6 77       DEC $77    [$00:0077]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCB3 10 CE       BPL $CE    [$BC83]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCB5 E6 7B       INC $7B    [$00:007B]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCB7 A5 79       LDA $79    [$00:0079]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCB9 C5 7B       CMP $7B    [$00:007B]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCBB B0 A3       BCS $A3    [$BC60]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCBD 86 00       STX $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCBF A6 00       LDX $00    [$00:0000]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCC1 F0 08       BEQ $08    [$BCCB]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCC3 A9 10       LDA #$10                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCC5 9D 00 06    STA $0600,x[$97:060C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCC8 E8          INX                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCC9 D0 FA       BNE $FA    [$BCC5]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCCB A5 79       LDA $79    [$00:0079]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCCD 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCCE AA          TAX                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCCF C2 20       REP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BCD1 BF D9 BC 80 LDA $80BCD9,x[$80:BCE5] A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCD5 8D 60 07    STA $0760  [$97:0760]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BCD8 A4 12       LDY $12    [$00:0012]   A:8004 X:000C Y:0000 P:eNvmXdizc	; Set DMA transfer address based on $12, $10
$80/BCDA 8C 83 21    STY $2183  [$97:2183]   A:8004 X:000C Y:0000 P:eNvmXdizc	; It's a transfer from ROM to RAM.
$80/BCDD A5 10       LDA $10    [$00:0010]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCDF 8D 81 21    STA $2181  [$97:2181]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BCE2 A5 6C       LDA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCE4 A6 71       LDX $71    [$00:0071]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCE6 7C F9 BC    JMP ($BCF9,x)[$80:BDBE] A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCE9 C5 BF       CMP $BF    [$00:00BF]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCEB C8          INY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCEC BF D0 BF DD LDA $DDBFD0,x[$DD:BFDC] A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BCF0 BF EF BF 06 LDA $06BFEF,x[$06:BFFB] A:8004 X:000C Y:0000 P:eNvmXdizc

Data:
$80/BCF4 C0 22 C0 43 C0 

$80/BCF9 00 00 53 BF 00 BF AE BE 5D BE 0D BE BE BD 70 BD 23 BD 

$80/BD0B 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BD0C 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BD0D 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BD0E 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BD0F 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BD10 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BD11 0A          ASL A                   A:3D80 X:0002 Y:003D P:envmXdizC
$80/BD12 0A          ASL A                   A:7B00 X:0002 Y:003D P:envmXdizc
$80/BD13 E2 20       SEP #$20                A:F600 X:0002 Y:003D P:eNvmXdizc
$80/BD15 B2 0C       LDA ($0C)  [$81:CE36]   A:F600 X:0002 Y:003D P:eNvMXdizc
$80/BD17 C2 20       REP #$20                A:F68F X:0002 Y:003D P:eNvMXdizc
$80/BD19 E6 0C       INC $0C    [$00:000C]   A:F68F X:0002 Y:003D P:eNvmXdizc
$80/BD1B BE 00 05    LDX $0500,y[$81:053D]   A:F68F X:0002 Y:003D P:eNvmXdizc
$80/BD1E 8E 80 21    STX $2180  [$81:2180]   A:F68F X:0000 Y:003D P:envmXdiZc
$80/BD21 86 08       STX $08    [$00:0008]   A:F68F X:0000 Y:003D P:envmXdiZc
$80/BD23 85 6C       STA $6C    [$00:006C]   A:F68F X:0000 Y:003D P:envmXdiZc
$80/BD25 A4 6D       LDY $6D    [$00:006D]   A:F68F X:0000 Y:003D P:envmXdiZc
$80/BD27 BE 00 06    LDX $0600,y[$81:06F6]   A:F68F X:0000 Y:00F6 P:eNvmXdizc
$80/BD2A 7C 2D BD    JMP ($BD2D,x)[$80:BD46] A:F68F X:0010 Y:00F6 P:envmXdizc	; can go to BD46 or BD41

; This is an array of short pointers, jumpable locations
$80/BD2D 67 BD B4 BD 02 BE 51 BE A1 BE F2 BE 44 BF 0B BD 46 BD 41 BD

$80/BD41 A2 10       LDX #$10                A:A655 X:0012 Y:00A6 P:envmXdizc
$80/BD43 4C 7C C1    JMP $C17C  [$80:C17C]   A:A655 X:0010 Y:00A6 P:envmXdizc

$80/BD46 A2 10       LDX #$10                A:F68F X:0010 Y:00F6 P:envmXdizc
$80/BD48 64 6A       STZ $6A    [$00:006A]   A:F68F X:0010 Y:00F6 P:envmXdizc
$80/BD4A B2 0C       LDA ($0C)  [$81:CE37]   A:F68F X:0010 Y:00F6 P:envmXdizc
$80/BD4C 29 FF 00    AND #$00FF              A:F668 X:0010 Y:00F6 P:eNvmXdizc
$80/BD4F 05 6B       ORA $6B    [$00:006B]   A:0068 X:0010 Y:00F6 P:envmXdizc
$80/BD51 85 6B       STA $6B    [$00:006B]   A:8F68 X:0010 Y:00F6 P:eNvmXdizc
$80/BD53 A5 6C       LDA $6C    [$00:006C]   A:8F68 X:0010 Y:00F6 P:eNvmXdizc
$80/BD55 6C 60 07    JMP ($0760)[$80:BFDD]   A:F68F X:0010 Y:00F6 P:eNvmXdizc

$80/BD59 0A          ASL A                   A:D96C X:000C Y:00D9 P:envmXdizc

$80/BD5A 0A          ASL A                   A:8F60 X:000A Y:008F P:envmXdizC
$80/BD5B 0A          ASL A                   A:1EC0 X:000A Y:008F P:envmXdizC
$80/BD5C 0A          ASL A                   A:3D80 X:000A Y:008F P:envmXdizc
$80/BD5D 0A          ASL A                   A:7B00 X:000A Y:008F P:envmXdizc
$80/BD5E 0A          ASL A                   A:F600 X:000A Y:008F P:eNvmXdizc
$80/BD5F E2 20       SEP #$20                A:EC00 X:000A Y:008F P:eNvmXdizC
$80/BD61 B2 0C       LDA ($0C)  [$81:D58A]   A:EC00 X:000A Y:008F P:eNvMXdizC
$80/BD63 C2 20       REP #$20                A:EC30 X:000A Y:008F P:envMXdizC
$80/BD65 E6 0C       INC $0C    [$00:000C]   A:EC30 X:000A Y:008F P:envmXdizC
$80/BD67 0A          ASL A                   A:EC30 X:000A Y:008F P:eNvmXdizC
$80/BD68 BE 00 05    LDX $0500,y[$81:058F]   A:D860 X:000A Y:008F P:eNvmXdizC
$80/BD6B 8E 80 21    STX $2180  [$81:2180]   A:D860 X:00F0 Y:008F P:eNvmXdizC
$80/BD6E 86 08       STX $08    [$00:0008]   A:D860 X:00F0 Y:008F P:eNvmXdizC

$80/BD70 85 6C       STA $6C    [$00:006C]   A:CBF0 X:0002 Y:0000 P:envmXdiZc
$80/BD72 A4 6D       LDY $6D    [$00:006D]   A:CBF0 X:0002 Y:0000 P:envmXdiZc
$80/BD74 BE 00 06    LDX $0600,y[$81:06CB]   A:CBF0 X:0002 Y:00CB P:eNvmXdizc
$80/BD77 7C 7A BD    JMP ($BD7A,x)[$80:BD93] A:CBF0 X:0010 Y:00CB P:envmXdizc	; Can go to 80BD8E

; Array of short pointers, jumpable
$80/BD7A B5 BD 03 BE 52 BE A2 BE F3 BE 45 BF 0C BD 58 BD 93 BD 8E BD A2

$80/BD8E A2 0E       LDX #$0E                A:B640 X:0012 Y:00B6 P:envmXdizc
$80/BD90 4C 7C C1    JMP $C17C  [$80:C17C]   A:B640 X:000E Y:00B6 P:envmXdizc

$80/BD93 A2 0E       LDX #$0E                A:CBF0 X:0010 Y:00CB P:envmXdizc
$80/BD95 64 6A       STZ $6A    [$00:006A]   A:CBF0 X:000E Y:00CB P:envmXdizc
$80/BD97 B2 0C       LDA ($0C)  [$81:D587]   A:CBF0 X:000E Y:00CB P:envmXdizc
$80/BD99 29 FF 00    AND #$00FF              A:31D6 X:000E Y:00CB P:envmXdizc

$80/BD9C 0A          ASL A                   A:00D6 X:000E Y:00CB P:envmXdizc
$80/BD9D 05 6B       ORA $6B    [$00:006B]   A:01AC X:000E Y:00CB P:envmXdizc
$80/BD9F 85 6B       STA $6B    [$00:006B]   A:F1AC X:000E Y:00CB P:eNvmXdizc
$80/BDA1 A5 6C       LDA $6C    [$00:006C]   A:F1AC X:000E Y:00CB P:eNvmXdizc
$80/BDA3 6C 60 07    JMP ($0760)[$80:BFDD]   A:CBF1 X:000E Y:00CB P:eNvmXdizc

$80/BDA6 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BDA7 0A          ASL A                   A:D9B0 X:000C Y:00D9 P:envmXdizc
$80/BDA8 0A          ASL A                   A:B360 X:000C Y:00D9 P:eNvmXdizC
$80/BDA9 0A          ASL A                   A:66C0 X:000C Y:00D9 P:envmXdizC
$80/BDAA 0A          ASL A                   A:CD80 X:000C Y:00D9 P:eNvmXdizc
$80/BDAB 0A          ASL A                   A:9B00 X:000C Y:00D9 P:eNvmXdizC
$80/BDAC E2 20       SEP #$20                A:3600 X:000C Y:00D9 P:envmXdizC
$80/BDAE B2 0C       LDA ($0C)  [$97:E199]   A:3600 X:000C Y:00D9 P:envMXdizC
$80/BDB0 C2 20       REP #$20                A:365B X:000C Y:00D9 P:envMXdizC
$80/BDB2 E6 0C       INC $0C    [$00:000C]   A:365B X:000C Y:00D9 P:envmXdizC
$80/BDB4 0A          ASL A                   A:365B X:000C Y:00D9 P:eNvmXdizC
$80/BDB5 0A          ASL A                   A:6CB6 X:000C Y:00D9 P:envmXdizc
$80/BDB6 BE 00 05    LDX $0500,y[$97:05D9]   A:D96C X:000C Y:00D9 P:eNvmXdizc
$80/BDB9 8E 80 21    STX $2180  [$97:2180]   A:D96C X:00FF Y:00D9 P:eNvmXdizc
$80/BDBC 86 08       STX $08    [$00:0008]   A:D96C X:00FF Y:00D9 P:eNvmXdizc

$80/BDBE 85 6C       STA $6C    [$00:006C]   A:314C X:0010 Y:0000 P:envmXdiZc
$80/BDC0 A4 6D       LDY $6D    [$00:006D]   A:314C X:0010 Y:0000 P:envmXdiZc
$80/BDC2 BE 00 06    LDX $0600,y[$81:0631]   A:314C X:0010 Y:0031 P:envmXdizc
$80/BDC5 7C C8 BD    JMP ($BDC8,x)[$80:BE53] A:314C X:0002 Y:0031 P:envmXdizc	; Can go to BDE1, BD59

// Jump table
$80/BDC8 04 BE 53 BE A3 BE F4 BE 46 BF 0D BD 59 BD A6 BD E1 BD DC BD 
$80/BDDC A2 0C 4C 7C C1 A2

x$80/BDE1 A2 0C       LDX #$0C                A:E358 X:0010 Y:00E3 P:envmXdizC
x$80/BDE3 64 6A       STZ $6A    [$00:006A]   A:E358 X:000C Y:00E3 P:envmXdizC
x$80/BDE5 B2 0C       LDA ($0C)  [$81:D588]   A:E358 X:000C Y:00E3 P:envmXdizC
x$80/BDE7 29 FF 00    AND #$00FF              A:EC31 X:000C Y:00E3 P:eNvmXdizC
x$80/BDEA 0A          ASL A                   A:0031 X:000C Y:00E3 P:envmXdizC
x$80/BDEB 0A          ASL A                   A:0062 X:000C Y:00E3 P:envmXdizc
x$80/BDEC 05 6B       ORA $6B    [$00:006B]   A:00C4 X:000C Y:00E3 P:envmXdizc
x$80/BDEE 85 6B       STA $6B    [$00:006B]   A:58C4 X:000C Y:00E3 P:envmXdizc
x$80/BDF0 A5 6C       LDA $6C    [$00:006C]   A:58C4 X:000C Y:00E3 P:envmXdizc
x$80/BDF2 6C 60 07    JMP ($0760)[$80:BFDD]   A:E358 X:000C Y:00E3 P:eNvmXdizc

$80/BDF5 0A          ASL A                   A:B188 X:000E Y:00B1 P:envmXdizc

$80/BDF6 0A          ASL A                   A:6310 X:000E Y:00B1 P:envmXdizC
$80/BDF7 0A          ASL A                   A:C620 X:000E Y:00B1 P:eNvmXdizc
$80/BDF8 0A          ASL A                   A:8C40 X:000E Y:00B1 P:eNvmXdizC
$80/BDF9 0A          ASL A                   A:1880 X:000E Y:00B1 P:envmXdizC
$80/BDFA E2 20       SEP #$20                A:3100 X:000E Y:00B1 P:envmXdizc
$80/BDFC B2 0C       LDA ($0C)  [$81:D589]   A:3100 X:000E Y:00B1 P:envMXdizc
$80/BDFE C2 20       REP #$20                A:31EC X:000E Y:00B1 P:eNvMXdizc
$80/BE00 E6 0C       INC $0C    [$00:000C]   A:31EC X:000E Y:00B1 P:eNvmXdizc
$80/BE02 0A          ASL A                   A:31EC X:000E Y:00B1 P:eNvmXdizc
$80/BE03 0A          ASL A                   A:63D8 X:000E Y:00B1 P:envmXdizc
$80/BE04 0A          ASL A                   A:C7B0 X:000E Y:00B1 P:eNvmXdizc
$80/BE05 BE 00 05    LDX $0500,y[$81:05B1]   A:8F60 X:000E Y:00B1 P:eNvmXdizC
$80/BE08 8E 80 21    STX $2180  [$81:2180]   A:8F60 X:000E Y:00B1 P:envmXdizC
$80/BE0B 86 08       STX $08    [$00:0008]   A:8F60 X:000E Y:00B1 P:envmXdizC

$80/BE0D 85 6C       STA $6C    [$00:006C]   A:B188 X:000C Y:0000 P:envmXdiZc
$80/BE0F A4 6D       LDY $6D    [$00:006D]   A:B188 X:000C Y:0000 P:envmXdiZc
$80/BE11 BE 00 06    LDX $0600,y[$81:06B1]   A:B188 X:000C Y:00B1 P:eNvmXdizc
$80/BE14 7C 17 BE    JMP ($BE17,x)[$80:BDF5] A:B188 X:000E Y:00B1 P:envmXdizc	; Can go to BD5A, BDA7

$80/BE17 54 BE A4 BE F5 BE 47 BF 0E BD 5A BD A7 BD F5 BD 30 BE 2B BE A2 0A 4C 7C C1

$80/BE30 A2 0A       LDX #$0A                A:F968 X:0010 Y:00F9 P:envmXdizc
$80/BE32 64 6A       STZ $6A    [$00:006A]   A:F968 X:000A Y:00F9 P:envmXdizc
$80/BE34 B2 0C       LDA ($0C)  [$97:E197]   A:F968 X:000A Y:00F9 P:envmXdizc
$80/BE36 29 FF 00    AND #$00FF              A:369B X:000A Y:00F9 P:envmXdizc
$80/BE39 0A          ASL A                   A:009B X:000A Y:00F9 P:envmXdizc
$80/BE3A 0A          ASL A                   A:0136 X:000A Y:00F9 P:envmXdizc
$80/BE3B 0A          ASL A                   A:026C X:000A Y:00F9 P:envmXdizc
$80/BE3C 05 6B       ORA $6B    [$00:006B]   A:04D8 X:000A Y:00F9 P:envmXdizc
$80/BE3E 85 6B       STA $6B    [$00:006B]   A:6CD8 X:000A Y:00F9 P:envmXdizc
$80/BE40 A5 6C       LDA $6C    [$00:006C]   A:6CD8 X:000A Y:00F9 P:envmXdizc
$80/BE42 6C 60 07    JMP ($0760)[$80:BFC8]   A:F96C X:000A Y:00F9 P:eNvmXdizc	; Can go to $80:BFC8

$80/BE45 0A          ASL A                   A:C530 X:000E Y:00C5 P:envmXdizc
$80/BE46 0A          ASL A                   A:8A60 X:000E Y:00C5 P:eNvmXdizC

$80/BE47 0A          ASL A                   A:14C0 X:000E Y:00C5 P:envmXdizC
$80/BE48 0A          ASL A                   A:2980 X:000E Y:00C5 P:envmXdizc
$80/BE49 E2 20       SEP #$20                A:5300 X:000E Y:00C5 P:envmXdizc
$80/BE4B B2 0C       LDA ($0C)  [$81:CE34]   A:5300 X:000E Y:00C5 P:envMXdizc
$80/BE4D C2 20       REP #$20                A:5314 X:000E Y:00C5 P:envMXdizc
$80/BE4F E6 0C       INC $0C    [$00:000C]   A:5314 X:000E Y:00C5 P:envmXdizc
$80/BE51 0A          ASL A                   A:5314 X:000E Y:00C5 P:eNvmXdizc
$80/BE52 0A          ASL A                   A:A628 X:000E Y:00C5 P:eNvmXdizc
$80/BE53 0A          ASL A                   A:4C50 X:000E Y:00C5 P:envmXdizC
$80/BE54 0A          ASL A                   A:98A0 X:000E Y:00C5 P:eNvmXdizc
$80/BE55 BE 00 05    LDX $0500,y[$81:05C5]   A:3140 X:000E Y:00C5 P:envmXdizC
$80/BE58 8E 80 21    STX $2180  [$81:2180]   A:3140 X:00EF Y:00C5 P:eNvmXdizC
$80/BE5B 86 08       STX $08    [$00:0008]   A:3140 X:00EF Y:00C5 P:eNvmXdizC

$80/BE5D 85 6C       STA $6C    [$00:006C]   A:3140 X:00EF Y:00C5 P:eNvmXdizC
$80/BE5F A4 6D       LDY $6D    [$00:006D]   A:3140 X:00EF Y:00C5 P:eNvmXdizC
$80/BE61 BE 00 06    LDX $0600,y[$81:0631]   A:3140 X:00EF Y:0031 P:envmXdizC
$80/BE64 7C 67 BE    JMP ($BE67,x)[$80:BEF6] A:3140 X:0002 Y:0031 P:envmXdizC	; Can go to BDF6

$80/BE67 A5 BE F6 BE 48 BF 0F BD 5B BD A8 BD F6 BD 45 BE 80 BE

$80/BE79 7B          TDC                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE7A BE A2 08    LDX $08A2,y[$97:08A2]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE7D 4C 7C C1    JMP $C17C  [$80:C17C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE80 A2 08       LDX #$08                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE82 64 6A       STZ $6A    [$00:006A]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE84 B2 0C       LDA ($0C)  [$97:E19C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE86 29 FF 00    AND #$00FF              A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE89 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE8A 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE8B 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE8C 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE8D 05 6B       ORA $6B    [$00:006B]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE8F 85 6B       STA $6B    [$00:006B]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE91 A5 6C       LDA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE93 6C 60 07    JMP ($0760)[$80:BFC8]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BE96 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE97 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE98 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE99 E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BE9B B2 0C       LDA ($0C)  [$97:E19C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BE9D C2 20       REP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BE9F E6 0C       INC $0C    [$00:000C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEA1 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEA2 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEA3 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEA4 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEA5 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEA6 BE 00 05    LDX $0500,y[$97:0500]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEA9 8E 80 21    STX $2180  [$97:2180]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEAC 86 08       STX $08    [$00:0008]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEAE 85 6C       STA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEB0 A4 6D       LDY $6D    [$00:006D]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEB2 BE 00 06    LDX $0600,y[$97:0600]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BEB5 7C B8 BE    JMP ($BEB8,x)[$80:BE46] A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BEB8 F7 BE 49 BF 10 BD 5C BD A9 BD F7 BD 46 BE 96 BE D1 BE

x$80/BECC A2 06       LDX #$06                A:A6C0 X:0012 Y:00A6 P:envmXdizc
x$80/BECE 4C 7C C1    JMP $C17C  [$80:C17C]   A:A6C0 X:0012 Y:00A6 P:envmXdizc

$80/BED1 A2 06       LDX #$06                A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BED3 64 6A       STZ $6A    [$00:006A]   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BED5 B2 0C       LDA ($0C)  [$81:CD9C]   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BED7 29 FF 00    AND #$00FF              A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEDA 0A          ASL A                   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEDB 0A          ASL A                   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEDC 0A          ASL A                   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEDD 0A          ASL A                   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEDE 0A          ASL A                   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEDF 05 6B       ORA $6B    [$00:006B]   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEE1 85 6B       STA $6B    [$00:006B]   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEE3 A5 6C       LDA $6C    [$00:006C]   A:A6C0 X:0012 Y:00A6 P:envmXdizc
$80/BEE5 6C 60 07    JMP ($0760)[$80:BFDD]   A:A6C0 X:0012 Y:00A6 P:envmXdizc

$80/BEE8 0A          ASL A                   A:C500 X:000E Y:00C5 P:envmXdizc
$80/BEE9 0A          ASL A                   A:8A00 X:000E Y:00C5 P:eNvmXdizC
$80/BEEA E2 20       SEP #$20                A:1400 X:000E Y:00C5 P:envmXdizC
$80/BEEC B2 0C       LDA ($0C)  [$81:CE35]   A:1400 X:000E Y:00C5 P:envMXdizC
$80/BEEE C2 20       REP #$20                A:14F6 X:000E Y:00C5 P:eNvMXdizC
$80/BEF0 E6 0C       INC $0C    [$00:000C]   A:14F6 X:000E Y:00C5 P:eNvmXdizC
$80/BEF2 0A          ASL A                   A:14F6 X:000E Y:00C5 P:eNvmXdizC
$80/BEF3 0A          ASL A                   A:29EC X:000E Y:00C5 P:envmXdizc
$80/BEF4 0A          ASL A                   A:53D8 X:000E Y:00C5 P:envmXdizc
$80/BEF5 0A          ASL A                   A:A7B0 X:000E Y:00C5 P:eNvmXdizc

$80/BEF6 0A          ASL A                   A:4F60 X:000E Y:00C5 P:envmXdizC
$80/BEF7 0A          ASL A                   A:9EC0 X:000E Y:00C5 P:eNvmXdizc
$80/BEF8 BE 00 05    LDX $0500,y[$81:05C5]   A:3D80 X:000E Y:00C5 P:envmXdizC
$80/BEFB 8E 80 21    STX $2180  [$81:2180]   A:3D80 X:00EF Y:00C5 P:eNvmXdizC	; Another write
$80/BEFE 86 08       STX $08    [$00:0008]   A:3D80 X:00EF Y:00C5 P:eNvmXdizC

$80/BF00 85 6C       STA $6C    [$00:006C]   A:3D80 X:00EF Y:00C5 P:eNvmXdizC
$80/BF02 A4 6D       LDY $6D    [$00:006D]   A:3D80 X:00EF Y:00C5 P:eNvmXdizC
$80/BF04 BE 00 06    LDX $0600,y[$81:063D]   A:3D80 X:00EF Y:003D P:envmXdizC
$80/BF07 7C 0A BF    JMP ($BF0A,x)[$80:BD11] A:3D80 X:0002 Y:003D P:envmXdizC	; Can go to BE47

$80/BF0A 4A BF 11 BD 5D BD AA BD F8 BD 47 BE 97 BE E8 BE 23 BF 1E BF A2 04 4C 7C C1 A2

$80/BF24 04 64       TSB $64    [$00:0064]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF26 6A          ROR A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF27 B2 0C       LDA ($0C)  [$97:E19C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF29 29 FF 00    AND #$00FF              A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF2C 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF2D 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF2E 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF2F 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF30 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF31 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF32 05 6B       ORA $6B    [$00:006B]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF34 85 6B       STA $6B    [$00:006B]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF36 A5 6C       LDA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF38 6C 60 07    JMP ($0760)[$80:BFC8]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BF3B 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF3C E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF3E B2 0C       LDA ($0C)  [$97:E19C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BF40 C2 20       REP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BF42 E6 0C       INC $0C    [$00:000C]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BF44 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF45 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF46 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF47 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF48 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF49 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF4A 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF4B BE 00 05    LDX $0500,y[$97:0500]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF4E 8E 80 21    STX $2180  [$97:2180]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF51 86 08       STX $08    [$00:0008]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF53 85 6C       STA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF55 A4 6D       LDY $6D    [$00:006D]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF57 BE 00 06    LDX $0600,y[$97:0600]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF5A 7C 5D BF    JMP ($BF5D,x)[$80:BEE9] A:8004 X:000C Y:0000 P:eNvmXdizc

$80/BF5D 12 BD 5E BD AB BD F9 BD 48 BE 98 BE E9 BE 3B BF 76 BF 71 BF 
$80/BF71 A2 02 4C 7C C1 A2 02 64

$80/BF79 6A          ROR A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF7A B2 0C       LDA ($0C)  [$97:E19C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF7C 29 FF 00    AND #$00FF              A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF7F 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF80 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF81 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF82 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF83 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF84 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF85 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF86 05 6B       ORA $6B    [$00:006B]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF88 85 6B       STA $6B    [$00:006B]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF8A A5 6C       LDA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/BF8C 6C 60 07    JMP ($0760)[$80:BFC8]   A:8004 X:000C Y:0000 P:eNvmXdizc

x$80/BF8F 4A          LSR A                   A:CBF1 X:000E Y:00CB P:eNvmXdizc
x$80/BF90 4A          LSR A                   A:65F8 X:000E Y:00CB P:envmXdizC
x$80/BF91 4A          LSR A                   A:32FC X:000E Y:00CB P:envmXdizc
x$80/BF92 4A          LSR A                   A:197E X:000E Y:00CB P:envmXdizc
x$80/BF93 4A          LSR A                   A:0CBF X:000E Y:00CB P:envmXdizc
x$80/BF94 4A          LSR A                   A:065F X:000E Y:00CB P:envmXdizC
x$80/BF95 4A          LSR A                   A:032F X:000E Y:00CB P:envmXdizC
x$80/BF96 38          SEC                     A:0197 X:000E Y:00CB P:envmXdizC
x$80/BF97 ED 30 07    SBC $0730  [$81:0730]   A:0197 X:000E Y:00CB P:envmXdizC
x$80/BF9A A8          TAY                     A:003E X:000E Y:00CB P:envmXdizC
x$80/BF9B E2 20       SEP #$20                A:003E X:000E Y:003E P:envmXdizC
x$80/BF9D B9 00 01    LDA $0100,y[$81:013E]   A:003E X:000E Y:003E P:envMXdizC
x$80/BFA0 A0 01       LDY #$01                A:0016 X:000E Y:003E P:envMXdizC
x$80/BFA2 C5 73       CMP $73    [$00:0073]   A:0016 X:000E Y:0001 P:envMXdizC
x$80/BFA4 F0 1C       BEQ $1C    [$BFC2]      A:0016 X:000E Y:0001 P:envMXdizc
x$80/BFA6 4C E8 C0    JMP $C0E8  [$80:C0E8]   A:0016 X:000E Y:0001 P:envMXdizc	goto WriteImpl

$80/BFA9 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFAA 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFAB 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFAC 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFAD 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFAE 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFAF 38          SEC                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFB0 ED 32 07    SBC $0732  [$97:0732]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFB3 A8          TAY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFB4 E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFB6 B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFB9 A0 02       LDY #$02                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFBB C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFBD F0 03       BEQ $03    [$BFC2]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFBF 4C E8 C0    JMP $C0E8  [$80:C0E8]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFC2 4C 5F C1    JMP $C15F  [$80:C15F]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFC5 4C 8F BF    JMP $BF8F  [$80:BF8F]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFC8 CD 50 07    CMP $0750  [$97:0750]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFCB 90 C2       BCC $C2    [$BF8F]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFCD 4C A9 BF    JMP $BFA9  [$80:BFA9]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFD0 CD 50 07    CMP $0750  [$97:0750]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFD3 90 BA       BCC $BA    [$BF8F]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFD5 CD 52 07    CMP $0752  [$97:0752]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFD8 90 CF       BCC $CF    [$BFA9]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFDA 4C 69 C0    JMP $C069  [$80:C069]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BFDD CD 50 07    CMP $0750  [$81:0750]   A:F68F X:0010 Y:00F6 P:eNvmXdizc
$80/BFE0 90 AD       BCC $AD    [$BF8F]      A:F68F X:0010 Y:00F6 P:envmXdizC
$80/BFE2 CD 52 07    CMP $0752  [$81:0752]   A:F68F X:0010 Y:00F6 P:envmXdizC
$80/BFE5 90 C2       BCC $C2    [$BFA9]      A:F68F X:0010 Y:00F6 P:eNvmXdizc
$80/BFA9 4A          LSR A                   A:F68F X:0010 Y:00F6 P:eNvmXdizc
$80/BFAA 4A          LSR A                   A:7B47 X:0010 Y:00F6 P:envmXdizC
$80/BFAB 4A          LSR A                   A:3DA3 X:0010 Y:00F6 P:envmXdizC
$80/BFAC 4A          LSR A                   A:1ED1 X:0010 Y:00F6 P:envmXdizC
$80/BFAD 4A          LSR A                   A:0F68 X:0010 Y:00F6 P:envmXdizC
$80/BFAE 4A          LSR A                   A:07B4 X:0010 Y:00F6 P:envmXdizc
$80/BFAF 38          SEC                     A:03DA X:0010 Y:00F6 P:envmXdizc
$80/BFB0 ED 32 07    SBC $0732  [$81:0732]   A:03DA X:0010 Y:00F6 P:envmXdizC
$80/BFB3 A8          TAY                     A:00B4 X:0010 Y:00F6 P:envmXdizC
$80/BFB4 E2 20       SEP #$20                A:00B4 X:0010 Y:00B4 P:eNvmXdizC
$80/BFB6 B9 00 01    LDA $0100,y[$81:01B4]   A:00B4 X:0010 Y:00B4 P:eNvMXdizC
$80/BFB9 A0 02       LDY #$02                A:00EB X:0010 Y:00B4 P:eNvMXdizC

$80/BFBB C5 73       CMP $73    [$00:0073]   A:00EB X:0010 Y:0002 P:envMXdizC
$80/BFBD F0 03       BEQ $03    [$BFC2]      A:00EB X:0010 Y:0002 P:envMXdizC

$80/BFBF 4C E8 C0    JMP $C0E8  [$80:C0E8]   A:00EB X:0010 Y:0002 P:envMXdizC

$80/BFC8 CD 50 07    CMP $0750  [$97:0750]   A:F96C X:000A Y:00F9 P:eNvmXdizc
$80/BFCB 90 C2       BCC $C2    [$BF8F]      A:F96C X:000A Y:00F9 P:eNvmXdizc

x$80/BFDD CD 50 07    CMP $0750  [$81:0750]   A:E358 X:000C Y:00E3 P:eNvmXdizc
x$80/BFE0 90 AD       BCC $AD    [$BF8F]      A:E358 X:000C Y:00E3 P:eNvmXdizc

$80/BFE2 CD 52 07    CMP $0752  [$97:0752]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFE5 90 C2       BCC $C2    [$BFA9]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BFE7 CD 54 07    CMP $0754  [$97:0754]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFEA 90 7D       BCC $7D    [$C069]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BFEC 4C 80 C0    JMP $C080  [$80:C080]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BFEF CD 50 07    CMP $0750  [$97:0750]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFF2 90 9B       BCC $9B    [$BF8F]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BFF4 CD 52 07    CMP $0752  [$97:0752]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFF7 90 B0       BCC $B0    [$BFA9]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BFF9 CD 54 07    CMP $0754  [$97:0754]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/BFFC 90 6B       BCC $6B    [$C069]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/BFFE CD 56 07    CMP $0756  [$97:0756]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C001 90 7D       BCC $7D    [$C080]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C003 4C 9C C0    JMP $C09C  [$80:C09C]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C006 CD 50 07    CMP $0750  [$97:0750]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C009 90 84       BCC $84    [$BF8F]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C00B CD 52 07    CMP $0752  [$97:0752]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C00E 90 99       BCC $99    [$BFA9]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C010 CD 54 07    CMP $0754  [$97:0754]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C013 90 54       BCC $54    [$C069]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C015 CD 56 07    CMP $0756  [$97:0756]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C018 90 66       BCC $66    [$C080]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C01A CD 58 07    CMP $0758  [$97:0758]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C01D 90 7D       BCC $7D    [$C09C]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C01F 4C B1 C0    JMP $C0B1  [$80:C0B1]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C022 CD 50 07    CMP $0750  [$97:0750]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C025 90 9E       BCC $9E    [$BFC5]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C027 CD 52 07    CMP $0752  [$97:0752]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C02A 90 A1       BCC $A1    [$BFCD]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C02C CD 54 07    CMP $0754  [$97:0754]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C02F 90 38       BCC $38    [$C069]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C031 CD 56 07    CMP $0756  [$97:0756]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C034 90 4A       BCC $4A    [$C080]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C036 CD 58 07    CMP $0758  [$97:0758]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C039 90 61       BCC $61    [$C09C]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C03B CD 5A 07    CMP $075A  [$97:075A]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C03E 90 71       BCC $71    [$C0B1]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C040 4C C5 C0    JMP $C0C5  [$80:C0C5]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C043 CD 50 07    CMP $0750  [$97:0750]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C046 90 51       BCC $51    [$C099]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C048 CD 52 07    CMP $0752  [$97:0752]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C04B 90 80       BCC $80    [$BFCD]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C04D CD 54 07    CMP $0754  [$97:0754]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C050 90 17       BCC $17    [$C069]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C052 CD 56 07    CMP $0756  [$97:0756]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C055 90 29       BCC $29    [$C080]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C057 CD 58 07    CMP $0758  [$97:0758]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C05A 90 40       BCC $40    [$C09C]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C05C CD 5A 07    CMP $075A  [$97:075A]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C05F 90 50       BCC $50    [$C0B1]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C061 CD 5C 07    CMP $075C  [$97:075C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C064 90 5F       BCC $5F    [$C0C5]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C066 4C D8 C0    JMP $C0D8  [$80:C0D8]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C069 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C06A 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C06B 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C06C 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C06D 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C06E 38          SEC                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C06F ED 34 07    SBC $0734  [$97:0734]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C072 A8          TAY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C073 E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C075 B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C078 A0 03       LDY #$03                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C07A C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C07C F0 18       BEQ $18    [$C096]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C07E 80 68       BRA $68    [$C0E8]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C080 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C081 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C082 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C083 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C084 38          SEC                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C085 ED 36 07    SBC $0736  [$97:0736]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C088 A8          TAY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C089 E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C08B B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C08E A0 04       LDY #$04                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C090 C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C092 F0 02       BEQ $02    [$C096]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C094 80 52       BRA $52    [$C0E8]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C096 4C 5F C1    JMP $C15F  [$80:C15F]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C099 4C 8F BF    JMP $BF8F  [$80:BF8F]   A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C09C 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C09D 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C09E 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C09F 38          SEC                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0A0 ED 38 07    SBC $0738  [$97:0738]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0A3 A8          TAY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0A4 E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0A6 B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0A9 A0 05       LDY #$05                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0AB C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0AD F0 E7       BEQ $E7    [$C096]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0AF 80 37       BRA $37    [$C0E8]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C0B1 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0B2 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0B3 38          SEC                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0B4 ED 3A 07    SBC $073A  [$97:073A]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0B7 A8          TAY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0B8 E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0BA B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0BD A0 06       LDY #$06                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0BF C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0C1 F0 D3       BEQ $D3    [$C096]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0C3 80 23       BRA $23    [$C0E8]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C0C5 4A          LSR A                   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0C6 38          SEC                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0C7 ED 3C 07    SBC $073C  [$97:073C]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0CA A8          TAY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0CB E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0CD B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0D0 A0 07       LDY #$07                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0D2 C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0D4 F0 C0       BEQ $C0    [$C096]      A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0D6 80 10       BRA $10    [$C0E8]      A:8004 X:000C Y:0000 P:eNvMXdizc

$80/C0D8 38          SEC                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0D9 ED 3E 07    SBC $073E  [$97:073E]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0DC A8          TAY                     A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0DD E2 20       SEP #$20                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0DF B9 00 01    LDA $0100,y[$97:0100]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0E2 A0 08       LDY #$08                A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0E4 C5 73       CMP $73    [$00:0073]   A:8004 X:000C Y:0000 P:eNvMXdizc
$80/C0E6 F0 77       BEQ $77    [$C15F]      A:8004 X:000C Y:0000 P:eNvMXdizc

WriteImpl:
x$80/C0E8 8D 80 21    STA $2180  [$81:2180]   A:00EB X:0010 Y:0002 P:envMXdizC	; WRAM data register. 
	
x$80/C0EB 85 08       STA $08    [$00:0008]   A:00EB X:0010 Y:0002 P:envMXdizC
x$80/C0ED C2 20       REP #$20                A:00EB X:0010 Y:0002 P:envMXdizC
x$80/C0EF E6 0C       INC $0C    [$00:000C]   A:00EB X:0010 Y:0002 P:envmXdizC
x$80/C0F1 A5 6B       LDA $6B    [$00:006B]   A:00EB X:0010 Y:0002 P:eNvmXdizC
x$80/C0F3 7C F4 C0    JMP ($C0F4,x)[$80:C106] A:8C53 X:0010 Y:0002 P:eNvmXdizC	; Can go to 80C122, 80C10E

// The above jump should be indexed off of F6 not F4, shouldn't it?
$80/C0F6 22 C1 1E C1 1A C1 16 C1 12 C1 0E C1 0A C1 06 C1 0A

$80/C106 0A          ASL A                   A:8C53 X:0010 Y:0002 P:eNvmXdizC
$80/C107 88          DEY                     A:18A6 X:0010 Y:0002 P:envmXdizC
$80/C108 F0 4F       BEQ $4F    [$C159]      A:18A6 X:0010 Y:0001 P:envmXdizC
$80/C10A 0A          ASL A                   A:18A6 X:0010 Y:0001 P:envmXdizC
$80/C10B 88          DEY                     A:314C X:0010 Y:0001 P:envmXdizc
$80/C10C F0 48       BEQ $48    [$C156]      A:314C X:0010 Y:0000 P:envmXdiZc

$80/C10E 0A          ASL A                   A:58C4 X:000C Y:0001 P:envmXdizC
$80/C10F 88          DEY                     A:B188 X:000C Y:0001 P:eNvmXdizc
$80/C110 F0 41       BEQ $41    [$C153]      A:B188 X:000C Y:0000 P:envmXdiZc

$80/C112 0A          ASL A                   A:6CD8 X:000A Y:0001 P:envmXdizC
$80/C113 88          DEY                     A:D9B0 X:000A Y:0001 P:eNvmXdizc
$80/C114 F0 3A       BEQ $3A    [$C150]      A:D9B0 X:000A Y:0000 P:envmXdiZc

$80/C116 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C117 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C118 F0 33       BEQ $33    [$C14D]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C11A 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C11B 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C11C F0 2C       BEQ $2C    [$C14A]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C11E 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C11F 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C120 F0 25       BEQ $25    [$C147]      A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C122 0A          ASL A                   A:B280 X:0002 Y:0002 P:eNvmXdizc
$80/C123 E2 20       SEP #$20                A:6500 X:0002 Y:0002 P:envmXdizC
$80/C125 B2 0C       LDA ($0C)  [$81:D586]   A:6500 X:0002 Y:0002 P:envMXdizC
$80/C127 C2 20       REP #$20                A:65F8 X:0002 Y:0002 P:eNvMXdizC
$80/C129 E6 0C       INC $0C    [$00:000C]   A:65F8 X:0002 Y:0002 P:eNvmXdizC
$80/C12B 88          DEY                     A:65F8 X:0002 Y:0002 P:eNvmXdizC
$80/C12C F0 2E       BEQ $2E    [$C15C]      A:65F8 X:0002 Y:0001 P:envmXdizC
$80/C12E 0A          ASL A                   A:65F8 X:0002 Y:0001 P:envmXdizC
$80/C12F 88          DEY                     A:CBF0 X:0002 Y:0001 P:eNvmXdizc
$80/C130 F0 27       BEQ $27    [$C159]      A:CBF0 X:0002 Y:0000 P:envmXdiZc

$80/C132 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C133 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C134 F0 20       BEQ $20    [$C156]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C136 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C137 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C138 F0 19       BEQ $19    [$C153]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C13A 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C13B 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C13C F0 12       BEQ $12    [$C150]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C13E 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C13F 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C140 F0 0B       BEQ $0B    [$C14D]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C142 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C143 88          DEY                     A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C144 F0 04       BEQ $04    [$C14A]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C146 0A          ASL A                   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C147 4C 53 BF    JMP $BF53  [$80:BF53]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C14A 4C 00 BF    JMP $BF00  [$80:BF00]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C14D 4C AE BE    JMP $BEAE  [$80:BEAE]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C150 4C 5D BE    JMP $BE5D  [$80:BE5D]   A:D9B0 X:000A Y:0000 P:envmXdiZc

$80/C153 4C 0D BE    JMP $BE0D  [$80:BE0D]   A:B188 X:000C Y:0000 P:envmXdiZc

$80/C156 4C BE BD    JMP $BDBE  [$80:BDBE]   A:314C X:0010 Y:0000 P:envmXdiZc

$80/C159 4C 70 BD    JMP $BD70  [$80:BD70]   A:CBF0 X:0002 Y:0000 P:envmXdiZc

$80/C15C 4C 23 BD    JMP $BD23  [$80:BD23]   A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C15F C2 20       REP #$20                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C161 E6 0C       INC $0C    [$00:000C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C163 A5 6B       LDA $6B    [$00:006B]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C165 20 DC C2    JSR $C2DC  [$80:C2DC]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C168 85 6C       STA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C16A 20 32 C2    JSR $C232  [$80:C232]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C16D F0 26       BEQ $26    [$C195]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C16F A4 08       LDY $08    [$00:0008]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C171 8C 80 21    STY $2180  [$97:2180]   A:8004 X:000C Y:0000 P:eNvmXdizc	; Transfer

$80/C174 3A          DEC A                   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C175 D0 FA       BNE $FA    [$C171]      A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C177 A5 6C       LDA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C179 7C F9 BC    JMP ($BCF9,x)[$80:BDBE] A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C17C A4 74       LDY $74    [$00:0074]   A:A655 X:0010 Y:00A6 P:envmXdizc
$80/C17E 20 DC C2    JSR $C2DC  [$80:C2DC]   A:A655 X:0010 Y:0007 P:envmXdizc
$80/C181 85 6C       STA $6C    [$00:006C]   A:2A80 X:0002 Y:0000 P:envmXdiZC
$80/C183 20 32 C2    JSR $C232  [$80:C232]   A:2A80 X:0002 Y:0000 P:envmXdiZC	; Function sets Acc = NumberOfValues. For writing a series of like values
										; Function sets $08 = What value to write
										; Function sets Z flag
$80/C186 F0 0D       BEQ $0D    [$C195]      A:0011 X:0004 Y:0004 P:envmXdizc
$80/C188 A4 08       LDY $08    [$00:0008]   A:0011 X:0004 Y:0004 P:envmXdizc

WriteSeries:
$80/C18A 8C 80 21    STY $2180  [$81:2180]   A:0011 X:0004 Y:0000 P:envmXdiZc
$80/C18D 3A          DEC A                   A:0011 X:0004 Y:0000 P:envmXdiZc
$80/C18E D0 FA       BNE $FA    [$C18A]      A:0010 X:0004 Y:0000 P:envmXdizc

$80/C190 A5 6C       LDA $6C    [$00:006C]   A:0000 X:0004 Y:0000 P:envmXdiZc
$80/C192 7C F9 BC    JMP ($BCF9,x)[$80:BF00] A:6C40 X:0004 Y:0000 P:envmXdizc	; Can go to BE5D

$80/C195 A5 6C       LDA $6C    [$00:006C]   A:0000 X:000C Y:0000 P:envmXdiZc	; If $6C < 0, time to exit
$80/C197 30 16       BMI $16    [$C1AF]      A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C199 A0 01       LDY #$01                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C19B 20 DC C2    JSR $C2DC  [$80:C2DC]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C19E 85 6C       STA $6C    [$00:006C]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C1A0 A4 6D       LDY $6D    [$00:006D]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C1A2 8C 80 21    STY $2180  [$97:2180]   A:8004 X:000C Y:0000 P:eNvmXdizc	; Transfer

$80/C1A5 84 08       STY $08    [$00:0008]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C1A7 A0 08       LDY #$08                A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C1A9 20 DC C2    JSR $C2DC  [$80:C2DC]   A:8004 X:000C Y:0000 P:eNvmXdizc
$80/C1AC 7C F9 BC    JMP ($BCF9,x)[$80:BDBE] A:8004 X:000C Y:0000 P:eNvmXdizc

$80/C1AF 60          RTS                     A:8004 X:000C Y:0000 P:eNvmXdizc

///////////////////////////////////////////////////////////////////////////////////////////////

// Interrupt handler

*** NMI
$00/8147 5C 4B 81 80 JMP $80814B[$80:814B]   A:009C X:0628 Y:0005 P:eNvmxdIzc
$80/814B C2 30       REP #$30                A:009C X:0628 Y:0005 P:eNvmxdIzc
$80/814D 8B          PHB                     A:009C X:0628 Y:0005 P:eNvmxdIzc
$80/814E 4B          PHK                     A:009C X:0628 Y:0005 P:eNvmxdIzc
$80/814F AB          PLB                     A:009C X:0628 Y:0005 P:eNvmxdIzc
$80/8150 48          PHA                     A:009C X:0628 Y:0005 P:eNvmxdIzc
$80/8151 58          CLI                     A:009C X:0628 Y:0005 P:eNvmxdIzc
$80/8152 A9 00 00    LDA #$0000              A:009C X:0628 Y:0005 P:eNvmxdizc
$80/8155 5B          TCD                     A:0000 X:0628 Y:0005 P:envmxdiZc
$80/8156 AD 10 42    LDA $4210  [$80:4210]   A:0000 X:0628 Y:0005 P:envmxdiZc
$80/8159 A5 38       LDA $38    [$00:0038]   A:42C2 X:0628 Y:0005 P:envmxdizc
$80/815B F0 03       BEQ $03    [$8160]      A:0000 X:0628 Y:0005 P:envmxdiZc
$80/8160 DA          PHX                     A:0000 X:0628 Y:0005 P:envmxdiZc
$80/8161 5A          PHY                     A:0000 X:0628 Y:0005 P:envmxdiZc
$80/8162 A9 D3 16    LDA #$16D3              A:0000 X:0628 Y:0005 P:envmxdiZc
$80/8165 85 3E       STA $3E    [$00:003E]   A:16D3 X:0628 Y:0005 P:envmxdizc
$80/8167 E2 10       SEP #$10                A:16D3 X:0628 Y:0005 P:envmxdizc
$80/8169 AE 74 07    LDX $0774  [$80:0774]   A:16D3 X:0028 Y:0005 P:envmXdizc
$80/816C F0 0F       BEQ $0F    [$817D]      A:16D3 X:0000 Y:0005 P:envmXdiZc
$80/817D A2 80       LDX #$80                A:16D3 X:0000 Y:0005 P:envmXdiZc
$80/817F DA          PHX                     A:16D3 X:0080 Y:0005 P:eNvmXdizc
$80/8180 F4 85 81    PEA $8185               A:16D3 X:0080 Y:0005 P:eNvmXdizc	; Setup where to go next after the interrupt ig
$80/8183 DC 8C 07    JML [$078C][$80:8019]   A:16D3 X:0080 Y:0005 P:eNvmXdizc	; Jump to a ret
$80/8019 6B          RTL                     A:16D3 X:0080 Y:0005 P:eNvmXdizc

///////////////////////////////////////////////////////////////////////////////////////////////

// void AfterInterrupt()
// Looks like this loops until $3A == $3C.
// In general, $3C goes up to 03F8.
// It's some kind of timer.

$80/8186 C2 30       REP #$30                A:16D3 X:0080 Y:0007 P:eNvmXdizC	
$80/8188 AD D2 07    LDA $07D2  [$80:07D2]   A:16D3 X:0080 Y:0007 P:eNvmxdizC
$80/818B 8D 09 42    STA $4209  [$80:4209]   A:0046 X:0080 Y:0007 P:envmxdizC
$80/818E AD D4 07    LDA $07D4  [$80:07D4]   A:0046 X:0080 Y:0007 P:envmxdizC
$80/8191 8D 07 42    STA $4207  [$80:4207]   A:0000 X:0080 Y:0007 P:envmxdiZC
$80/8194 E2 30       SEP #$30                A:0000 X:0080 Y:0007 P:envmxdiZC
$80/8196 AD 62 07    LDA $0762  [$80:0762]   A:0000 X:0080 Y:0007 P:envMXdiZC
$80/8199 8D 00 21    STA $2100  [$80:2100]   A:008F X:0080 Y:0007 P:eNvMXdizC
$80/819C A2 00       LDX #$00                A:008F X:0080 Y:0007 P:eNvMXdizC
$80/819E 9B          TXY                     A:008F X:0000 Y:0007 P:envMXdiZC

Loop:
$80/819F 8A          TXA                     A:008F X:0000 Y:0000 P:envMXdiZC
$80/81A0 BD D6 07    LDA $07D6,x[$80:07D6]   A:0000 X:0000 Y:0000 P:envMXdiZC
$80/81A3 99 0D 21    STA $210D,y[$80:210D]   A:0000 X:0000 Y:0000 P:envMXdiZC
$80/81A6 BD D7 07    LDA $07D7,x[$80:07D7]   A:0000 X:0000 Y:0000 P:envMXdiZC
$80/81A9 99 0D 21    STA $210D,y[$80:210D]   A:0000 X:0000 Y:0000 P:envMXdiZC
$80/81AC C8          INY                     A:0000 X:0000 Y:0000 P:envMXdiZC
$80/81AD E8          INX                     A:0000 X:0000 Y:0001 P:envMXdizC
$80/81AE E8          INX                     A:0000 X:0001 Y:0001 P:envMXdizC
$80/81AF E0 0C       CPX #$0C                A:0000 X:0002 Y:0001 P:envMXdizC
$80/81B1 D0 EC       BNE $EC    [$819F]      A:0000 X:0002 Y:0001 P:eNvMXdizc

$80/81B3 C2 20       REP #$20                A:00FF X:000C Y:0006 P:envMXdiZC
$80/81B5 AD 68 07    LDA $0768  [$80:0768]   A:00FF X:000C Y:0006 P:envmXdiZC
$80/81B8 F0 31       BEQ $31    [$81EB]      A:0000 X:000C Y:0006 P:envmXdiZC
$80/81EB E2 20       SEP #$20                A:0000 X:000C Y:0006 P:envmXdiZC
$80/81ED C2 10       REP #$10                A:0000 X:000C Y:0006 P:envMXdiZC
$80/81EF A6 3A       LDX $3A    [$00:003A]   A:0000 X:000C Y:0006 P:envMxdiZC	// Load unblock number from $3A

CompareThen_JumpTo_WriteXTo3A:
$80/81F1 E4 3C       CPX $3C    [$00:003C]   A:0000 X:0140 Y:0006 P:envMxdizC
$80/81F3 F0 48       BEQ $48    [$823D]      A:0000 X:0140 Y:0006 P:envMxdiZC	// goto JumpTo_WriteXTo3A

$80/81F8 30 49       BMI $49    [$8243]      A:0101 X:0158 Y:0006 P:envMxdizc
$80/81FA 8D 00 43    STA $4300  [$80:4300]   A:0101 X:0158 Y:0006 P:envMxdizc
$80/81FD A9 18       LDA #$18                A:0101 X:0158 Y:0006 P:envMxdizc
$80/81FF 8D 01 43    STA $4301  [$80:4301]   A:0118 X:0158 Y:0006 P:envMxdizc
$80/8202 BD 01 01    LDA $0101,x[$80:0259]   A:0118 X:0158 Y:0006 P:envMxdizc
$80/8205 8D 02 43    STA $4302  [$80:4302]   A:0135 X:0158 Y:0006 P:envMxdizc
$80/8208 C2 20       REP #$20                A:0135 X:0158 Y:0006 P:envMxdizc
$80/820A A5 3E       LDA $3E    [$00:003E]   A:0135 X:0158 Y:0006 P:envmxdizc
$80/820C 38          SEC                     A:1061 X:0158 Y:0006 P:envmxdizc
$80/820D E9 66 00    SBC #$0066              A:1061 X:0158 Y:0006 P:envmxdizC
$80/8210 90 6A       BCC $6A    [$827C]      A:0FFB X:0158 Y:0006 P:envmxdizC
$80/8212 FD 04 01    SBC $0104,x[$80:025C]   A:0FFB X:0158 Y:0006 P:envmxdizC
$80/8215 90 65       BCC $65    [$827C]      A:0E3B X:0158 Y:0006 P:envmxdizC
$80/8217 85 3E       STA $3E    [$00:003E]   A:0E3B X:0158 Y:0006 P:envmxdizC
$80/8219 BD 02 01    LDA $0102,x[$80:025A]   A:0E3B X:0158 Y:0006 P:envmxdizC
$80/821C 8D 03 43    STA $4303  [$80:4303]   A:7FA8 X:0158 Y:0006 P:envmxdizC
$80/821F BD 04 01    LDA $0104,x[$80:025C]   A:7FA8 X:0158 Y:0006 P:envmxdizC
$80/8222 8D 05 43    STA $4305  [$80:4305]   A:01C0 X:0158 Y:0006 P:envmxdizC
$80/8225 BD 06 01    LDA $0106,x[$80:025E]   A:01C0 X:0158 Y:0006 P:envmxdizC
$80/8228 8D 16 21    STA $2116  [$80:2116]   A:6300 X:0158 Y:0006 P:envmxdizC

$80/822B 8A          TXA                     A:6300 X:0158 Y:0006 P:envmxdizC	// X+=8
$80/822C 18          CLC                     A:0158 X:0158 Y:0006 P:envmxdizC
$80/822D 69 08 00    ADC #$0008              A:0158 X:0158 Y:0006 P:envmxdizc
$80/8230 29 FF 03    AND #$03FF              A:0160 X:0158 Y:0006 P:envmxdizc
$80/8233 AA          TAX                     A:0160 X:0158 Y:0006 P:envmxdizc

$80/8234 E2 20       SEP #$20                A:0160 X:0160 Y:0006 P:envmxdizc
$80/8236 A9 01       LDA #$01                A:0160 X:0160 Y:0006 P:envMxdizc
$80/8238 8D 0B 42    STA $420B  [$80:420B]   A:0101 X:0160 Y:0006 P:envMxdizc
$80/823B 80 B4       BRA $B4    [$81F1]      A:0101 X:0160 Y:0006 P:envMxdizc	// goto CompareThen_JumpTo_WriteXTo3A

JumpTo_WriteXTo3A:
$80/823D 4C A5 83    JMP $83A5  [$80:83A5]   A:0000 X:0140 Y:0006 P:envMxdiZC	// Jump to unblock

WriteXTo3A:
$80/83A5 86 3A       STX $3A    [$00:003A]   A:0201 X:0228 Y:0006 P:envMxdiZC	// Unblocks DecompressActual1_WaitUntilReady
$80/83A7 C2 20       REP #$20                A:0201 X:0228 Y:0006 P:envMxdiZC
$80/83A9 A5 3E       LDA $3E    [$00:003E]   A:0201 X:0228 Y:0006 P:envmxdiZC
$80/83AB C9 6E 02    CMP #$026E              A:0E6D X:0228 Y:0006 P:envmxdizC
$80/83AE 90 2D       BCC $2D    [$83DD]      A:0E6D X:0228 Y:0006 P:envmxdizC
$80/83B0 E2 20       SEP #$20                A:0E6D X:0228 Y:0006 P:envmxdizC
$80/83B2 AD 66 07    LDA $0766  [$80:0766]   A:0E6D X:0228 Y:0006 P:envMxdizC
$80/83B5 F0 26       BEQ $26    [$83DD]      A:0E00 X:0228 Y:0006 P:envMxdiZC

$80/83DD C2 30       REP #$30                A:0E00 X:0228 Y:0006 P:envMxdiZC
$80/83DF AF D8 35 7E LDA $7E35D8[$7E:35D8]   A:0E00 X:0228 Y:0006 P:envmxdiZC
$80/83E3 18          CLC                     A:0000 X:0228 Y:0006 P:envmxdiZC
$80/83E4 6F DA 35 7E ADC $7E35DA[$7E:35DA]   A:0000 X:0228 Y:0006 P:envmxdiZc
$80/83E8 8F D8 35 7E STA $7E35D8[$7E:35D8]   A:0000 X:0228 Y:0006 P:envmxdiZc
$80/83EC A5 83       LDA $83    [$00:0083]   A:0000 X:0228 Y:0006 P:envmxdiZc
$80/83EE 69 01 00    ADC #$0001              A:07FF X:0228 Y:0006 P:envmxdizc
$80/83F1 85 83       STA $83    [$00:0083]   A:0800 X:0228 Y:0006 P:envmxdizc
$80/83F3 90 02       BCC $02    [$83F7]      A:0800 X:0228 Y:0006 P:envmxdizc
$80/83F7 AD D1 0A    LDA $0AD1  [$80:0AD1]   A:0800 X:0228 Y:0006 P:envmxdizc
$80/83FA F0 42       BEQ $42    [$843E]      A:0004 X:0228 Y:0006 P:envmxdizc
$80/83FC 30 21       BMI $21    [$841F]      A:0004 X:0228 Y:0006 P:envmxdizc
$80/83FE C9 01 00    CMP #$0001              A:0004 X:0228 Y:0006 P:envmxdizc
$80/8401 D0 12       BNE $12    [$8415]      A:0004 X:0228 Y:0006 P:envmxdizC

$80/8415 CE D1 0A    DEC $0AD1  [$80:0AD1]   A:0004 X:0228 Y:0006 P:envmxdizC
$80/8418 80 24       BRA $24    [$843E]      A:0004 X:0228 Y:0006 P:envmxdizC

$80/843E C2 20       REP #$20                A:0004 X:0228 Y:0006 P:envmxdizC
$80/8440 AD 87 07    LDA $0787  [$80:0787]   A:0004 X:0228 Y:0006 P:envmxdizC
$80/8443 D0 03       BNE $03    [$8448]      A:0000 X:0228 Y:0006 P:envmxdiZC
$80/8445 EE 64 07    INC $0764  [$80:0764]   A:0000 X:0228 Y:0006 P:envmxdiZC
$80/8448 E2 10       SEP #$10                A:0000 X:0228 Y:0006 P:envmxdizC
$80/844A C2 20       REP #$20                A:0000 X:0028 Y:0006 P:envmXdizC
$80/844C AE 71 07    LDX $0771  [$80:0771]   A:0000 X:0028 Y:0006 P:envmXdizC
$80/844F F0 0F       BEQ $0F    [$8460]      A:0000 X:0000 Y:0006 P:envmXdiZC

$80/8460 A2 80       LDX #$80                A:0000 X:0000 Y:0006 P:envmXdiZC
$80/8462 DA          PHX                     A:0000 X:0080 Y:0006 P:eNvmXdizC
$80/8463 F4 68 84    PEA $8468               A:0000 X:0080 Y:0006 P:eNvmXdizC
$80/8466 DC 89 07    JML [$0789][$9C:9971]   A:0000 X:0080 Y:0006 P:eNvmXdizC

// Jump location is dynamically decided. It's capable of returning up to $80/8469.

///////////////////////////////////////////////////////////////////////////////////////////////

// Code for incrementing (and resetting!) the counter in $3C.
// The counter goes from 0 up to 0x400, counting by 8.

$80/B4EB 8A          TXA                     A:6200 X:03F8 Y:0038 P:envmxdizc
$80/B4EC 18          CLC                     A:03F8 X:03F8 Y:0038 P:envmxdizc
$80/B4ED 69 08 00    ADC #$0008              A:03F8 X:03F8 Y:0038 P:envmxdizc	; Increment the counter
$80/B4F0 29 FF 03    AND #$03FF              A:0400 X:03F8 Y:0038 P:envmxdizc	; Mask. This way, if the counter gets to 0x400, it gets wrapped around to 0.
$80/B4F3 AA          TAX                     A:0000 X:03F8 Y:0038 P:envmxdiZc
$80/B4F4 A5 04       LDA $04    [$00:0004]   A:0000 X:0000 Y:0038 P:envmxdiZc
$80/B4F6 9D 00 01    STA $0100,x[$7E:0100]   A:3501 X:0000 Y:0038 P:envmxdizc
$80/B4F9 A5 06       LDA $06    [$00:0006]   A:3501 X:0000 Y:0038 P:envmxdizc
$80/B4FB 9D 02 01    STA $0102,x[$7E:0102]   A:7FA8 X:0000 Y:0038 P:envmxdizc
$80/B4FE AD CA 07    LDA $07CA  [$7E:07CA]   A:7FA8 X:0000 Y:0038 P:envmxdizc
$80/B501 9D 04 01    STA $0104,x[$7E:0104]   A:01C0 X:0000 Y:0038 P:envmxdizc
$80/B504 D0 0B       BNE $0B    [$B511]      A:01C0 X:0000 Y:0038 P:envmxdizc
$80/B511 A5 0A       LDA $0A    [$00:000A]   A:01C0 X:0000 Y:0038 P:envmxdizc
$80/B513 09 00 01    ORA #$0100              A:6200 X:0000 Y:0038 P:envmxdizc
$80/B516 9D 06 01    STA $0106,x[$7E:0106]   A:6300 X:0000 Y:0038 P:envmxdizc
$80/B519 8A          TXA                     A:6300 X:0000 Y:0038 P:envmxdizc
$80/B51A 18          CLC                     A:0000 X:0000 Y:0038 P:envmxdiZc
$80/B51B 69 08 00    ADC #$0008              A:0000 X:0000 Y:0038 P:envmxdiZc
$80/B51E 29 FF 03    AND #$03FF              A:0008 X:0000 Y:0038 P:envmxdizc
$80/B521 85 3C       STA $3C    [$00:003C]   A:0008 X:0000 Y:0038 P:envmxdizc
$80/B523 AB          PLB                     A:0008 X:0000 Y:0038 P:envmxdizc
$80/B524 7A          PLY                     A:0008 X:0000 Y:0038 P:envmxdizc
$80/B525 FA          PLX                     A:0008 X:0000 Y:0000 P:envmxdiZc
$80/B526 AD BA 07    LDA $07BA  [$7E:07BA]   A:0008 X:0038 Y:0000 P:envmxdizc
$80/B529 3A          DEC A                   A:001D X:0038 Y:0000 P:envmxdizc
$80/B52A 0A          ASL A                   A:001C X:0038 Y:0000 P:envmxdizc
$80/B52B 6B          RTL                     A:0038 X:0038 Y:0000 P:envmxdizc

///////////////////////////////////////////////////////////////////////////////////////////////
void SpinUntilCond()	808583
$80/8583 48          PHA                     A:000B X:0000 Y:000E P:envmxdizc
$80/8584 AD 64 07    LDA $0764  [$9F:0764]   A:000B X:0000 Y:000E P:envmxdizc

Spin:
$80/8587 CD 64 07    CMP $0764  [$9F:0764]   A:0172 X:0000 Y:000E P:envmxdizc
$80/858A F0 FB       BEQ $FB    [$8587]      A:0172 X:0000 Y:000E P:envmxdiZC	; goto Spin
$80/858C 68          PLA                     A:0172 X:0000 Y:000E P:eNvmxdizc
$80/858D 6B          RTL                     A:000B X:0000 Y:000E P:envmxdizc

///////////////////////////////////////////////////////////////////////////////////////////////

void DecompressActual1_WaitUntilReady() 80859A
// Makes sure $3A and $3C are equal.
// Maybe for vblank, or waiting until DMA channel is open.

$80/859A 48          PHA                     A:007F X:4000 Y:0010 P:envmxdizC
Loop:
$80/859B A5 3A       LDA $3A    [$00:003A]   A:007F X:4000 Y:0010 P:envmxdizC
$80/859D C5 3C       CMP $3C    [$00:003C]   A:0000 X:4000 Y:0010 P:envmxdiZC
$80/859F D0 FA       BNE $FA    [$859B]      A:0000 X:4000 Y:0010 P:envmxdiZC
$80/85A1 68          PLA                     A:0000 X:4000 Y:0010 P:envmxdiZC
$80/85A2 6B          RTL                     A:007F X:4000 Y:0010 P:envmxdizC

///////////////////////////////////////////////////////////////////////////////////////////////

// void UnblockL2()
...
$9D/DF61 80 04       BRA $04    [$DF67]      A:000B X:0000 Y:000E P:envmxdizc
$9D/DF63 22 83 85 80 JSL $808583[$80:8583]   A:000B X:0000 Y:000E P:envmxdizc	// call SpinUntilCond()
$9D/DF67 22 38 D9 9D JSL $9DD938[$9D:D938]   A:000B X:0000 Y:000E P:envmxdizc
$9D/DF6B 22 62 D9 9D JSL $9DD962[$9D:D962]   A:01A0 X:0300 Y:01A0 P:envmxdizc
$9D/DF6F 68          PLA                     A:05A0 X:0300 Y:05A0 P:envmxdizc
$9D/DF70 85 91       STA $91    [$00:0091]   A:0002 X:0300 Y:05A0 P:envmxdizc
$9D/DF72 6B          RTL                     A:0002 X:0300 Y:05A0 P:envmxdizc

///////////////////////////////////////////////////////////////////////////////////////////////

// void UnblockL3()

// call void UnblockL2()
$9D/DDAF 68          PLA                     A:0002 X:0300 Y:05A0 P:envmxdizc
$9D/DDB0 85 91       STA $91    [$00:0091]   A:FFFF X:0300 Y:05A0 P:eNvmxdizc
$9D/DDB2 6B          RTL                     A:FFFF X:0300 Y:05A0 P:eNvmxdizc

///////////////////////////////////////////////////////////////////////////////////////////////

void DecompressActual1()	80C373
///
// Preconditions:	$0C-$0E stores a pointer to compressed data.
//						The caller looks this up based on whichever player we are loading.
//						The compressed data is expected to start with a header containing the compression type.
//						The compression type is one of four choices: FB10, FB46, FB30, FB7A.
//						In practice, the profile images are all of compression type FB30.
//
//						For Patrick Roy  this is 9a862e.
//
//					$10-$12 stores a pointer to where the decompressed result should be written.
//						In practice, this is always 7F0000 for profile images.
//
// Postconditions: Decompressed result is written to the requested location.
//
// This function is used for a lot more than just decompressing profile images. It gets called on startup, too, for team logos.
//				 
$80/C373 08          PHP                     A:0000 X:0478 Y:0000 P:envmxdiZc
$80/C374 8B          PHB                     A:0000 X:0478 Y:0000 P:envmxdiZc
$80/C375 C2 30       REP #$30                A:0000 X:0478 Y:0000 P:envmxdiZc
$80/C377 22 9A 85 80 JSL $80859A[$80:859A]   A:0000 X:0478 Y:0000 P:envmxdiZc	// call DecompressActual1_WaitUntilReady()

$80/C37B A5 11       LDA $11    [$00:0011]   A:0000 X:0478 Y:0000 P:envmxdiZC
$80/C37D 8D 82 21    STA $2182  [$9F:2182]   A:7F00 X:0478 Y:0000 P:envmxdizC
$80/C380 A5 10       LDA $10    [$00:0010]   A:7F00 X:0478 Y:0000 P:envmxdizC
$80/C382 8D 81 21    STA $2181  [$9F:2181]   A:0000 X:0478 Y:0000 P:envmxdiZC	// Set up output register based on $10-$12

$80/C385 A5 0D       LDA $0D    [$00:000D]   A:0000 X:0478 Y:0000 P:envmxdiZC
$80/C387 48          PHA                     A:9A86 X:0478 Y:0000 P:eNvmxdizC
$80/C388 AB          PLB                     A:9A86 X:0478 Y:0000 P:eNvmxdizC
$80/C389 AB          PLB                     A:9A86 X:0478 Y:0000 P:eNvmxdizC	// Set data bank to $0D.

$80/C38A A4 0C       LDY $0C    [$00:000C]   A:9A86 X:0478 Y:0000 P:eNvmxdizC
$80/C38C B9 03 00    LDA $0003,y[$9A:8631]   A:9A86 X:0478 Y:862E P:eNvmxdizC
$80/C38F EB          XBA                     A:8004 X:0478 Y:862E P:eNvmxdizC
$80/C390 48          PHA                     A:0480 X:0478 Y:862E P:eNvmxdizC

$80/C391 B9 00 00    LDA $0000,y[$9A:862E]   A:0480 X:0478 Y:862E P:eNvmxdizC	// Load the compression type from the header.
																				//		This is FB30 for profile images.
$80/C394 C8          INY                     A:FB30 X:0478 Y:862E P:eNvmxdizC
$80/C395 C8          INY                     A:FB30 X:0478 Y:862F P:eNvmxdizC

$80/C396 C9 10 FB    CMP #$FB10              A:FB30 X:0478 Y:8630 P:eNvmxdizC	// Choose different routines for the different compression types
$80/C399 D0 05       BNE $05    [$C3A0]      A:FB30 X:0478 Y:8630 P:envmxdizC
...

$80/C3A0 C9 46 FB    CMP #$FB46              A:FB30 X:0478 Y:8630 P:envmxdizC
$80/C3A3 D0 05       BNE $05    [$C3AA]      A:FB30 X:0478 Y:8630 P:eNvmxdizc
...

$80/C3AA C9 30 FB    CMP #$FB30              A:FB30 X:0478 Y:8630 P:eNvmxdizc	// Check for ProfileImageCompressionType
$80/C3AD D0 05       BNE $05    [$C3B4]      A:FB30 X:0478 Y:8630 P:envmxdiZC
$80/C3AF 20 B3 BB    JSR $BBB3  [$80:BBB3]   A:FB30 X:0478 Y:8630 P:envmxdiZC	// Call DecompressFB30()
$80/C3B2 80 11       BRA $11    [$C3C5]      A:8300 X:0008 Y:0000 P:eNvmXdizc	// goto DoneDecompression

$80/C3B4 C9 7A FB    CMP #$FB7A              A:8300 X:0480 Y:0000 P:envmxdiZc	// Last type to check
$80/C3B7 D0 05       BNE $05    [$C3BE]      A:8300 X:0480 Y:0000 P:envmxdiZc	// Fall through to InvalidCompressionType if this one isn't it either.
$80/C3B9 20 F3 C2    JSR $C2F3  [$80:C2F3]   A:8300 X:0480 Y:0000 P:envmxdiZc
$80/C3BC 80 07       BRA $07    [$C3C5]      A:8300 X:0480 Y:0000 P:envmxdiZc

InvalidCompressionType:
$80/C3BE FA          PLX                     A:8300 X:0480 Y:0000 P:envmxdiZc	// Returns an error code if we couldn't make sense of the compression type.
$80/C3BF A2 01 00    LDX #$0001              A:8300 X:0480 Y:0000 P:envmxdiZc
$80/C3C2 AB          PLB                     A:8300 X:0480 Y:0000 P:envmxdiZc
$80/C3C3 28          PLP                     A:8300 X:0480 Y:0000 P:envmxdiZc
$80/C3C4 6B          RTL                     A:8300 X:0480 Y:0000 P:envmxdiZc

DoneDecompression:
$80/C3C5 C2 30       REP #$30                A:8300 X:0008 Y:0000 P:eNvmXdizc
$80/C3C7 FA          PLX                     A:8300 X:0008 Y:0000 P:eNvmxdizc
$80/C3C8 AB          PLB                     A:8300 X:0480 Y:0000 P:envmxdizc
$80/C3C9 28          PLP                     A:8300 X:0480 Y:0000 P:eNvmxdizc
$80/C3CA 6B          RTL                     A:8300 X:0480 Y:0000 P:envmxdiZc

///////////////////////////////////////////////////////////////////////////////////////////////

void DecompressActual2()	9B85C2
// Preconditions: 
//					$00 contains an arg.
//					$0C-$0E = Stores a pointer to decompressed data (e.g., 7F0000)
//					$10-$12 = Dest pointer (E.g., 7F6F00, 7F6900, 7F6300, 7F5D00, 7F5700, 7F5100, for home
											      7F4B00, 7F4500, 7F3F00, 7F3900, 7F3300, 7F2D00, for away)
//
// Postconditions: The decompressed bytes are transferred to a different location in system memory.
//				   Scrambles $00.
//				   Scrambles $04.
//				   Scrambles $06.
//				   Scrambles $14.
//				   Scrambles $16.

$9B/85C2 A6 00       LDX $00    [$00:0000]   A:0000 X:000A Y:0000 P:envmxdiZc	// This seems pointless but oh well.

$9B/85C4 8B          PHB                     A:0000 X:0480 Y:0000 P:envmxdizc	// Back up the data bank reg.

$9B/85C5 E2 20       SEP #$20                A:0000 X:0480 Y:0000 P:envmxdizc
$9B/85C7 A5 0E       LDA $0E    [$00:000E]   A:0000 X:0480 Y:0000 P:envMxdizc
$9B/85C9 48          PHA                     A:007F X:0480 Y:0000 P:envMxdizc	
$9B/85CA C2 20       REP #$20                A:007F X:0480 Y:0000 P:envMxdizc
$9B/85CC AB          PLB                     A:007F X:0480 Y:0000 P:envmxdizc	// Set the data bank to the high byte of the source pointer.

$9B/85CD 64 04       STZ $04    [$00:0004]   A:007F X:0480 Y:0000 P:envmxdizc	// Set $04 = 0
$9B/85CF A9 FE FF    LDA #$FFFE              A:007F X:0480 Y:0000 P:envmxdizc	// Set $06 = -1
$9B/85D2 85 06       STA $06    [$00:0006]   A:FFFE X:0480 Y:0000 P:eNvmxdizc

$9B/85D4 A5 00       LDA $00    [$00:0000]   A:FFFE X:0480 Y:0000 P:eNvmxdizc
$9B/85D6 4A          LSR A                   A:0480 X:0480 Y:0000 P:envmxdizc
$9B/85D7 4A          LSR A                   A:0240 X:0480 Y:0000 P:envmxdizc	
$9B/85D8 85 00       STA $00    [$00:0000]   A:0120 X:0480 Y:0000 P:envmxdizc	// $00 /= 4

$9B/85DA 80 0C       BRA $0C    [$85E8]      A:0120 X:0480 Y:0000 P:envmxdizc

$9B/85DC 8A          TXA                     A:0000 X:0080 Y:0000 P:envmxdiZc
$9B/85DD 4A          LSR A                   A:0080 X:0080 Y:0000 P:envmxdizc
$9B/85DE 4A          LSR A                   A:0040 X:0080 Y:0000 P:envmxdizc
$9B/85DF 4A          LSR A                   A:0020 X:0080 Y:0000 P:envmxdizc
$9B/85E0 4A          LSR A                   A:0010 X:0080 Y:0000 P:envmxdizc
$9B/85E1 F0 54       BEQ $54    [$8637]      A:0008 X:0080 Y:0000 P:envmxdizc
$9B/85E3 AA          TAX                     A:0008 X:0080 Y:0000 P:envmxdizc
$9B/85E4 C8          INY                     A:0008 X:0008 Y:0000 P:envmxdizc
$9B/85E5 C8          INY                     A:0008 X:0008 Y:0001 P:envmxdizc
$9B/85E6 80 0C       BRA $0C    [$85F4]      A:0008 X:0008 Y:0002 P:envmxdizc

$9B/85E8 A2 80 00    LDX #$0080              A:0120 X:0480 Y:0000 P:envmxdizc
$9B/85EB 64 14       STZ $14    [$00:0014]   A:0120 X:0080 Y:0000 P:envmxdizc
$9B/85ED 64 16       STZ $16    [$00:0016]   A:0120 X:0080 Y:0000 P:envmxdizc
$9B/85EF A5 04       LDA $04    [$00:0004]   A:0120 X:0080 Y:0000 P:envmxdizc
$9B/85F1 0A          ASL A                   A:0000 X:0080 Y:0000 P:envmxdiZc
$9B/85F2 0A          ASL A                   A:0000 X:0080 Y:0000 P:envmxdiZc
$9B/85F3 A8          TAY                     A:0000 X:0080 Y:0000 P:envmxdiZc

$9B/85F4 B1 0C       LDA ($0C),y[$7F:0000]   A:0000 X:0080 Y:0000 P:envmxdiZc
$9B/85F6 F0 E4       BEQ $E4    [$85DC]      A:0000 X:0080 Y:0000 P:envmxdiZc

$9B/85F8 EB          XBA                     A:0B00 X:0008 Y:004A P:envmxdizc
$9B/85F9 A8          TAY                     A:000B X:0008 Y:004A P:envmxdizc

$9B/85FA 98          TYA                     A:000B X:0008 Y:000B P:envmxdizc
$9B/85FB 0A          ASL A                   A:000B X:0008 Y:000B P:envmxdizc
$9B/85FC 90 05       BCC $05    [$8603]      A:0016 X:0008 Y:000B P:envmxdizc
$9B/8603 0A          ASL A                   A:0016 X:0008 Y:000B P:envmxdizc
$9B/8604 90 05       BCC $05    [$860B]      A:002C X:0008 Y:000B P:envmxdizc
$9B/860B 0A          ASL A                   A:002C X:0008 Y:000B P:envmxdizc
$9B/860C 90 05       BCC $05    [$8613]      A:0058 X:0008 Y:000B P:envmxdizc

$9B/860E A8          TAY                     A:8000 X:0001 Y:6000 P:eNvmxdizC
$9B/860F 8A          TXA                     A:8000 X:0001 Y:8000 P:eNvmxdizC
$9B/8610 04 17       TSB $17    [$00:0017]   A:0001 X:0001 Y:8000 P:envmxdizC
$9B/8612 98          TYA                     A:0001 X:0001 Y:8000 P:envmxdiZC

$9B/8613 0A          ASL A                   A:0058 X:0008 Y:000B P:envmxdizc
$9B/8614 90 05       BCC $05    [$861B]      A:00B0 X:0008 Y:000B P:envmxdizc
$9B/861B A8          TAY                     A:00B0 X:0008 Y:000B P:envmxdizc
$9B/861C 8A          TXA                     A:00B0 X:0008 Y:00B0 P:envmxdizc
$9B/861D 4A          LSR A                   A:0008 X:0008 Y:00B0 P:envmxdizc
$9B/861E AA          TAX                     A:0004 X:0008 Y:00B0 P:envmxdizc
$9B/861F E0 10 00    CPX #$0010              A:0004 X:0004 Y:00B0 P:envmxdizc
$9B/8622 B0 D6       BCS $D6    [$85FA]      A:0004 X:0004 Y:00B0 P:eNvmxdizc
$9B/8624 E0 00 00    CPX #$0000              A:0004 X:0004 Y:00B0 P:eNvmxdizc
$9B/8627 F0 0E       BEQ $0E    [$8637]      A:0004 X:0004 Y:00B0 P:envmxdizC
$9B/8629 E0 08 00    CPX #$0008              A:0004 X:0004 Y:00B0 P:envmxdizC
$9B/862C 90 CC       BCC $CC    [$85FA]      A:0004 X:0004 Y:00B0 P:eNvmxdizc

$9B/862E A5 04       LDA $04    [$00:0004]   A:0008 X:0008 Y:0000 P:envmxdiZC
$9B/8630 0A          ASL A                   A:001D X:0008 Y:0000 P:envmxdizC
$9B/8631 0A          ASL A                   A:003A X:0008 Y:0000 P:envmxdizc
$9B/8632 A8          TAY                     A:0074 X:0008 Y:0000 P:envmxdizc
$9B/8633 C8          INY                     A:0074 X:0008 Y:0074 P:envmxdizc
$9B/8634 C8          INY                     A:0074 X:0008 Y:0075 P:envmxdizc
$9B/8635 80 BD       BRA $BD    [$85F4]      A:0074 X:0008 Y:0076 P:envmxdizc

$9B/8637 A5 06       LDA $06    [$00:0006]   A:0000 X:0008 Y:0012 P:envmxdiZC
$9B/8639 1A          INC A                   A:0006 X:0008 Y:0012 P:envmxdizC
$9B/863A 1A          INC A                   A:0007 X:0008 Y:0012 P:envmxdizC
$9B/863B A8          TAY                     A:0008 X:0008 Y:0012 P:envmxdizC
$9B/863C 29 10 00    AND #$0010              A:0008 X:0008 Y:0008 P:envmxdizC
$9B/863F F0 06       BEQ $06    [$8647]      A:0000 X:0008 Y:0008 P:envmxdiZC
$9B/8647 84 06       STY $06    [$00:0006]   A:0000 X:0008 Y:0008 P:envmxdiZC
$9B/8649 A5 16       LDA $16    [$00:0016]   A:0000 X:0008 Y:0008 P:envmxdiZC
$9B/864B 91 10       STA ($10),y[$7F:6F08]   A:0000 X:0008 Y:0008 P:envmxdiZC
$9B/864D 98          TYA                     A:0000 X:0008 Y:0008 P:envmxdiZC
$9B/864E 18          CLC                     A:0008 X:0008 Y:0008 P:envmxdizC
$9B/864F 69 10 00    ADC #$0010              A:0008 X:0008 Y:0008 P:envmxdizc
$9B/8652 A8          TAY                     A:0018 X:0008 Y:0008 P:envmxdizc
$9B/8653 A5 14       LDA $14    [$00:0014]   A:0018 X:0008 Y:0018 P:envmxdizc
$9B/8655 91 10       STA ($10),y[$7F:6F18]   A:0000 X:0008 Y:0018 P:envmxdiZc
$9B/8657 E6 04       INC $04    [$00:0004]   A:0000 X:0008 Y:0018 P:envmxdiZc
$9B/8659 C6 00       DEC $00    [$00:0000]   A:0000 X:0008 Y:0018 P:envmxdizc
$9B/865B 10 8B       BPL $8B    [$85E8]      A:0000 X:0008 Y:0018 P:envmxdizc

$9B/865D AB          PLB                     A:4444 X:0000 Y:0490 P:eNvmxdizc
$9B/865E 6B          RTL                     A:4444 X:0000 Y:0490 P:eNvmxdizc

///////////////////////////////////////////////////////////////////////////////////////////////

$9D/C99F 85 A5       STA $A5    [$00:00A5]   // Store an argument used by LoadTeamProfileImages.

///////////////////////////////////////////////////////////////////////////////////////////////

// void LoadTeamProfileImagesForGameSetup() 9DCC42
// Preconditions: Choice of home or away team is in $91. 0 means home, 2 means away
//		  Current teams choices are stored at 0x9F1C98 and 0x9f1C9A for home and away respectively.
//
// Postconditions: 
//		  Six players on a team have their decompressed profile images stored in system memory at the appropriate location.
//		  For example, when loading profile images for Montreal, they get stored to RAM file offset 0x015000 = RAM address 0x7F5000 up to about 0x7F7380 (length 0x2380)
//        Doesn't do VRAM transfer.
// 
// This function is called twice when loading the GAME SETUP screen, once per team.
// Scrambles $0C-$0E.
// Scrambles $10-$12.
//
$9D/CC42 A5 A5       LDA $A5    [$00:00A5]  
$9D/CC44 48          PHA                    // Politely back up $A5 because LookupPlayerIndexForPlayerWithProfile is going to scramble it.

$9D/CC45 A2 0A 00    LDX #$000A				// OffsetCounter=10. This gets used to index in an array of 2-byte elements.

ForEachProfileImagePlayerOnTeam:
$9D/CC48 DA          PHX                     
$9D/CC49 8A          TXA                     
$9D/CC4A 4A          LSR A                   // Divide OffsetCounter by 2 to get a [0-5] player index, used as an argument to the function below
$9D/CC4B 22 73 C9 9D JSL $9DC973[$9D:C973]   // Call LookupPlayerIndexForPlayerWithProfile().
											 //		Translates the [0-5] index to PlayerIndexOnTeam.
											 //     Sets $A5 to PlayerIndexOnTeam.

$9D/CC4F A6 91       LDX $91    [$00:0091]   // Load the choice of HomeOrAway. 0 == home, 2 == away					     
$9D/CC51 BD 98 1C    LDA $1C98,x[$9F:1C98]   // Load the index of CurrentTeam. 

					     // Current selection for home team is stored at 0x9F1C98					     
					     // Current selection for visitor team is stored at 0x9F1C9A

					     // Each possible value of CurrentTeam is one of the below
					     // 00 - Anaheim
					     // 01 - Boston
					     // 02 - Buffalo
					     // 03 - Calgary
					     // 04 - Chicago
					     // 05 - Dallas
					     // 06 - Detroit
					     // 07 - Edmonton
					     // 08 - Florida
					     // 09 - Hartford
					     // 0A - LA
					     // 0B - Montreal
					     // 0C - New Jersey
					     // 0D - New York Islanders
					     // 0E - New York Rangers
					     // 0F - Ottawa
					     // 10 - Philidelphia
					     // 11 - Pittsburg
					     // 12 - Quebec
					     // 13 - San Jose
					     // 14 - St Louis
					     // 15 - Tampa Bay
					     // 16 - Toronto
					     // 17 - Vancouver
					     // 18 - Washington
					     // 19 - Winnepeg
					     // 1A - All Stars East
					     // 1B - All Stars West

$9D/CC54 9F 6C 34 7E STA $7E346C,x[$7E:346C] // Stash the CurrentTeamIndex in the appropriate slot for home versus away

$9D/CC58 A2 1A 00    LDX #$001A              // Set X = 26
$9D/CC5B 22 4C 8F 80 JSL $808F4C[$80:8F4C]   // Call MultiplyAWithX() to set 
											 //		Acc = CurrentTeam * 26
$9D/CC5F 18          CLC                     
$9D/CC60 65 A5       ADC $A5    [$00:00A5]   //     Acc = GlobalPlayerIndex.
											 // GlobalPlayerIndex goes from 0 to about 800, a numerical ID for which player.
											 // Starting from the first player in Anaheim, to the last player of ASW.

$9D/CC62 0A          ASL A                   // X = GlobalPlayerIndex * 4
$9D/CC63 0A          ASL A	             // These are getting into pretty high numbers but fortunately 4 bytes is enough. :)
$9D/CC64 AA          TAX    
											 // There is a giant array stored at 9DCD53.
											 // There are like 800 elements, one per GlobalPlayerIndex.
											 // Each element is 4 bytes.
											 // Each element is a long pointer.
											 // Each element is pointer to compressed data (i.e., data with the header 30 FB...)
$9D/CC65 BF 53 CD 9D LDA $9DCD53,x[$9D:D16F]
$9D/CC69 85 0C       STA $0C    [$00:000C]                 
$9D/CC6B BF 55 CD 9D LDA $9DCD55,x[$9D:D1CD] 
$9D/CC6F 85 0E       STA $0E    [$00:000E]   // $0C-$0E stores a long pointer to compressed data.

$9D/CC71 A9 7F 00    LDA #$007F              
$9D/CC74 85 12       STA $12    [$00:0012]   
$9D/CC76 A9 00 00    LDA #$0000              
$9D/CC79 85 10       STA $10    [$00:0010]   // $10-$12 stores 7F0000.

$9D/CC7B 22 73 C3 80 JSL $80C373[$80:C373]   // Call DecompressActual1().

$9D/CC7F 86 00       STX $00    [$00:0000]   
$9D/CC81 A3 01       LDA $01,s  [$00:1FF6]   // Load OffsetCounter from before to use as an array element index
$9D/CC83 AA          TAX                     

$9D/CC84 A5 91       LDA $91    [$00:0091]   
$9D/CC86 F0 05       BEQ $05    [$CC8D]      
$9D/CC88 A9 00 2D    LDA #$2D00              
$9D/CC8B 80 03       BRA $03    [$CC90]      
$9D/CC8D A9 00 51    LDA #$5100              
$9D/CC90 18          CLC					Acc = 5100 for Home. 2D00 for Away.

$9D/CC91 7F AE CC 9D ADC $9DCCAE,x[$9D:CCAE]	// $9DCCAE is an array of pre-computed increments
												// 0000, 0600, 0C00, 1200, 1800
												// so that we don't have to do that sorta multiplication at runtime.
												// Call the array Times600.

												// PointerLow = 2D00 + Times600[PlayerIndex]	for Home, or
												                5100 + Times600[PlayerIndex]	for Away

$9D/CC95 85 10       STA $10    [$00:0010]		// $12 already contains 7F as high word.

$9D/CC97 A9 7F 00    LDA #$007F					
$9D/CC9A 85 0E       STA $0E    [$00:000E]				
$9D/CC9C A9 00 00    LDA #$0000              
$9D/CC9F 85 0C       STA $0C    [$00:000C]		// $0C-$0E = 7F0000
												// $10     = PointerLow

$9D/CCA1 22 C2 85 9B JSL $9B85C2[$9B:85C2]   // Call DecompressActual2()

$9D/CCA5 FA          PLX                     // Pull OffsetCounter
$9D/CCA6 CA          DEX                     // OffsetCounter -= 2
$9D/CCA7 CA          DEX                     

$9D/CCA8 10 9E       BPL $9E    [$CC48]      // if OffsetCounter > 0, goto ForEachProfileImagePlayerOnTeam

Done:
$9D/CCAA 68          PLA                     
$9D/CCAB 85 A5       STA $A5    [$00:00A5]   
$9D/CCAD 6B          RTL					// Restore $A5 and return

///////////////////////////////////////////////////////////////////////////////////////////

; During EDIT LINES
$9B/86B6 A9 FF FF    LDA #$FFFF              A:0000 X:0300 Y:05A0 P:envmxdiZc
$9B/86B9 8F 52 34 7E STA $7E3452[$7E:3452]   A:0000 X:0300 Y:05A0 P:envmxdiZc
$9B/86BD 22 AD D9 9D JSL $9DD9AD[$9D:D9AD]   A:0000 X:0300 Y:05A0 P:envmxdiZc	; Call DecompressForGameSetupAndEditLines()
$9B/86C1 22 0F 89 9B JSL $9B890F[$9B:890F]   A:0000 X:0300 Y:05A0 P:envmxdiZc	; - patch sysmem here
$9B/86C5 A9 05 00    LDA #$0005              A:0177 X:0002 Y:0005 P:envmxdizc

///////////////////////////////////////////////////////////////////////////////////////////

; During GAME SETUP
...
$9D/A629 8F 52 34 7E STA $7E3452[$7E:3452]   A:FFFF X:0300 Y:05A0 P:eNvmxdizc
$9D/A62D 22 AD D9 9D JSL $9DD9AD[$9D:D9AD]   A:FFFF X:0300 Y:05A0 P:eNvmxdizc	; Call DecompressForGameSetupAndEditLines()
$9D/A631 E2 20       SEP #$20                A:FFFF X:0300 Y:05A0 P:eNvmxdizc

$9D/A633 A9 01       LDA #$01                A:FFFF X:0300 Y:05A0 P:eNvMxdizc
$9D/A635 8D 05 21    STA $2105  [$9F:2105]   A:FF01 X:0300 Y:05A0 P:envMxdizc
$9D/A638 C2 20       REP #$20                A:FF01 X:0300 Y:05A0 P:envMxdizc
$9D/A63A A2 4F 00    LDX #$004F              A:FF01 X:0300 Y:05A0 P:envmxdizc
$9D/A63D A0 18 00    LDY #$0018              A:FF01 X:004F Y:05A0 P:envmxdizc
$9D/A640 A9 9C 00    LDA #$009C              A:FF01 X:004F Y:0018 P:envmxdizc
$9D/A643 85 8F       STA $8F    [$00:008F]   A:009C X:004F Y:0018 P:envmxdizc
$9D/A645 A9 97 F5    LDA #$F597              A:009C X:004F Y:0018 P:envmxdizc
$9D/A648 85 8D       STA $8D    [$00:008D]   A:F597 X:004F Y:0018 P:eNvmxdizc
$9D/A64A 22 6C 93 9C JSL $9C936C[$9C:936C]   A:F597 X:004F Y:0018 P:eNvmxdizc
$9D/A64E A9 47 1D    LDA #$1D47              A:000C X:000F Y:000C P:envmxdiZC
$9D/A651 8F 0A 75 7F STA $7F750A[$7F:750A]   A:1D47 X:000F Y:000C P:envmxdizC
$9D/A655 A9 FF FF    LDA #$FFFF              A:1D47 X:000F Y:000C P:envmxdizC
$9D/A658 8F 0E 75 7F STA $7F750E[$7F:750E]   A:FFFF X:000F Y:000C P:eNvmxdizC
$9D/A65C A9 A5 14    LDA #$14A5              A:FFFF X:000F Y:000C P:eNvmxdizC
$9D/A65F 8F 82 76 7F STA $7F7682[$7F:7682]   A:14A5 X:000F Y:000C P:envmxdizC
$9D/A663 A9 7B 6F    LDA #$6F7B              A:14A5 X:000F Y:000C P:envmxdizC
$9D/A666 8F C2 75 7F STA $7F75C2[$7F:75C2]   A:6F7B X:000F Y:000C P:envmxdizC
$9D/A66A 22 D4 E0 9D JSL $9DE0D4[$9D:E0D4]   A:6F7B X:000F Y:000C P:envmxdizC
$9D/A66E 22 DE AF 80 JSL $80AFDE[$80:AFDE]   A:7500 X:0002 Y:0000 P:envmxdizc
$9D/A672 22 23 D9 9D JSL $9DD923[$9D:D923]   A:2301 X:2320 Y:2100 P:eNvmxdizC
$9D/A676 22 17 80 9B JSL $9B8017[$9B:8017]   A:0400 X:0800 Y:0400 P:envmxdizc
$9D/A67A 22 26 85 9B JSL $9B8526[$9B:8526]   A:000F X:0000 Y:0400 P:envmxdizc
$9D/A67E A9 00 00    LDA #$0000              A:000F X:0000 Y:0400 P:envmxdizc
$9D/A681 8D CB 0C    STA $0CCB  [$9F:0CCB]   A:0000 X:0000 Y:0400 P:envmxdiZc
$9D/A684 A5 8F       LDA $8F    [$00:008F]   A:0000 X:0000 Y:0400 P:envmxdiZc
$9D/A686 48          PHA                     A:009C X:0000 Y:0400 P:envmxdizc
$9D/A687 A5 8D       LDA $8D    [$00:008D]   A:009C X:0000 Y:0400 P:envmxdizc
$9D/A689 48          PHA                     A:F597 X:0000 Y:0400 P:eNvmxdizc
$9D/A68A A9 9C 00    LDA #$009C              A:F597 X:0000 Y:0400 P:eNvmxdizc
$9D/A68D 85 8F       STA $8F    [$00:008F]   A:009C X:0000 Y:0400 P:envmxdizc
$9D/A68F A9 A3 F5    LDA #$F5A3              A:009C X:0000 Y:0400 P:envmxdizc
$9D/A692 85 8D       STA $8D    [$00:008D]   A:F5A3 X:0000 Y:0400 P:eNvmxdizc
$9D/A694 22 BE C7 9E JSL $9EC7BE[$9E:C7BE]   A:F5A3 X:0000 Y:0400 P:eNvmxdizc
$9D/A698 68          PLA                     A:F5A8 X:0000 Y:0005 P:envmxdiZc
$9D/A699 85 8D       STA $8D    [$00:008D]   A:F597 X:0000 Y:0005 P:eNvmxdizc
$9D/A69B 68          PLA                     A:F597 X:0000 Y:0005 P:eNvmxdizc
$9D/A69C 85 8F       STA $8F    [$00:008F]   A:009C X:0000 Y:0005 P:envmxdizc
$9D/A69E 22 27 A2 9C JSL $9CA227[$9C:A227]   A:009C X:0000 Y:0005 P:envmxdizc
$9D/A6A2 A9 28 00    LDA #$0028              A:0000 X:010E Y:001E P:envmxdizC
$9D/A6A5 8D 43 0E    STA $0E43  [$9F:0E43]   A:0028 X:010E Y:001E P:envmxdizC
$9D/A6A8 8D 4F 0E    STA $0E4F  [$9F:0E4F]   A:0028 X:010E Y:001E P:envmxdizC
$9D/A6AB A9 FF FF    LDA #$FFFF              A:0028 X:010E Y:001E P:envmxdizC
$9D/A6AE 8F CC 34 7E STA $7E34CC[$7E:34CC]   A:FFFF X:010E Y:001E P:eNvmxdizC
$9D/A6B2 A5 91       LDA $91    [$00:0091]   A:FFFF X:010E Y:001E P:eNvmxdizC
$9D/A6B4 48          PHA                     A:0120 X:010E Y:001E P:envmxdizC
$9D/A6B5 A9 00 00    LDA #$0000              A:0120 X:010E Y:001E P:envmxdizC
$9D/A6B8 85 91       STA $91    [$00:0091]   A:0000 X:010E Y:001E P:envmxdiZC
$9D/A6BA 22 34 93 9B JSL $9B9334[$9B:9334]   A:0000 X:010E Y:001E P:envmxdiZC
$9D/A6BE A9 02 00    LDA #$0002              A:0000 X:010E Y:001E P:envmxdiZC
$9D/A6C1 85 91       STA $91    [$00:0091]   A:0002 X:010E Y:001E P:envmxdizC
$9D/A6C3 22 34 93 9B JSL $9B9334[$9B:9334]   A:0002 X:010E Y:001E P:envmxdizC
$9D/A6C7 68          PLA                     A:0000 X:010E Y:001E P:envmxdiZC
$9D/A6C8 85 91       STA $91    [$00:0091]   A:0120 X:010E Y:001E P:envmxdizC
$9D/A6CA 64 C1       STZ $C1    [$00:00C1]   A:0120 X:010E Y:001E P:envmxdizC
$9D/A6CC 64 A5       STZ $A5    [$00:00A5]   A:0120 X:010E Y:001E P:envmxdizC
$9D/A6CE 20 4F AC    JSR $AC4F  [$9D:AC4F]   A:0120 X:010E Y:001E P:envmxdizC
$9D/A6D1 20 73 A9    JSR $A973  [$9D:A973]   A:0001 X:010E Y:001E P:envmxdizc
$9D/A6D4 20 83 AA    JSR $AA83  [$9D:AA83]   A:0000 X:000E Y:0010 P:envmxdiZC
$9D/A6D7 22 17 80 9B JSL $9B8017[$9B:8017]   A:7500 X:0002 Y:0000 P:envmxdizc
$9D/A6DB 22 F6 DF 9D JSL $9DDFF6[$9D:DFF6]   A:2000 X:0000 Y:0000 P:envmxdizc
$9D/A6DF 22 BD 86 80 JSL $8086BD[$80:86BD]   A:200C X:E064 Y:0000 P:envmxdizc
$9D/A6E3 A9 08 00    LDA #$0008              A:200C X:009C Y:0000 P:envmxdizc
$9D/A6E6 A2 40 00    LDX #$0040              A:0008 X:009C Y:0000 P:envmxdizc
$9D/A6E9 22 F5 CC 9D JSL $9DCCF5[$9D:CCF5]   A:0008 X:0040 Y:0000 P:envmxdizc
$9D/A6ED 22 23 D9 9D JSL $9DD923[$9D:D923]   A:CDB1 X:009D Y:00C4 P:envmxdizc
$9D/A6F1 A2 00 00    LDX #$0000              A:F800 X:00F0 Y:0400 P:eNvmxdizc
$9D/A6F4 22 36 CC 9D JSL $9DCC36[$9D:CC36]   A:F800 X:0000 Y:0400 P:envmxdiZc
$9D/A6F8 A2 02 00    LDX #$0002              A:FFFF X:0000 Y:FFFF P:eNvmxdizc
$9D/A6FB 22 36 CC 9D JSL $9DCC36[$9D:CC36]   A:FFFF X:0002 Y:FFFF P:envmxdizc

Main:
$9D/A6FF A9 C0 12    LDA #$12C0              A:0000 X:0002 Y:FFFF P:eNvmxdizc
$9D/A702 85 BD       STA $BD    [$00:00BD]   A:12C0 X:0002 Y:FFFF P:envmxdizc
$9D/A704 22 83 85 80 JSL $808583[$80:8583]   A:12C0 X:0002 Y:FFFF P:envmxdizc
$9D/A708 AD 66 07    LDA $0766  [$9F:0766]   A:12C0 X:0002 Y:FFFF P:envmxdizc
$9D/A70B D0 F7       BNE $F7    [$A704]      A:0000 X:0002 Y:FFFF P:envmxdiZc
$9D/A70D EE 92 07    INC $0792  [$9F:0792]   A:0000 X:0002 Y:FFFF P:envmxdiZc
$9D/A710 D0 03       BNE $03    [$A715]      A:0000 X:0002 Y:FFFF P:envmxdizc
$9D/A715 22 83 85 80 JSL $808583[$80:8583]   A:0000 X:0002 Y:FFFF P:envmxdizc
$9D/A719 AE 0E 1A    LDX $1A0E  [$9F:1A0E]   A:0000 X:0002 Y:FFFF P:envmxdiZc
$9D/A71C 8A          TXA                     A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A71D 45 AF       EOR $AF    [$00:00AF]   A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A71F D0 03       BNE $03    [$A724]      A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A721 4C 68 A7    JMP $A768  [$9D:A768]   A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A768 AD 0E 1A    LDA $1A0E  [$9F:1A0E]   A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A76B F0 03       BEQ $03    [$A770]      A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A770 22 17 80 9B JSL $9B8017[$9B:8017]   A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A774 A5 83       LDA $83    [$00:0083]   A:0000 X:0000 Y:FFFF P:envmxdiZc
$9D/A776 8D 35 0D    STA $0D35  [$9F:0D35]   A:07D7 X:0000 Y:FFFF P:envmxdizc
$9D/A779 8D 01 0D    STA $0D01  [$9F:0D01]   A:07D7 X:0000 Y:FFFF P:envmxdizc
$9D/A77C 22 F5 84 9B JSL $9B84F5[$9B:84F5]   A:07D7 X:0000 Y:FFFF P:envmxdizc
$9D/A780 CE C4 0A    DEC $0AC4  [$9F:0AC4]   A:0008 X:0000 Y:FFFF P:envmxdizc
$9D/A783 22 2C 85 9B JSL $9B852C[$9B:852C]   A:0008 X:0000 Y:FFFF P:eNvmxdizc
$9D/A787 A5 A9       LDA $A9    [$00:00A9]   A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A789 29 F0 FF    AND #$FFF0              A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A78C 85 A9       STA $A9    [$00:00A9]   A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A78E F0 03       BEQ $03    [$A793]      A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A790 4C 69 A8    JMP $A869  [$9D:A869]   A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A869 A5 A9       LDA $A9    [$00:00A9]   A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A86B 89 00 10    BIT #$1000              A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A86E F0 03       BEQ $03    [$A873]      A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A873 89 00 04    BIT #$0400              A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A876 F0 0E       BEQ $0E    [$A886]      A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A886 89 00 08    BIT #$0800              A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A889 F0 0E       BEQ $0E    [$A899]      A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A899 A9 01 00    LDA #$0001              A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A89C 85 AD       STA $AD    [$00:00AD]   A:0001 X:0000 Y:FFFF P:envmxdizc
$9D/A89E A5 A9       LDA $A9    [$00:00A9]   A:0001 X:0000 Y:FFFF P:envmxdizc
$9D/A8A0 89 00 01    BIT #$0100              A:2000 X:0000 Y:FFFF P:envmxdizc
$9D/A8A3 D0 0A       BNE $0A    [$A8AF]      A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A8A5 89 00 02    BIT #$0200              A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A8A8 F0 EC       BEQ $EC    [$A896]      A:2000 X:0000 Y:FFFF P:envmxdiZc
$9D/A896 4C FF A6    JMP $A6FF  [$9D:A6FF]   A:2000 X:0000 Y:FFFF P:envmxdiZc

///////////////////////////////////////////////////////////////////////////////////////////
//
// During loading of the EDIT LINES menu, and during GAME SETUP.
// There's a bunch of repetitive code in this function. Looks like a loop that has been unrolled and manually tweaked.
//
// void DecompressForGameSetupAndEditLines() -9DD9AD
// 	This is used during the EDIT LINES menu, and before GAME SETUP
$9D/D9AD 9C C6 07    STZ $07C6  [$9F:07C6]   A:FFFF X:0000 Y:0000 P:eNvmxdizc
$9D/D9B0 A5 91       LDA $91    [$00:0091]   A:FFFF X:0000 Y:0000 P:eNvmxdizc
$9D/D9B2 48          PHA                     A:0000 X:0000 Y:0000 P:envmxdiZc
$9D/D9B3 22 F7 96 9F JSL $9F96F7[$9F:96F7]   A:0000 X:0000 Y:0000 P:envmxdiZc

$9D/D9B7 A9 81 00    LDA #$0081              A:9971 X:009C Y:0000 P:eNvmxdizc
$9D/D9BA 85 0E       STA $0E    [$00:000E]   A:0081 X:009C Y:0000 P:envmxdizc
$9D/D9BC A9 DE AB    LDA #$ABDE              A:0081 X:009C Y:0000 P:envmxdizc
$9D/D9BF 85 0C       STA $0C    [$00:000C]   A:8001 X:7C00 Y:0000 P:envmxdizc	// decompress src = 0x81ABDE

$9D/D9C1 A9 7F 00    LDA #$007F              A:8001 X:7C00 Y:0000 P:envmxdizc	// decompress dest = 0x7F78000
$9D/D9C4 85 12       STA $12    [$00:0012]   A:8001 X:7C00 Y:0000 P:envmxdizc
$9D/D9C6 A9 00 78    LDA #$7800              A:8001 X:7C00 Y:0000 P:envmxdizc
$9D/D9C9 85 10       STA $10    [$00:0010]   A:8001 X:7C00 Y:0000 P:envmxdizc	

$9D/D9CB 22 73 C3 80 JSL $80C373[$80:C373]   A:8001 X:7C00 Y:0000 P:envmxdizc	// Call DecompressActual1()

$9D/D9CF 22 08 97 9F JSL $9F9708[$9F:9708]   A:8001 X:7C00 Y:0000 P:envmxdizc	// Call EditLinesSpinUntilCond()

$9D/D9D3 22 A9 86 80 JSL $8086A9[$80:86A9]   A:0000 X:7C00 Y:0000 P:envmxdiZc	// Call EditLinesDecompressHelper()

$9D/D9D7 E2 20       SEP #$20                A:00E0 X:7C00 Y:0000 P:envmxdizc
$9D/D9D9 A9 00       LDA #$00                A:00E0 X:7C00 Y:0000 P:envMxdizc
$9D/D9DB 8D 07 21    STA $2107  [$9F:2107]   A:0000 X:7C00 Y:0000 P:envMxdiZc
$9D/D9DE A9 04       LDA #$04                A:0000 X:7C00 Y:0000 P:envMxdiZc
$9D/D9E0 8D 08 21    STA $2108  [$9F:2108]   A:0004 X:7C00 Y:0000 P:envMxdizc
$9D/D9E3 A9 12       LDA #$12                A:0004 X:7C00 Y:0000 P:envMxdizc
$9D/D9E5 8D 0B 21    STA $210B  [$9F:210B]   A:0012 X:7C00 Y:0000 P:envMxdizc
$9D/D9E8 A9 03       LDA #$03                A:0012 X:7C00 Y:0000 P:envMxdizc
$9D/D9EA 8D 01 21    STA $2101  [$9F:2101]   A:0003 X:7C00 Y:0000 P:envMxdizc
$9D/D9ED A9 13       LDA #$13                A:0003 X:7C00 Y:0000 P:envMxdizc
$9D/D9EF 8D 2C 21    STA $212C  [$9F:212C]   A:0013 X:7C00 Y:0000 P:envMxdizc
$9D/D9F2 C2 30       REP #$30                A:0013 X:7C00 Y:0000 P:envMxdizc

$9D/D9F4 A9 00 FF    LDA #$FF00              A:0013 X:7C00 Y:0000 P:envmxdizc
$9D/D9F7 8F AA 35 7E STA $7E35AA[$7E:35AA]   A:FF00 X:7C00 Y:0000 P:eNvmxdizc
$9D/D9FB 8F A8 35 7E STA $7E35A8[$7E:35A8]   A:FF00 X:7C00 Y:0000 P:eNvmxdizc
$9D/D9FF A9 00 00    LDA #$0000              A:FF00 X:7C00 Y:0000 P:eNvmxdizc
$9D/DA02 8F AC 35 7E STA $7E35AC[$7E:35AC]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$9D/DA06 AF B6 35 7E LDA $7E35B6[$7E:35B6]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$9D/DA0A 8F C4 35 7E STA $7E35C4[$7E:35C4]   A:03A0 X:7C00 Y:0000 P:envmxdizc
$9D/DA0E 0A          ASL A                   A:03A0 X:7C00 Y:0000 P:envmxdizc
$9D/DA0F 0A          ASL A                   A:0740 X:7C00 Y:0000 P:envmxdizc
$9D/DA10 0A          ASL A                   A:0E80 X:7C00 Y:0000 P:envmxdizc
$9D/DA11 0A          ASL A                   A:1D00 X:7C00 Y:0000 P:envmxdizc
$9D/DA12 69 00 10    ADC #$1000              A:3A00 X:7C00 Y:0000 P:envmxdizc
$9D/DA15 8D 16 21    STA $2116  [$9F:2116]   A:4A00 X:7C00 Y:0000 P:envmxdizc
$9D/DA18 A9 96 00    LDA #$0096              A:4A00 X:7C00 Y:0000 P:envmxdizc
$9D/DA1B 85 0E       STA $0E    [$00:000E]   A:0096 X:7C00 Y:0000 P:envmxdizc
$9D/DA1D A9 03 A1    LDA #$A103              A:0096 X:7C00 Y:0000 P:envmxdizc
$9D/DA20 85 0C       STA $0C    [$00:000C]   A:A103 X:7C00 Y:0000 P:eNvmxdizc
$9D/DA22 A9 7F 00    LDA #$007F              A:A103 X:7C00 Y:0000 P:eNvmxdizc
$9D/DA25 85 12       STA $12    [$00:0012]   A:007F X:7C00 Y:0000 P:envmxdizc
$9D/DA27 A9 00 00    LDA #$0000              A:007F X:7C00 Y:0000 P:envmxdizc
$9D/DA2A 85 10       STA $10    [$00:0010]   A:0000 X:7C00 Y:0000 P:envmxdiZc

$9D/DA2C 22 73 C3 80 JSL $80C373[$80:C373]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$9D/DA30 8A          TXA                     A:0000 X:0D40 Y:A5E4 P:envmxdiZc
$9D/DA31 4A          LSR A                   A:0D40 X:0D40 Y:A5E4 P:envmxdizc
$9D/DA32 4A          LSR A                   A:06A0 X:0D40 Y:A5E4 P:envmxdizc
$9D/DA33 4A          LSR A                   A:0350 X:0D40 Y:A5E4 P:envmxdizc
$9D/DA34 4A          LSR A                   A:01A8 X:0D40 Y:A5E4 P:envmxdizc
$9D/DA35 4A          LSR A                   A:00D4 X:0D40 Y:A5E4 P:envmxdizc
$9D/DA36 85 A5       STA $A5    [$00:00A5]   A:006A X:0D40 Y:A5E4 P:envmxdizc
$9D/DA38 18          CLC                     A:006A X:0D40 Y:A5E4 P:envmxdizc
$9D/DA39 6F C4 35 7E ADC $7E35C4[$7E:35C4]   A:006A X:0D40 Y:A5E4 P:envmxdizc
$9D/DA3D 8F C4 35 7E STA $7E35C4[$7E:35C4]   A:040A X:0D40 Y:A5E4 P:envmxdizc
$9D/DA41 8F BC 35 7E STA $7E35BC[$7E:35BC]   A:040A X:0D40 Y:A5E4 P:envmxdizc

$9D/DA45 A9 7F 00    LDA #$007F              A:040A X:0D40 Y:A5E4 P:envmxdizc
$9D/DA48 85 8B       STA $8B    [$00:008B]   A:007F X:0D40 Y:A5E4 P:envmxdizc
$9D/DA4A A9 00 00    LDA #$0000              A:007F X:0D40 Y:A5E4 P:envmxdizc
$9D/DA4D 85 89       STA $89    [$00:0089]   A:0000 X:0D40 Y:A5E4 P:envmxdiZc
$9D/DA4F A9 80 00    LDA #$0080              A:0000 X:0D40 Y:A5E4 P:envmxdiZc
$9D/DA52 85 8F       STA $8F    [$00:008F]   A:0080 X:0D40 Y:A5E4 P:envmxdizc
$9D/DA54 A9 D7 D3    LDA #$D3D7              A:0080 X:0D40 Y:A5E4 P:envmxdizc
$9D/DA57 85 8D       STA $8D    [$00:008D]   A:D3D7 X:0D40 Y:A5E4 P:eNvmxdizc
$9D/DA59 22 4B 85 9B JSL $9B854B[$9B:854B]   A:D3D7 X:0D40 Y:A5E4 P:eNvmxdizc
$9D/DA5D A9 9C 00    LDA #$009C              A:0D40 X:0020 Y:0000 P:envmxdiZc
$9D/DA60 85 8B       STA $8B    [$00:008B]   A:009C X:0020 Y:0000 P:envmxdizc
$9D/DA62 A9 00 80    LDA #$8000              A:009C X:0020 Y:0000 P:envmxdizc
$9D/DA65 85 89       STA $89    [$00:0089]   A:8000 X:0020 Y:0000 P:eNvmxdizc
$9D/DA67 A9 80 00    LDA #$0080              A:8000 X:0020 Y:0000 P:eNvmxdizc
$9D/DA6A 85 8F       STA $8F    [$00:008F]   A:0080 X:0020 Y:0000 P:envmxdizc
$9D/DA6C A9 E7 D3    LDA #$D3E7              A:0080 X:0020 Y:0000 P:envmxdizc
$9D/DA6F 85 8D       STA $8D    [$00:008D]   A:D3E7 X:0020 Y:0000 P:eNvmxdizc
$9D/DA71 A9 04 00    LDA #$0004              A:D3E7 X:0020 Y:0000 P:eNvmxdizc
$9D/DA74 85 A5       STA $A5    [$00:00A5]   A:0004 X:0020 Y:0000 P:envmxdizc
$9D/DA76 18          CLC                     A:0004 X:0020 Y:0000 P:envmxdizc
$9D/DA77 6F C4 35 7E ADC $7E35C4[$7E:35C4]   A:0004 X:0020 Y:0000 P:envmxdizc
$9D/DA7B 8F C4 35 7E STA $7E35C4[$7E:35C4]   A:040E X:0020 Y:0000 P:envmxdizc
$9D/DA7F 8F CE 35 7E STA $7E35CE[$7E:35CE]   A:040E X:0020 Y:0000 P:envmxdizc

$9D/DA83 22 4B 85 9B JSL $9B854B[$9B:854B]   A:040E X:0020 Y:0000 P:envmxdizc
$9D/DA87 A9 9A 00    LDA #$009A              A:8080 X:0020 Y:0002 P:envmxdiZc
$9D/DA8A 85 0E       STA $0E    [$00:000E]   A:009A X:0020 Y:0002 P:envmxdizc
$9D/DA8C A9 F3 C1    LDA #$C1F3              A:009A X:0020 Y:0002 P:envmxdizc
$9D/DA8F 85 0C       STA $0C    [$00:000C]   A:C1F3 X:0020 Y:0002 P:eNvmxdizc
$9D/DA91 A9 7F 00    LDA #$007F              A:C1F3 X:0020 Y:0002 P:eNvmxdizc
$9D/DA94 85 12       STA $12    [$00:0012]   A:007F X:0020 Y:0002 P:envmxdizc
$9D/DA96 A9 00 00    LDA #$0000              A:007F X:0020 Y:0002 P:envmxdizc
$9D/DA99 85 10       STA $10    [$00:0010]   A:0000 X:0020 Y:0002 P:envmxdiZc
$9D/DA9B 22 73 C3 80 JSL $80C373[$80:C373]   A:0000 X:0020 Y:0002 P:envmxdiZc
$9D/DA9F 8A          TXA                     A:8030 X:02A0 Y:0000 P:envmxdiZc
$9D/DAA0 4A          LSR A                   A:02A0 X:02A0 Y:0000 P:envmxdizc
$9D/DAA1 4A          LSR A                   A:0150 X:02A0 Y:0000 P:envmxdizc
$9D/DAA2 4A          LSR A                   A:00A8 X:02A0 Y:0000 P:envmxdizc
$9D/DAA3 4A          LSR A                   A:0054 X:02A0 Y:0000 P:envmxdizc
$9D/DAA4 4A          LSR A                   A:002A X:02A0 Y:0000 P:envmxdizc
$9D/DAA5 18          CLC                     A:0015 X:02A0 Y:0000 P:envmxdizc
$9D/DAA6 6F C4 35 7E ADC $7E35C4[$7E:35C4]   A:0015 X:02A0 Y:0000 P:envmxdizc
$9D/DAAA 8F C4 35 7E STA $7E35C4[$7E:35C4]   A:0423 X:02A0 Y:0000 P:envmxdizc
$9D/DAAE 8F BE 35 7E STA $7E35BE[$7E:35BE]   A:0423 X:02A0 Y:0000 P:envmxdizc
$9D/DAB2 A9 7F 00    LDA #$007F              A:0423 X:02A0 Y:0000 P:envmxdizc
$9D/DAB5 85 0E       STA $0E    [$00:000E]   A:007F X:02A0 Y:0000 P:envmxdizc
$9D/DAB7 A9 00 00    LDA #$0000              A:007F X:02A0 Y:0000 P:envmxdizc
$9D/DABA 85 0C       STA $0C    [$00:000C]   A:0000 X:02A0 Y:0000 P:envmxdiZc

$9D/DABC A0 FF FF    LDY #$FFFF              A:0000 X:02A0 Y:0000 P:envmxdiZc
$9D/DABF 22 1F 89 80 JSL $80891F[$80:891F]   A:0000 X:02A0 Y:FFFF P:eNvmxdizc
$9D/DAC3 AF C4 35 7E LDA $7E35C4[$7E:35C4]   A:0153 X:02A0 Y:0153 P:envmxdizc
$9D/DAC7 8F BE 35 7E STA $7E35BE[$7E:35BE]   A:0423 X:02A0 Y:0153 P:envmxdizc
$9D/DACB A9 9C 00    LDA #$009C              A:0423 X:02A0 Y:0153 P:envmxdizc
$9D/DACE 85 0E       STA $0E    [$00:000E]   A:009C X:02A0 Y:0153 P:envmxdizc
$9D/DAD0 A9 98 80    LDA #$8098              A:009C X:02A0 Y:0153 P:envmxdizc
$9D/DAD3 85 0C       STA $0C    [$00:000C]   A:8098 X:02A0 Y:0153 P:eNvmxdizc
$9D/DAD5 A2 40 01    LDX #$0140              A:8098 X:02A0 Y:0153 P:eNvmxdizc
$9D/DAD8 8A          TXA                     A:8098 X:0140 Y:0153 P:envmxdizc
$9D/DAD9 4A          LSR A                   A:0140 X:0140 Y:0153 P:envmxdizc
$9D/DADA 4A          LSR A                   A:00A0 X:0140 Y:0153 P:envmxdizc
$9D/DADB 4A          LSR A                   A:0050 X:0140 Y:0153 P:envmxdizc
$9D/DADC 4A          LSR A                   A:0028 X:0140 Y:0153 P:envmxdizc
$9D/DADD 4A          LSR A                   A:0014 X:0140 Y:0153 P:envmxdizc
$9D/DADE 18          CLC                     A:000A X:0140 Y:0153 P:envmxdizc
$9D/DADF 6F C4 35 7E ADC $7E35C4[$7E:35C4]   A:000A X:0140 Y:0153 P:envmxdizc
$9D/DAE3 8F C4 35 7E STA $7E35C4[$7E:35C4]   A:042D X:0140 Y:0153 P:envmxdizc
$9D/DAE7 8F C0 35 7E STA $7E35C0[$7E:35C0]   A:042D X:0140 Y:0153 P:envmxdizc

$9D/DAEB A0 FF FF    LDY #$FFFF              A:042D X:0140 Y:0153 P:envmxdizc
$9D/DAEE 22 1F 89 80 JSL $80891F[$80:891F]   A:042D X:0140 Y:FFFF P:eNvmxdizc
$9D/DAF2 AF C4 35 7E LDA $7E35C4[$7E:35C4]   A:00A3 X:0140 Y:00A3 P:envmxdizc
$9D/DAF6 8F C0 35 7E STA $7E35C0[$7E:35C0]   A:042D X:0140 Y:00A3 P:envmxdizc
$9D/DAFA A9 9C 00    LDA #$009C              A:042D X:0140 Y:00A3 P:envmxdizc
$9D/DAFD 85 0E       STA $0E    [$00:000E]   A:009C X:0140 Y:00A3 P:envmxdizc
$9D/DAFF A9 F0 81    LDA #$81F0              A:009C X:0140 Y:00A3 P:envmxdizc
$9D/DB02 85 0C       STA $0C    [$00:000C]   A:81F0 X:0140 Y:00A3 P:eNvmxdizc
$9D/DB04 A2 20 01    LDX #$0120              A:81F0 X:0140 Y:00A3 P:eNvmxdizc
$9D/DB07 8A          TXA                     A:81F0 X:0120 Y:00A3 P:envmxdizc
$9D/DB08 4A          LSR A                   A:0120 X:0120 Y:00A3 P:envmxdizc
$9D/DB09 4A          LSR A                   A:0090 X:0120 Y:00A3 P:envmxdizc
$9D/DB0A 4A          LSR A                   A:0048 X:0120 Y:00A3 P:envmxdizc
$9D/DB0B 4A          LSR A                   A:0024 X:0120 Y:00A3 P:envmxdizc
$9D/DB0C 4A          LSR A                   A:0012 X:0120 Y:00A3 P:envmxdizc
$9D/DB0D 18          CLC                     A:0009 X:0120 Y:00A3 P:envmxdizc
$9D/DB0E 6F C4 35 7E ADC $7E35C4[$7E:35C4]   A:0009 X:0120 Y:00A3 P:envmxdizc
$9D/DB12 8F C4 35 7E STA $7E35C4[$7E:35C4]   A:0436 X:0120 Y:00A3 P:envmxdizc

$9D/DB16 A0 FF FF    LDY #$FFFF              A:0436 X:0120 Y:00A3 P:envmxdizc
$9D/DB19 22 1F 89 80 JSL $80891F[$80:891F]   A:0436 X:0120 Y:FFFF P:eNvmxdizc
$9D/DB1D AF C4 35 7E LDA $7E35C4[$7E:35C4]   A:0093 X:0120 Y:0093 P:envmxdizc
$9D/DB21 38          SEC                     A:0436 X:0120 Y:0093 P:envmxdizc
$9D/DB22 E9 00 01    SBC #$0100              A:0436 X:0120 Y:0093 P:envmxdizC
$9D/DB25 8F D2 35 7E STA $7E35D2[$7E:35D2]   A:0336 X:0120 Y:0093 P:envmxdizC
$9D/DB29 A9 95 00    LDA #$0095              A:0336 X:0120 Y:0093 P:envmxdizC
$9D/DB2C 85 0E       STA $0E    [$00:000E]   A:0095 X:0120 Y:0093 P:envmxdizC
$9D/DB2E A9 AE F1    LDA #$F1AE              A:0095 X:0120 Y:0093 P:envmxdizC
$9D/DB31 85 0C       STA $0C    [$00:000C]   A:F1AE X:0120 Y:0093 P:eNvmxdizC
$9D/DB33 A9 7F 00    LDA #$007F              A:F1AE X:0120 Y:0093 P:eNvmxdizC
$9D/DB36 85 12       STA $12    [$00:0012]   A:007F X:0120 Y:0093 P:envmxdizC
$9D/DB38 A9 00 00    LDA #$0000              A:007F X:0120 Y:0093 P:envmxdizC
$9D/DB3B 85 10       STA $10    [$00:0010]   A:0000 X:0120 Y:0093 P:envmxdiZC
$9D/DB3D 22 73 C3 80 JSL $80C373[$80:C373]   A:0000 X:0120 Y:0093 P:envmxdiZC
$9D/DB41 8A          TXA                     A:0000 X:0D00 Y:0718 P:envmxdiZC
$9D/DB42 4A          LSR A                   A:0D00 X:0D00 Y:0718 P:envmxdizC
$9D/DB43 4A          LSR A                   A:0680 X:0D00 Y:0718 P:envmxdizc
$9D/DB44 4A          LSR A                   A:0340 X:0D00 Y:0718 P:envmxdizc
$9D/DB45 4A          LSR A                   A:01A0 X:0D00 Y:0718 P:envmxdizc
$9D/DB46 4A          LSR A                   A:00D0 X:0D00 Y:0718 P:envmxdizc
$9D/DB47 8F B4 35 7E STA $7E35B4[$7E:35B4]   A:0068 X:0D00 Y:0718 P:envmxdizc

$9D/DB4B A9 7F 00    LDA #$007F              A:0068 X:0D00 Y:0718 P:envmxdizc
$9D/DB4E 85 0E       STA $0E    [$00:000E]   A:007F X:0D00 Y:0718 P:envmxdizc
$9D/DB50 A9 00 00    LDA #$0000              A:007F X:0D00 Y:0718 P:envmxdizc
$9D/DB53 85 0C       STA $0C    [$00:000C]   A:0000 X:0D00 Y:0718 P:envmxdiZc
$9D/DB55 A0 00 10    LDY #$1000              A:0000 X:0D00 Y:0718 P:envmxdiZc
$9D/DB58 22 1F 89 80 JSL $80891F[$80:891F]   A:0000 X:0D00 Y:1000 P:envmxdizc
$9D/DB5C A9 9A 00    LDA #$009A              A:1680 X:0D00 Y:1680 P:envmxdizc
$9D/DB5F 85 0E       STA $0E    [$00:000E]   A:009A X:0D00 Y:1680 P:envmxdizc
$9D/DB61 A9 95 BA    LDA #$BA95              A:009A X:0D00 Y:1680 P:envmxdizc
$9D/DB64 85 0C       STA $0C    [$00:000C]   A:BA95 X:0D00 Y:1680 P:eNvmxdizc
$9D/DB66 A9 7F 00    LDA #$007F              A:BA95 X:0D00 Y:1680 P:eNvmxdizc
$9D/DB69 85 12       STA $12    [$00:0012]   A:007F X:0D00 Y:1680 P:envmxdizc
$9D/DB6B A9 00 00    LDA #$0000              A:007F X:0D00 Y:1680 P:envmxdizc
$9D/DB6E 85 10       STA $10    [$00:0010]   A:0000 X:0D00 Y:1680 P:envmxdiZc

$9D/DB70 22 73 C3 80 JSL $80C373[$80:C373]   A:0000 X:0D00 Y:1680 P:envmxdiZc
$9D/DB74 A9 7F 00    LDA #$007F              A:0000 X:0706 Y:0160 P:envmxdiZc
$9D/DB77 85 0E       STA $0E    [$00:000E]   A:007F X:0706 Y:0160 P:envmxdizc
$9D/DB79 A9 00 00    LDA #$0000              A:007F X:0706 Y:0160 P:envmxdizc
$9D/DB7C 85 0C       STA $0C    [$00:000C]   A:0000 X:0706 Y:0160 P:envmxdiZc
$9D/DB7E A9 7F 00    LDA #$007F              A:0000 X:0706 Y:0160 P:envmxdiZc
$9D/DB81 85 12       STA $12    [$00:0012]   A:007F X:0706 Y:0160 P:envmxdizc
$9D/DB83 A9 06 77    LDA #$7706              A:007F X:0706 Y:0160 P:envmxdizc
$9D/DB86 85 10       STA $10    [$00:0010]   A:7706 X:0706 Y:0160 P:envmxdizc
$9D/DB88 A9 07 00    LDA #$0007              A:7706 X:0706 Y:0160 P:envmxdizc
$9D/DB8B 85 24       STA $24    [$00:0024]   A:0007 X:0706 Y:0160 P:envmxdizc
$9D/DB8D 85 18       STA $18    [$00:0018]   A:0007 X:0706 Y:0160 P:envmxdizc
$9D/DB8F 85 1A       STA $1A    [$00:001A]   A:0007 X:0706 Y:0160 P:envmxdizc
$9D/DB91 A9 01 00    LDA #$0001              A:0007 X:0706 Y:0160 P:envmxdizc
$9D/DB94 85 20       STA $20    [$00:0020]   A:0001 X:0706 Y:0160 P:envmxdizc
$9D/DB96 85 22       STA $22    [$00:0022]   A:0001 X:0706 Y:0160 P:envmxdizc

$9D/DB98 A2 00 00    LDX #$0000              A:0001 X:0706 Y:0160 P:envmxdizc
$9D/DB9B A0 00 00    LDY #$0000              A:0001 X:0000 Y:0160 P:envmxdiZc
$9D/DB9E A9 00 0C    LDA #$0C00              A:0001 X:0000 Y:0000 P:envmxdiZc
$9D/DBA1 22 7C 8E 80 JSL $808E7C[$80:8E7C]   A:0C00 X:0000 Y:0000 P:envmxdizc
$9D/DBA5 A9 07 00    LDA #$0007              A:0208 X:0000 Y:000E P:envmxdiZc
$9D/DBA8 8F 00 77 7F STA $7F7700[$7F:7700]   A:0007 X:0000 Y:000E P:envmxdizc
$9D/DBAC 8F 02 77 7F STA $7F7702[$7F:7702]   A:0007 X:0000 Y:000E P:envmxdizc

$9D/DBB0 A9 7F 00    LDA #$007F              A:0007 X:0000 Y:000E P:envmxdizc
$9D/DBB3 85 0E       STA $0E    [$00:000E]   A:007F X:0000 Y:000E P:envmxdizc
$9D/DBB5 A9 00 00    LDA #$0000              A:007F X:0000 Y:000E P:envmxdizc
$9D/DBB8 85 0C       STA $0C    [$00:000C]   A:0000 X:0000 Y:000E P:envmxdiZc
$9D/DBBA A9 7E 00    LDA #$007E              A:0000 X:0000 Y:000E P:envmxdiZc
$9D/DBBD 85 12       STA $12    [$00:0012]   A:007E X:0000 Y:000E P:envmxdizc
$9D/DBBF A9 F4 3F    LDA #$3FF4              A:007E X:0000 Y:000E P:envmxdizc
$9D/DBC2 85 10       STA $10    [$00:0010]   A:3FF4 X:0000 Y:000E P:envmxdizc
$9D/DBC4 A9 20 00    LDA #$0020              A:3FF4 X:0000 Y:000E P:envmxdizc
$9D/DBC7 85 24       STA $24    [$00:0024]   A:0020 X:0000 Y:000E P:envmxdizc
$9D/DBC9 A9 1C 00    LDA #$001C              A:0020 X:0000 Y:000E P:envmxdizc
$9D/DBCC 85 26       STA $26    [$00:0026]   A:001C X:0000 Y:000E P:envmxdizc
$9D/DBCE A2 00 00    LDX #$0000              A:001C X:0000 Y:000E P:envmxdizc
$9D/DBD1 A0 00 00    LDY #$0000              A:001C X:0000 Y:000E P:envmxdiZc
$9D/DBD4 AF AC 35 7E LDA $7E35AC[$7E:35AC]   A:001C X:0000 Y:0000 P:envmxdiZc
$9D/DBD8 18          CLC                     A:0000 X:0000 Y:0000 P:envmxdiZc
$9D/DBD9 69 00 0C    ADC #$0C00              A:0000 X:0000 Y:0000 P:envmxdiZc
$9D/DBDC 22 2D 8E 80 JSL $808E2D[$80:8E2D]   A:0C00 X:0000 Y:0000 P:envmxdizc

$9D/DBE0 A9 9A 00    LDA #$009A              A:0706 X:0000 Y:0040 P:envmxdiZc
$9D/DBE3 85 0E       STA $0E    [$00:000E]   A:009A X:0000 Y:0040 P:envmxdizc
$9D/DBE5 A9 57 DE    LDA #$DE57              A:009A X:0000 Y:0040 P:envmxdizc
$9D/DBE8 85 0C       STA $0C    [$00:000C]   A:DE57 X:0000 Y:0040 P:eNvmxdizc
$9D/DBEA A9 7F 00    LDA #$007F              A:DE57 X:0000 Y:0040 P:eNvmxdizc
$9D/DBED 85 12       STA $12    [$00:0012]   A:007F X:0000 Y:0040 P:envmxdizc
$9D/DBEF A9 00 00    LDA #$0000              A:007F X:0000 Y:0040 P:envmxdizc
$9D/DBF2 85 10       STA $10    [$00:0010]   A:0000 X:0000 Y:0040 P:envmxdiZc
$9D/DBF4 22 73 C3 80 JSL $80C373[$80:C373]   A:0000 X:0000 Y:0040 P:envmxdiZc
$9D/DBF8 A9 7F 00    LDA #$007F              A:0000 X:0100 Y:0089 P:envmxdiZc
$9D/DBFB 85 0E       STA $0E    [$00:000E]   A:007F X:0100 Y:0089 P:envmxdizc
$9D/DBFD A9 00 00    LDA #$0000              A:007F X:0100 Y:0089 P:envmxdizc
$9D/DC00 85 0C       STA $0C    [$00:000C]   A:0000 X:0100 Y:0089 P:envmxdiZc
$9D/DC02 A0 00 00    LDY #$0000              A:0000 X:0100 Y:0089 P:envmxdiZc
$9D/DC05 A2 30 00    LDX #$0030              A:0000 X:0100 Y:0000 P:envmxdiZc
$9D/DC08 22 BD E0 9D JSL $9DE0BD[$9D:E0BD]   A:0000 X:0030 Y:0000 P:envmxdizc

$9D/DC0C AF 54 34 7E LDA $7E3454[$7E:3454]   A:1000 X:0060 Y:0060 P:envmxdiZc
$9D/DC10 C9 21 00    CMP #$0021              A:000B X:0060 Y:0060 P:envmxdizc
$9D/DC13 B0 03       BCS $03    [$DC18]      A:000B X:0060 Y:0060 P:eNvmxdizc
$9D/DC15 4C F3 DC    JMP $DCF3  [$9D:DCF3]   A:000B X:0060 Y:0060 P:eNvmxdizc

// ...

$9D/DCF3 E2 20       SEP #$20                A:000B X:0060 Y:0060 P:eNvmxdizc
$9D/DCF5 9C 21 21    STZ $2121  [$9F:2121]   A:000B X:0060 Y:0060 P:eNvMxdizc
$9D/DCF8 9C 22 21    STZ $2122  [$9F:2122]   A:000B X:0060 Y:0060 P:eNvMxdizc
$9D/DCFB 9C 22 21    STZ $2122  [$9F:2122]   A:000B X:0060 Y:0060 P:eNvMxdizc
$9D/DCFE C2 20       REP #$20                A:000B X:0060 Y:0060 P:eNvMxdizc
$9D/DD00 A9 9A 00    LDA #$009A              A:000B X:0060 Y:0060 P:eNvmxdizc
$9D/DD03 85 0E       STA $0E    [$00:000E]   A:009A X:0060 Y:0060 P:envmxdizc
$9D/DD05 A9 21 E1    LDA #$E121              A:009A X:0060 Y:0060 P:envmxdizc
$9D/DD08 85 0C       STA $0C    [$00:000C]   A:E121 X:0060 Y:0060 P:eNvmxdizc
$9D/DD0A A9 7F 00    LDA #$007F              A:E121 X:0060 Y:0060 P:eNvmxdizc
$9D/DD0D 85 12       STA $12    [$00:0012]   A:007F X:0060 Y:0060 P:envmxdizc
$9D/DD0F A9 00 00    LDA #$0000              A:007F X:0060 Y:0060 P:envmxdizc
$9D/DD12 85 10       STA $10    [$00:0010]   A:0000 X:0060 Y:0060 P:envmxdiZc
$9D/DD14 22 73 C3 80 JSL $80C373[$80:C373]   A:0000 X:0060 Y:0060 P:envmxdiZc
$9D/DD18 A9 7F 00    LDA #$007F              A:0000 X:0100 Y:0047 P:envmxdiZc
$9D/DD1B 85 0E       STA $0E    [$00:000E]   A:007F X:0100 Y:0047 P:envmxdizc
$9D/DD1D A9 00 00    LDA #$0000              A:007F X:0100 Y:0047 P:envmxdizc
$9D/DD20 85 0C       STA $0C    [$00:000C]   A:0000 X:0100 Y:0047 P:envmxdiZc
$9D/DD22 A0 30 00    LDY #$0030              A:0000 X:0100 Y:0047 P:envmxdiZc
$9D/DD25 A2 20 00    LDX #$0020              A:0000 X:0100 Y:0030 P:envmxdizc
$9D/DD28 22 BD E0 9D JSL $9DE0BD[$9D:E0BD]   A:0000 X:0020 Y:0030 P:envmxdizc
$9D/DD2C AF BE 35 7E LDA $7E35BE[$7E:35BE]   A:050E X:00A0 Y:0040 P:envmxdiZc
$9D/DD30 18          CLC                     A:0423 X:00A0 Y:0040 P:envmxdizc
$9D/DD31 69 00 14    ADC #$1400              A:0423 X:00A0 Y:0040 P:envmxdizc
$9D/DD34 8F BE 35 7E STA $7E35BE[$7E:35BE]   A:1823 X:00A0 Y:0040 P:envmxdizc
$9D/DD38 AF C0 35 7E LDA $7E35C0[$7E:35C0]   A:1823 X:00A0 Y:0040 P:envmxdizc
$9D/DD3C 18          CLC                     A:042D X:00A0 Y:0040 P:envmxdizc
$9D/DD3D 69 00 10    ADC #$1000              A:042D X:00A0 Y:0040 P:envmxdizc
$9D/DD40 8F C0 35 7E STA $7E35C0[$7E:35C0]   A:142D X:00A0 Y:0040 P:envmxdizc
$9D/DD44 AD 98 1C    LDA $1C98  [$9F:1C98]   A:142D X:00A0 Y:0040 P:envmxdizc
$9D/DD47 C9 18 00    CMP #$0018              A:000B X:00A0 Y:0040 P:envmxdizc
$9D/DD4A B0 02       BCS $02    [$DD4E]      A:000B X:00A0 Y:0040 P:eNvmxdizc
$9D/DD4C 64 91       STZ $91    [$00:0091]   A:000B X:00A0 Y:0040 P:eNvmxdizc
$9D/DD4E AD 9A 1C    LDA $1C9A  [$9F:1C9A]   A:000B X:00A0 Y:0040 P:eNvmxdizc
$9D/DD51 C9 18 00    CMP #$0018              A:000A X:00A0 Y:0040 P:envmxdizc
$9D/DD54 B0 05       BCS $05    [$DD5B]      A:000A X:00A0 Y:0040 P:eNvmxdizc
$9D/DD56 A9 02 00    LDA #$0002              A:000A X:00A0 Y:0040 P:eNvmxdizc
$9D/DD59 85 91       STA $91    [$00:0091]   A:0002 X:00A0 Y:0040 P:envmxdizc
$9D/DD5B A9 9A 00    LDA #$009A              A:0002 X:00A0 Y:0040 P:envmxdizc
$9D/DD5E 85 0E       STA $0E    [$00:000E]   A:009A X:00A0 Y:0040 P:envmxdizc
$9D/DD60 A9 B6 D3    LDA #$D3B6              A:009A X:00A0 Y:0040 P:envmxdizc
$9D/DD63 85 0C       STA $0C    [$00:000C]   A:D3B6 X:00A0 Y:0040 P:eNvmxdizc
$9D/DD65 A0 80 00    LDY #$0080              A:D3B6 X:00A0 Y:0040 P:eNvmxdizc
$9D/DD68 A2 10 00    LDX #$0010              A:D3B6 X:00A0 Y:0080 P:envmxdizc
$9D/DD6B 22 BD E0 9D JSL $9DE0BD[$9D:E0BD]   A:D3B6 X:0010 Y:0080 P:envmxdizc
$9D/DD6F A9 9A 00    LDA #$009A              A:741F X:0120 Y:0020 P:envmxdiZc
$9D/DD72 85 0E       STA $0E    [$00:000E]   A:009A X:0120 Y:0020 P:envmxdizc
$9D/DD74 A9 5C F1    LDA #$F15C              A:009A X:0120 Y:0020 P:envmxdizc
$9D/DD77 85 0C       STA $0C    [$00:000C]   A:F15C X:0120 Y:0020 P:eNvmxdizc
$9D/DD79 A0 C0 00    LDY #$00C0              A:F15C X:0120 Y:0020 P:eNvmxdizc
$9D/DD7C A2 10 00    LDX #$0010              A:F15C X:0120 Y:00C0 P:envmxdizc
$9D/DD7F 22 BD E0 9D JSL $9DE0BD[$9D:E0BD]   A:F15C X:0010 Y:00C0 P:envmxdizc
$9D/DD83 22 77 D9 9D JSL $9DD977[$9D:D977]   A:FFFF X:01A0 Y:0020 P:envmxdiZc
$9D/DD87 22 4D D9 9D JSL $9DD94D[$9D:D94D]   A:029F X:0800 Y:0000 P:envmxdiZC
$9D/DD8B 22 83 85 80 JSL $808583[$80:8583]   A:0800 X:0800 Y:0800 P:envmxdizc
$9D/DD8F A2 00 00    LDX #$0000              A:0800 X:0800 Y:0800 P:envmxdizc
$9D/DD92 A0 80 01    LDY #$0180              A:0800 X:0000 Y:0800 P:envmxdiZc
$9D/DD95 A9 00 60    LDA #$6000              A:0800 X:0000 Y:0180 P:envmxdizc
$9D/DD98 22 E1 AE 80 JSL $80AEE1[$80:AEE1]   A:6000 X:0000 Y:0180 P:envmxdizc
$9D/DD9C 22 7B AF 80 JSL $80AF7B[$80:AF7B]   A:2101 X:2100 Y:2320 P:eNvmxdizC
$9D/DDA0 22 73 DF 9D JSL $9DDF73[$9D:DF73]   A:0000 X:0000 Y:2320 P:eNvmxdizc
$9D/DDA4 A9 68 35    LDA #$3568              A:77E8 X:0000 Y:FFFE P:envmxdiZc
$9D/DDA7 8F 00 75 7F STA $7F7500[$7F:7500]   A:3568 X:0000 Y:FFFE P:envmxdizc

$9D/DDAB 22 B3 DD 9D JSL $9DDDB3[$9D:DDB3]   A:3568 X:0000 Y:FFFE P:envmxdizc	// Call DecompressWrapper2() - This owns calling DecompressActual2 down the chain.

// Sadly, this code diverges a lot from the loader code during GAME SETUP. 
// It would be safer to copy overtop here rather than replace the code outright.

$9D/DDAF 68          PLA                     A:0002 X:0300 Y:05A0 P:envmxdizc
$9D/DDB0 85 91       STA $91    [$00:0091]   A:0000 X:0300 Y:05A0 P:envmxdiZc
$9D/DDB2 6B          RTL                     A:0000 X:0300 Y:05A0 P:envmxdiZc

////////////////////////////////////////////////////////////////////////////////////////

$9B/86BD 22 AD D9 9D JSL $9DD9AD[$9D:D9AD]   A:7500 X:0002 Y:0000 P:envmxdizc	; Call DecompressForEditLines() (9DD9AD)
$9B/86C1 22 0F 89 9B JSL $9B890F[$9B:890F]   A:0000 X:0300 Y:05A0 P:envmxdiZc
$9B/86C5 A9 05 00    LDA #$0005              A:0177 X:0002 Y:0005 P:envmxdizc
$9B/86C8 8D 9F 1D    STA $1D9F  [$9F:1D9F]   A:0005 X:0002 Y:0005 P:envmxdizc
$9B/86CB 3A          DEC A                   A:0005 X:0002 Y:0005 P:envmxdizc
$9B/86CC 8D 8B 1D    STA $1D8B  [$9F:1D8B]   A:0004 X:0002 Y:0005 P:envmxdizc
$9B/86CF 22 10 88 9B JSL $9B8810[$9B:8810]   A:0004 X:0002 Y:0005 P:envmxdizc
$9B/86D3 A9 05 00    LDA #$0005              A:7500 X:0002 Y:0000 P:envmxdizc

////////////////////////////////////////////////////////////////////////////////////////

// void EditLinesSpinUntilCond()
// Probably waiting on Vblank

$9F/9708 22 83 85 80 JSL $808583[$80:8583]   A:8001 X:7C00 Y:0000 P:envmxdizc	; Call SpinUntilCond()
$9F/970C AD D3 0A    LDA $0AD3  [$9F:0AD3]   A:8001 X:7C00 Y:0000 P:eNvmxdizc
$9F/970F D0 F7       BNE $F7    [$9708]      A:0000 X:7C00 Y:0000 P:envmxdiZc
$9F/9711 6B          RTL                     A:0000 X:7C00 Y:0000 P:envmxdiZc

////////////////////////////////////////////////////////////////////////////////////////

// void EditLinesDecompressHelper()
// This doesn't call DecompressActual2- that happens later.

$80/86A9 AD D3 0A    LDA $0AD3  [$9F:0AD3]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$80/86AC F0 08       BEQ $08    [$86B6]      A:0000 X:7C00 Y:0000 P:envmxdiZc

$80/86B6 22 84 86 80 JSL $808684[$80:8684]   A:0000 X:7C00 Y:0000 P:envmxdiZc	; Call EditLinesDecompressHelper2()

$80/86BA 4C D5 86    JMP $86D5  [$80:86D5]   A:0130 X:7C00 Y:0000 P:envmxdizc
$80/86D5 48          PHA                     A:0130 X:7C00 Y:0000 P:envmxdizc
$80/86D6 9C D6 07    STZ $07D6  [$9F:07D6]   A:0130 X:7C00 Y:0000 P:envmxdizc
$80/86D9 9C DA 07    STZ $07DA  [$9F:07DA]   A:0130 X:7C00 Y:0000 P:envmxdizc
$80/86DC 9C DE 07    STZ $07DE  [$9F:07DE]   A:0130 X:7C00 Y:0000 P:envmxdizc
$80/86DF A9 FF FF    LDA #$FFFF              A:0130 X:7C00 Y:0000 P:envmxdizc
$80/86E2 8D D8 07    STA $07D8  [$9F:07D8]   A:FFFF X:7C00 Y:0000 P:eNvmxdizc
$80/86E5 8D DC 07    STA $07DC  [$9F:07DC]   A:FFFF X:7C00 Y:0000 P:eNvmxdizc
$80/86E8 8D E0 07    STA $07E0  [$9F:07E0]   A:FFFF X:7C00 Y:0000 P:eNvmxdizc
$80/86EB A9 F0 FF    LDA #$FFF0              A:FFFF X:7C00 Y:0000 P:eNvmxdizc
$80/86EE 85 30       STA $30    [$00:0030]   A:FFF0 X:7C00 Y:0000 P:eNvmxdizc
$80/86F0 85 2C       STA $2C    [$00:002C]   A:FFF0 X:7C00 Y:0000 P:eNvmxdizc
$80/86F2 A9 E0 00    LDA #$00E0              A:FFF0 X:7C00 Y:0000 P:eNvmxdizc
$80/86F5 85 32       STA $32    [$00:0032]   A:00E0 X:7C00 Y:0000 P:envmxdizc
$80/86F7 A9 00 01    LDA #$0100              A:00E0 X:7C00 Y:0000 P:envmxdizc
$80/86FA 85 2E       STA $2E    [$00:002E]   A:0100 X:7C00 Y:0000 P:envmxdizc
$80/86FC E2 20       SEP #$20                A:0100 X:7C00 Y:0000 P:envmxdizc
$80/86FE 9C 31 21    STZ $2131  [$9F:2131]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8701 9C 30 21    STZ $2130  [$9F:2130]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8704 9C 2E 21    STZ $212E  [$9F:212E]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8707 9C 06 21    STZ $2106  [$9F:2106]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/870A 9C 33 21    STZ $2133  [$9F:2133]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/870D 9C 2D 21    STZ $212D  [$9F:212D]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8710 9C 2F 21    STZ $212F  [$9F:212F]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8713 9C 23 21    STZ $2123  [$9F:2123]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8716 9C 24 21    STZ $2124  [$9F:2124]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8719 9C 25 21    STZ $2125  [$9F:2125]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/871C 9C E2 07    STZ $07E2  [$9F:07E2]   A:0100 X:7C00 Y:0000 P:envMxdizc
$80/871F C2 20       REP #$20                A:0100 X:7C00 Y:0000 P:envMxdizc
$80/8721 68          PLA                     A:0100 X:7C00 Y:0000 P:envmxdizc
$80/8722 6B          RTL                     A:0130 X:7C00 Y:0000 P:envmxdizc

////////////////////////////////////////////////////////////////////////////////////////

// void EditLinesDecompressHelper2() - 808684
$80/86B6 22 84 86 80 JSL $808684[$80:8684]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$80/8684 22 83 85 80 JSL $808583[$80:8583]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$80/8688 22 07 85 80 JSL $808507[$80:8507]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$80/868C 22 4F 85 80 JSL $80854F[$80:854F]   A:0000 X:7C00 Y:0000 P:envmxdiZc
$80/8690 22 69 85 80 JSL $808569[$80:8569]   A:0000 X:7C00 Y:0000 P:envmxdiZc

$80/8694 E2 20       SEP #$20                A:0000 X:7C00 Y:0000 P:envmxdiZc
$80/8696 A9 8F       LDA #$8F                A:0000 X:7C00 Y:0000 P:envMxdiZc
$80/8698 8D 62 07    STA $0762  [$9F:0762]   A:008F X:7C00 Y:0000 P:eNvMxdizc

$80/869B 22 83 85 80 JSL $808583[$80:8583]   A:008F X:7C00 Y:0000 P:eNvMxdizc

$80/869F 9C 0C 42    STZ $420C  [$9F:420C]   A:008F X:7C00 Y:0000 P:eNvMxdizc
$80/86A2 C2 20       REP #$20                A:008F X:7C00 Y:0000 P:eNvMxdizc
$80/86A4 A5 3A       LDA $3A    [$00:003A]   A:008F X:7C00 Y:0000 P:eNvmxdizc
$80/86A6 85 3C       STA $3C    [$00:003C]   A:0330 X:7C00 Y:0000 P:envmxdizc
$80/86A8 6B          RTL                     A:0330 X:7C00 Y:0000 P:envmxdizc


//////////////////////

void DecompressWrapper() - 9DC9A8

...

$9D/CA1B 22 C2 85 9B JSL $9B85C2[$9B:85C2]   A:4444 X:0000 Y:0490 P:eNvmxdizc	// Call DecompressActual2()

$9D/CA1F 22 1C CB 9D JSL $9DCB1C[$9D:CB1C]   A:4444 X:0000 Y:0490 P:eNvmxdizc	// Immediately after calling DecompressActual2 during EDIT LINES


$9D/CA23 D0 31       BNE $31    [$CA56]      A:0001 X:0000 Y:000A P:envmxdizc
$9D/CA56 A9 92 00    LDA #$0092              A:0001 X:0000 Y:000A P:envmxdizc
$9D/CA59 85 0E       STA $0E    [$00:000E]   A:0092 X:0000 Y:000A P:envmxdizc
$9D/CA5B A9 B0 FF    LDA #$FFB0              A:0092 X:0000 Y:000A P:envmxdizc
$9D/CA5E 85 0C       STA $0C    [$00:000C]   A:FFB0 X:0000 Y:000A P:eNvmxdizc
$9D/CA60 A9 7E 00    LDA #$007E              A:FFB0 X:0000 Y:000A P:eNvmxdizc
$9D/CA63 85 12       STA $12    [$00:0012]   A:007E X:0000 Y:000A P:envmxdizc
$9D/CA65 A9 F4 37    LDA #$37F4              A:007E X:0000 Y:000A P:envmxdizc
$9D/CA68 85 10       STA $10    [$00:0010]   A:37F4 X:0000 Y:000A P:envmxdizc
$9D/CA6A A9 20 00    LDA #$0020              A:37F4 X:0000 Y:000A P:envmxdizc
$9D/CA6D 85 24       STA $24    [$00:0024]   A:0020 X:0000 Y:000A P:envmxdizc
$9D/CA6F AF D2 35 7E LDA $7E35D2[$7E:35D2]   A:0020 X:0000 Y:000A P:envmxdizc
$9D/CA73 18          CLC                     A:0336 X:0000 Y:000A P:envmxdizc
$9D/CA74 6D 87 1E    ADC $1E87  [$9F:1E87]   A:0336 X:0000 Y:000A P:envmxdizc
$9D/CA77 A4 91       LDY $91    [$00:0091]   A:2736 X:0000 Y:000A P:envmxdizc
$9D/CA79 F0 04       BEQ $04    [$CA7F]      A:2736 X:0000 Y:0002 P:envmxdizc
$9D/CA7B 18          CLC                     A:2736 X:0000 Y:0002 P:envmxdizc
$9D/CA7C 69 30 00    ADC #$0030              A:2736 X:0000 Y:0002 P:envmxdizc
$9D/CA7F FA          PLX                     A:2766 X:0000 Y:0002 P:envmxdizc
$9D/CA80 7A          PLY                     A:2766 X:0018 Y:0002 P:envmxdizc
$9D/CA81 22 B7 CA 9D JSL $9DCAB7[$9D:CAB7]   A:2766 X:0018 Y:0001 P:envmxdizc
$9D/CA85 A4 91       LDY $91    [$00:0091]   A:FFFE X:0000 Y:000C P:envmxdiZc
$9D/CA87 B9 98 1C    LDA $1C98,y[$9F:1C9A]   A:FFFE X:0000 Y:0002 P:envmxdizc
$9D/CA8A 0A          ASL A                   A:000A X:0000 Y:0002 P:envmxdizc
$9D/CA8B 0A          ASL A                   A:0014 X:0000 Y:0002 P:envmxdizc
$9D/CA8C 18          CLC                     A:0028 X:0000 Y:0002 P:envmxdizc
$9D/CA8D 69 B3 D8    ADC #$D8B3              A:0028 X:0000 Y:0002 P:envmxdizc
$9D/CA90 85 8D       STA $8D    [$00:008D]   A:D8DB X:0000 Y:0002 P:eNvmxdizc
$9D/CA92 A9 9D 00    LDA #$009D              A:D8DB X:0000 Y:0002 P:eNvmxdizc
$9D/CA95 85 8F       STA $8F    [$00:008F]   A:009D X:0000 Y:0002 P:envmxdizc
$9D/CA97 A7 8D       LDA [$8D]  [$9D:D8DB]   A:009D X:0000 Y:0002 P:envmxdizc
$9D/CA99 85 0C       STA $0C    [$00:000C]   A:EB7C X:0000 Y:0002 P:eNvmxdizc
$9D/CA9B E6 8D       INC $8D    [$00:008D]   A:EB7C X:0000 Y:0002 P:eNvmxdizc
$9D/CA9D E6 8D       INC $8D    [$00:008D]   A:EB7C X:0000 Y:0002 P:eNvmxdizc
$9D/CA9F A7 8D       LDA [$8D]  [$9D:D8DD]   A:EB7C X:0000 Y:0002 P:eNvmxdizc
$9D/CAA1 85 0E       STA $0E    [$00:000E]   A:009A X:0000 Y:0002 P:envmxdizc
$9D/CAA3 A5 91       LDA $91    [$00:0091]   A:009A X:0000 Y:0002 P:envmxdizc
$9D/CAA5 F0 05       BEQ $05    [$CAAC]      A:0002 X:0000 Y:0002 P:envmxdizc
$9D/CAA7 A0 10 00    LDY #$0010              A:0002 X:0000 Y:0002 P:envmxdizc
$9D/CAAA 80 03       BRA $03    [$CAAF]      A:0002 X:0000 Y:0010 P:envmxdizc
$9D/CAAF A2 10 00    LDX #$0010              A:0002 X:0000 Y:0010 P:envmxdizc
$9D/CAB2 22 BD E0 9D JSL $9DE0BD[$9D:E0BD]   A:0002 X:0010 Y:0010 P:envmxdizc
$9D/CAB6 6B          RTL                     A:77BD X:0040 Y:0020 P:envmxdiZc

//////////////////////

// void DecompressWrapper2() - 9DDDB3

...

$9D/DE7D 22 A8 C9 9D JSL $9DC9A8[$9D:C9A8]   A:7780 X:0040 Y:0020 P:envmxdizc	// Call DecompressWrapper()
$9D/DE81 A9 7F 00    LDA #$007F              A:77BD X:0040 Y:0020 P:envmxdiZc
$9D/DE84 85 0E       STA $0E    [$00:000E]   A:007F X:0040 Y:0020 P:envmxdizc
$9D/DE86 A9 80 77    LDA #$7780              A:007F X:0040 Y:0020 P:envmxdizc
$9D/DE89 85 0C       STA $0C    [$00:000C]   A:7780 X:0040 Y:0020 P:envmxdizc
$9D/DE8B A9 7E 00    LDA #$007E              A:7780 X:0040 Y:0020 P:envmxdizc
$9D/DE8E 85 12       STA $12    [$00:0012]   A:007E X:0040 Y:0020 P:envmxdizc
$9D/DE90 A9 F4 3F    LDA #$3FF4              A:007E X:0040 Y:0020 P:envmxdizc
$9D/DE93 85 10       STA $10    [$00:0010]   A:3FF4 X:0040 Y:0020 P:envmxdizc
$9D/DE95 A9 20 00    LDA #$0020              A:3FF4 X:0040 Y:0020 P:envmxdizc
$9D/DE98 85 24       STA $24    [$00:0024]   A:0020 X:0040 Y:0020 P:envmxdizc
$9D/DE9A A2 18 00    LDX #$0018              A:0020 X:0040 Y:0020 P:envmxdizc
$9D/DE9D A0 01 00    LDY #$0001              A:0020 X:0018 Y:0020 P:envmxdizc
$9D/DEA0 A9 00 00    LDA #$0000              A:0020 X:0018 Y:0001 P:envmxdizc
$9D/DEA3 22 2D 8E 80 JSL $808E2D[$80:8E2D]   A:0000 X:0018 Y:0001 P:envmxdiZc
$9D/DEA7 80 64       BRA $64    [$DF0D]      A:77E8 X:0000 Y:000E P:envmxdiZc
$9D/DF0D AF 78 34 7E LDA $7E3478[$7E:3478]   A:77E8 X:0000 Y:000E P:envmxdiZc
$9D/DF11 D0 5C       BNE $5C    [$DF6F]      A:0000 X:0000 Y:000E P:envmxdiZc
$9D/DF13 AF 54 34 7E LDA $7E3454[$7E:3454]   A:0000 X:0000 Y:000E P:envmxdiZc
$9D/DF17 30 06       BMI $06    [$DF1F]      A:000B X:0000 Y:000E P:envmxdizc
$9D/DF19 AF 52 34 7E LDA $7E3452[$7E:3452]   A:000B X:0000 Y:000E P:envmxdizc
$9D/DF1D 10 44       BPL $44    [$DF63]      A:FFFF X:0000 Y:000E P:eNvmxdizc
$9D/DF1F A9 7F 00    LDA #$007F              A:FFFF X:0000 Y:000E P:eNvmxdizc
$9D/DF22 85 0E       STA $0E    [$00:000E]   A:007F X:0000 Y:000E P:envmxdizc
$9D/DF24 AF 70 34 7E LDA $7E3470[$7E:3470]   A:007F X:0000 Y:000E P:envmxdizc
$9D/DF28 85 0C       STA $0C    [$00:000C]   A:0000 X:0000 Y:000E P:envmxdiZc
$9D/DF2A AF D2 35 7E LDA $7E35D2[$7E:35D2]   A:0000 X:0000 Y:000E P:envmxdiZc
$9D/DF2E 0A          ASL A                   A:0336 X:0000 Y:000E P:envmxdizc
$9D/DF2F 0A          ASL A                   A:066C X:0000 Y:000E P:envmxdizc
$9D/DF30 0A          ASL A                   A:0CD8 X:0000 Y:000E P:envmxdizc
$9D/DF31 0A          ASL A                   A:19B0 X:0000 Y:000E P:envmxdizc
$9D/DF32 18          CLC                     A:3360 X:0000 Y:000E P:envmxdizc
$9D/DF33 69 00 20    ADC #$2000              A:3360 X:0000 Y:000E P:envmxdizc
$9D/DF36 A8          TAY                     A:5360 X:0000 Y:000E P:envmxdizc
$9D/DF37 A2 00 06    LDX #$0600              A:5360 X:0000 Y:5360 P:envmxdizc
$9D/DF3A 22 83 85 80 JSL $808583[$80:8583]   A:5360 X:0600 Y:5360 P:envmxdizc
$9D/DF3E 22 1F 89 80 JSL $80891F[$80:891F]   A:5360 X:0600 Y:5360 P:envmxdizc
$9D/DF42 A9 7F 00    LDA #$007F              A:5660 X:0600 Y:5660 P:envmxdizc
$9D/DF45 85 0E       STA $0E    [$00:000E]   A:007F X:0600 Y:5660 P:envmxdizc
$9D/DF47 AF 72 34 7E LDA $7E3472[$7E:3472]   A:007F X:0600 Y:5660 P:envmxdizc
$9D/DF4B 85 0C       STA $0C    [$00:000C]   A:6900 X:0600 Y:5660 P:envmxdizc
$9D/DF4D AF D2 35 7E LDA $7E35D2[$7E:35D2]   A:6900 X:0600 Y:5660 P:envmxdizc
$9D/DF51 0A          ASL A                   A:0336 X:0600 Y:5660 P:envmxdizc
$9D/DF52 0A          ASL A                   A:066C X:0600 Y:5660 P:envmxdizc
$9D/DF53 0A          ASL A                   A:0CD8 X:0600 Y:5660 P:envmxdizc
$9D/DF54 0A          ASL A                   A:19B0 X:0600 Y:5660 P:envmxdizc
$9D/DF55 18          CLC                     A:3360 X:0600 Y:5660 P:envmxdizc
$9D/DF56 69 00 23    ADC #$2300              A:3360 X:0600 Y:5660 P:envmxdizc
$9D/DF59 A8          TAY                     A:5660 X:0600 Y:5660 P:envmxdizc
$9D/DF5A A2 00 06    LDX #$0600              A:5660 X:0600 Y:5660 P:envmxdizc
$9D/DF5D 22 1F 89 80 JSL $80891F[$80:891F]   A:5660 X:0600 Y:5660 P:envmxdizc
$9D/DF61 80 04       BRA $04    [$DF67]      A:5960 X:0600 Y:5960 P:envmxdizc
$9D/DF67 22 38 D9 9D JSL $9DD938[$9D:D938]   A:5960 X:0600 Y:5960 P:envmxdizc
$9D/DF6B 22 62 D9 9D JSL $9DD962[$9D:D962]   A:01A0 X:0300 Y:01A0 P:envmxdizc
$9D/DF6F 68          PLA                     A:05A0 X:0300 Y:05A0 P:envmxdizc
$9D/DF70 85 91       STA $91    [$00:0091]   A:0002 X:0300 Y:05A0 P:envmxdizc
$9D/DF72 6B          RTL                     A:0002 X:0300 Y:05A0 P:envmxdizc
