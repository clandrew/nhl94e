$9F/C732 A9 9C 00    LDA #$009C              A:009D X:0000 Y:0004 P:envmxdizc	; Hardcode 9C in the upper bytes. Easy enough
$9F/C735 85 8B       STA $8B    [$00:008B]   A:009C X:0000 Y:0004 P:envmxdizc

$9F/C737 A4 91       LDY $91    [$00:0091]   A:009C X:0000 Y:0004 P:envmxdizc	; Load the choice of HomeOrAway. 0 == home, 2 == away

$9F/C739 B9 DC 1C    LDA $1CDC,y[$9F:1CDC]   A:009C X:0000 Y:0000 P:envmxdiZc	; Load PlayerNamesStartAddress for the corresponding team.
																				;
																				; This value is set up by SetupStringThings().
																				;
																				; For home, load from 9F1CDC = e.g., C2DB from cartridge for Montreal
																				; For away, load from 9F1CDE = e.g., BDB2 from catridge for LA Kings
																				; One thing: 9F1CDC is a cartridge value. Why can it change?
																				; Apparently they decided to treat this as scratch memory. Ok then.
// We scramble X, so back up and restore it.
DA                   PHX

///////////////////////////////////////////////////////////
// Calculate key from A
// 
4A                   LSR
4A                   LSR
4A                   LSR
4A                   LSR
4A                   LSR
4A                   LSR
4A                   LSR
4A                   LSR ; Mask off one byte

4A                   LSR ; Divide by 2

// Subtract 0x53
38                   SEC
E9 53 00		     SBC #$0053

// Use the key to look up table value from 0xA8D000
AA                   TAX
BF 00 D0 A8          LDA 0xA8D000,x
29 0F 00             AND #$000F
// E.g., for Montreal we would get 0B as the value.

// Multiply by 4
0A                   ASL
0A                   ASL

// Load the array element from 0xA8D023
AA                   TAX
BF 23 D0 A8          LDA 0xA8D000,x
85 89                STA $89
E8                   INX
E8                   INX
BF 23 D0 A8          LDA 0xA8D000,x
85 8B                STA $8B

// Now we've set up 89-8B to contain 0xA8D477, an address which points to 0x9CC330
// If A5 == 0, we should load a pointer from 0xA8D477.
// If A5 == 1, we should load it from 0xA8D477 + 4.
// $A0/8131
//PlayerIndexIncrement:
A5 A5                LDA $A5 ; Sets Z
F0 0C				 BEQ __ ; goto DonePlayerIndex

E6 89                INC $89
E6 89                INC $89
E6 89                INC $89
E6 89                INC $89

C6 A5                DEC A5

80 F0                BRA $F0 ; goto PlayerIndexIncrement

//DonePlayerIndex:
A7 89                LDA [$89] ; 0x00A8
48                   PHA
E6 89                INC $89
E6 89                INC $89
A7 89                LDA [$89] ; 0xD477
85 8B                STA $8B
68                   PLA
85 89                STA $89

// Index by $A5

FA                   PLX

6B                   RTL

// The ultimate value we want to return, before indexing for players, is 9CC330. This points to Patrick Roy (Plus leading 0xD, 0x0), the player index 0 of Montreal.