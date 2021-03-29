//////////

// Detoured out into 9D/C12B - 9D/C12B

A4 91       LDY $91 
A9 9C 00    LDA #$009C
85 8F       STA $8F
B9 DC 1C    LDA $1CDC,y[$9F:1CDC]
85 8D       STA $8D

A0 04 00    LDY #$0004 
18          CLC
77 8D       ADC [$8D],y[$9C:C2DF]
85 8D       STA $8D    [$00:008D]

18          CLC 
67 8D       ADC [$8D]  [$9C:C554]
85 8D       STA $8D    [$00:008D]
18          CLC 
67 8D       ADC [$8D]  [$9C:C55E]
85 8D       STA $8D    [$00:008D]

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
