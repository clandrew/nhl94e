// void LoadTeamProfileImages()
// Preconditions: Choice of home or away team is in $91. 0 means home, 2 means away
//		  Current teams choices are stored at 0x9F1C98 and 0x9f1C9A for home and away respectively.
//
// Postconditions: 
//		  Six players on a team have their decompressed profile images stored in system memory at the appropriate location.
//        Doesn't do VRAM transfer.
// 
// This function is called twice when loading the GAME SETUP screen, once per team.
//

//Done:
// Do an absolutely stupid copy of 0x600 bytes from A3C500 to 7F5100 for home, to 7F2D00 for away.
// Source: $0C-$0E
// Dest:   $10-$12
A0 00 24			LDY #$2400
88					DEY

A9 00 C5			LDA C500
85 0C				STA $0C
A9 A3 00			LDA 00A3
85 0E				STA $0E

A6 91				LDX $91
D0 07				BNE Away

A9 00 51			LDA 5100
85 10				STA $10
80 05				BRA after

// Away:
A9 00 2D			LDA 2D00
85 10				STA $10

// After:
A9 7F 00			LDA 007F
85 12				STA $12

// CopyLoop:
B7 0C				LDA [$0C],y
97 10				STA [$10],y
88					DEY
10 F9				BPL CopyLoop

$9D/CCAD 6B          RTL