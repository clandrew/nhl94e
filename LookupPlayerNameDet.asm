// We scramble X, so back up and restore it.
DA                   PHX

// Load the team index, which has been stored at 9F1C98/9F1C9A for home/away.
A4 91                LDY $91
B9 98 1C             LDA $1C98, y[$9F:1C98]
// Multiply by 4 to turn index into an offset
0A                   ASL
0A                   ASL

// Use the team index to look up into the "alternate main table".
// Load the array element from 0xA8D000
AA                   TAX
BF 00 D0 A8          LDA 0xA8D000,x
85 89                STA $89
E8                   INX
E8                   INX
BF 00 D0 A8          LDA 0xA8D000,x
85 8B                STA $8B

// The "alternate main table" is formatted a bit differently from the "main table".
// Each element is itself an array, one four-byte element per player.
// Use $A5 as a counter to get to the right player.

// $A0/8131
//PlayerIndexIncrement:
A5 A5                LDA $A5 ; Sets Z
F0 0C                BEQ __ ; goto DonePlayerIndex

E6 89                INC $89
E6 89                INC $89
E6 89                INC $89
E6 89                INC $89

C6 A5                DEC A5

80 F0                BRA $F0 ; goto PlayerIndexIncrement

//DonePlayerIndex:
// We have the element for the right player stored at $89-$8B.
// The element is a pointer. 
// It'll be either $9Cxxxx if we're keeping the original names, or $A8xxxx/whatever if we're using new names.
// Dereference it, and store the dereferenced result at $89-$8B.
A7 89                LDA [$89] ; 
48                   PHA
E6 89                INC $89
E6 89                INC $89
A7 89                LDA [$89] ; 
85 8B                STA $8B
68                   PLA
85 89                STA $89

// Restore X and return.
FA                   PLX
6B                   RTL

// Note that the original routine is 37 bytes.
// This routine is ~53 bytes.
// It'd be nice if we could simply not detour and instead patch over the original, but we can't so oh well.
