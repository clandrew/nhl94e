//////////

// Detoured out into 9D/C12B - 9D/C12B

// Load the team index, which has been stored at 9F1C98/9F1C9A for home/away.
A4 91                LDY $91
B9 98 1C             LDA $1C98, y[$9F:1C98]

// Multiply by 4 to turn index into an offset
0A                   ASL
0A                   ASL
A8                   TAY

// Put 0xA08200 (or whatever the string table address is) into $8D
A9 A0 00    LDA #$00A0
85 8F       STA $8F
A9 00 82    LDA #$8200
85 8D       STA $8D

// load from 0xA08200, y
B7 8D       LDA [$8D],y     // Low short
48          PHA
98          TYA
1A          INC
1A          INC
A8          TAY
B7 8D       LDA [$8D],y     // High short
85 8F       STA $8F
68          PLA
85 8D       STA $8D

5C 49 C1 9D JMP 9DC149

// 3A ED 9C


// 31 bytes aaagh so close to being patchable on top aaah oh well.

// $9D/C12B A4 91       LDY $91    [$00:0091]   A:0000 X:000C Y:0024 P:envmxdiZc	; $91 == 0 for home, 2 for away. $91 is HomeOrAway
// $9D/C12D A9 9C 00    LDA #$009C              A:0000 X:000C Y:0000 P:envmxdiZc
// $9D/C130 85 8F       STA $8F    [$00:008F]   A:009C X:000C Y:0000 P:envmxdizc
// $9D/C132 B9 DC 1C    LDA $1CDC,y[$9F:1CDC]   A:009C X:000C Y:0000 P:envmxdizc	; Load main pointer table entry for the corresponding team
// $9D/C135 85 8D       STA $8D    [$00:008D]   A:C2DB X:000C Y:0000 P:eNvmxdizc	; E.g., [$8D] == 9CC2DB, the beginning of the team data for Montreal

// $9D/C137 A0 04 00    LDY #$0004              A:C2DB X:000C Y:0000 P:eNvmxdizc	; Seek to load the 4th+5th bytes of metadata, which can jump you to the team strings. E.g., for Montreal, 0279
// $9D/C13A 18          CLC                     A:C2DB X:000C Y:0004 P:envmxdizc
// $9D/C13B 77 8D       ADC [$8D],y[$9C:C2DF]   A:C2DB X:000C Y:0004 P:envmxdizc
// $9D/C13D 85 8D       STA $8D    [$00:008D]   A:C554 X:000C Y:0004 P:eNvmxdizc	; E.g., [$8D] == 9CC554, the beginning of the team strings

// $9D/C13F 18          CLC                     A:C554 X:000C Y:0004 P:eNvmxdizc
// $9D/C140 67 8D       ADC [$8D]  [$9C:C554]   A:C554 X:000C Y:0004 P:eNvmxdizc	; Get location string length so we can skip over it
// $9D/C142 85 8D       STA $8D    [$00:008D]   A:C55E X:000C Y:0004 P:eNvmxdizc
// $9D/C144 18          CLC                     A:C55E X:000C Y:0004 P:eNvmxdizc
// $9D/C145 67 8D       ADC [$8D]  [$9C:C55E]   A:C55E X:000C Y:0004 P:eNvmxdizc	; Get acronym string length so we can skip over it
// $9D/C147 85 8D       STA $8D    [$00:008D]   A:C563 X:000C Y:0004 P:eNvmxdizc
