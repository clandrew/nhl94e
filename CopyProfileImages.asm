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

$9D/CC42 A5 A5       LDA $A5    [$00:00A5]  
$9D/CC44 48          PHA                    // Politely back up $A5 because LookupPlayerIndexForPlayerWithProfile is going to scramble it.

$9D/CC45 A2 0A 00    LDX #$000A				// OffsetCounter=10. This gets used to index in an array of 2-byte elements.

//ForEachProfileImagePlayerOnTeam:			// Probably don't need this loop!
$9D/CC48 DA          PHX                     

$9D/CC4F A6 91       LDX $91    [$00:0091]   // Load the choice of HomeOrAway. 0 == home, 2 == away					     
$9D/CC51 BD 98 1C    LDA $1C98,x[$9F:1C98]   // Load the index of CurrentTeam. 

$9D/CC54 9F 6C 34 7E STA $7E346C,x[$7E:346C] // Stash the CurrentTeamIndex in the appropriate slot for home versus away. Maybe not necessary when doing the load

$9D/CCA5 FA          PLX                     // Pull OffsetCounter
$9D/CCA6 CA          DEX                     // OffsetCounter -= 2
$9D/CCA7 CA          DEX                     

$9D/CCA8 10 F1       BPL $F1            // if OffsetCounter > 0, goto ForEachProfileImagePlayerOnTeam

//Done:
// Do an absolutely stupid copy of 0x600 bytes from A3C500 to 7F5100 for home, to 7F2D00 for away.
// Source: $0C-$0E
// Dest:   $10-$12
A0 00 24			LDY #$2400

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
D0 F9				BNE CopyLoop

$9D/CCAA 68          PLA                     
$9D/CCAB 85 A5       STA $A5    [$00:00A5]   
$9D/CCAD 6B          RTL					// Restore $A5 and return