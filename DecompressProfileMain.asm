// void DecompressProfileMain()
// Preconditions: Choice of home or away team is in $91. 0 means home, 2 means away
//		  Current teams choices are stored at 0x9F1C98 and 0x9f1C9A for home and away respectively.
//
// Postconditions: 
// 
// This function is called twice when loading the GAME SETUP screen, once per team.
// The inner loop iterates six times.
//
$9D/CC42 A5 A5       LDA $A5    [$00:00A5]  // Backup $A5, literal 0x10 
$9D/CC44 48          PHA                     
$9D/CC45 A2 0A 00    LDX #$000A   

// afterInit:
$9D/CC48 DA          PHX                     
$9D/CC49 8A          TXA                     
$9D/CC4A 4A          LSR A                   
$9D/CC4B 22 73 C9 9D JSL $9DC973[$9D:C973]   // Call BeforeDecompress()

$9D/CC4F A6 91       LDX $91    [$00:0091]   // Load the choice of HomeOrAway. 0 == home, 2 == away
					     
$9D/CC51 BD 98 1C    LDA $1C98,x[$9F:1C98]   // Load the index, CurrentTeam. Whether it's home or away will depend on the value of X.

					     // Current selection for home team is stored at 0x9F1C98					     
					     // Current selection for visitor team is stored at 0x8f1C9A

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

$9D/CC54 9F 6C 34 7E STA $7E346C,x[$7E:346C] 
$9D/CC58 A2 1A 00    LDX #$001A              // Literal 0x1A = 26
$9D/CC5B 22 4C 8F 80 JSL $808F4C[$80:8F4C]   // Call MultiplyAWithX() to multiply CurrentTeam by 26
$9D/CC5F 18          CLC                     

$9D/CC60 65 A5       ADC $A5    [$00:00A5]   // ($A5) stores a number indicating which player on the team. Note that the numbers are per-team.

$9D/CC62 0A          ASL A                   
$9D/CC63 0A          ASL A                   
$9D/CC64 AA          TAX                     
$9D/CC65 BF 53 CD 9D LDA $9DCD53,x[$9D:D16F] // X stores a special number representing which profile to load!! E.g., 0478 means P. Roy
$9D/CC69 85 0C       STA $0C    [$00:000C]                 
$9D/CC6B BF 55 CD 9D LDA $9DCD55,x[$9D:D1CD] 
$9D/CC6F 85 0E       STA $0E    [$00:000E]   

$9D/CC71 A9 7F 00    LDA #$007F              
$9D/CC74 85 12       STA $12    [$00:0012]   
$9D/CC76 A9 00 00    LDA #$0000              
$9D/CC79 85 10       STA $10    [$00:0010]   // Make ($10)-($12) store literal 0x7F0000

$9D/CC7B 22 73 C3 80 JSL $80C373[$80:C373]   // Call RelatedToDecompression(). X is garbage by now, ignore it.
$9D/CC7F 86 00       STX $00    [$00:0000]   

$9D/CC81 A3 01       LDA $01,s  [$00:1FF6]   // Load one of {0, 2, 4, 6, 8, 10} off the stack and stash it in X
$9D/CC83 AA          TAX                     
$9D/CC84 A5 91       LDA $91    [$00:0091]   // Acc = ($91) == 0? #$2D00 : #$5100;
$9D/CC86 F0 05       BEQ $05    [$CC8D]      
 					     // First 6 calls yield literal 0x5100, last 6 are literal 0x2D00
 					     // This is a baseline number to add on top of.
$9D/CC88 A9 00 2D    LDA #$2D00              
$9D/CC8B 80 03       BRA $03    [$CC90]      
$9D/CC8D A9 00 51    LDA #$5100              
$9D/CC90 18          CLC                     
$9D/CC91 7F AE CC 9D ADC $9DCCAE,x[$9D:CCAE] // $9DCCAE is an array of two-byte elements. X is {A, 8, 6, 4, 2}. Each element is an offset

					     // This array is just hard coded in the ROM.

					     // For example, on the first call, we look at the last element. To do that, X=0xA.
					     // The last element stores "00 1E" meaning we get 0x1E00.
					     // Adding that to Acc's baseline number of 0x5100, we get
					     // Acc = 0x5100 + 0x1E00 = 0x6F00
 					     
 					     // Store Acc as the lower 2 bytes of ImageAddress, so that if Acc=0x6F00, then
 					     // ImageAddress located at ($10)-($12) will be 0x7F6F00 for example
$9D/CC95 85 10       STA $10    [$00:0010]   
$9D/CC97 A9 7F 00    LDA #$007F              
$9D/CC9A 85 0E       STA $0E    [$00:000E]   
$9D/CC9C A9 00 00    LDA #$0000              
$9D/CC9F 85 0C       STA $0C    [$00:000C]   // Set ($0E) = 0x7F, ($0C) = 0x0
$9D/CCA1 22 C2 85 9B JSL $9B85C2[$9B:85C2]   // Call DecompressPlayerGraphic()
$9D/CCA5 FA          PLX                     // Restore the 0x10 from before
$9D/CCA6 CA          DEX                     
$9D/CCA7 CA          DEX                     
$9D/CCA8 10 9E       BPL $9E    [$CC48]      // branch to afterInit
$9D/CCAA 68          PLA                     
$9D/CCAB 85 A5       STA $A5    [$00:00A5]   // Restore $A5 from before

//////////////////////////////////////// 
// Starts at $A0/806B  bp at A0806B
// Treat 0C-0E as scratch space. Also 10-12.
//////////////////////////////////////// 

// Configure source (example: Montreal = 0xA39400)

A6 91                LDX $91    // Load the choice of HomeOrAway. 0 == home, 2 == away
BD 98 1C             LDA $1C98,x[$9F:1C98]   // Load the index, CurrentTeam. Whether it's home or away will depend on the value of X.
0A                   ASL     // *= 4
0A                   ASL
AA                   TAX

// Load from PlayerGraphicsArray, starting at 0xA08700. Each element is 4 bytes.
BF 00 87 A0          LDA $A08700, X
85 0E                STA $0E
E8                   INX
E8                   INX
BF 00 87 A0          LDA $A08700, X
85 0C                STA $0C
/////////////////////////////////////////////////////////

// Configure dest (example: Home 1 = 0x7F5100)////////////
A9 7F 00             LDA #$007F
85 12                STA $12
A5 91                LDA $91    // Load the choice of HomeOrAway. 0 == home, 2 == away
D0 05                BNE InitAway

// InitHome:
A9 00 51             LDA #$5100
80 03                BRA DoneHomeOrAway

// InitAway:
A9 00 2D             LDA #$2D00

// DoneHomeOrAway
85 10                STA $10
/////////////////////////////////////////////////////////

A2 06 00             LDX #$0006

// EachPlayer:
A0 80 04			 LDY #$0480

B7 0C                LDA long,y from 0xA3B200 //        Loads using 24-bit pointer
97 10                STA long,y from 0x7F5100			Store using 24-bit pointer
88                   DEY
D0 F9			     BNE 

// Source+=600
A5 0C                LDA $0C
18                   CLC                  
69 00 06             ADC #$0600
85 0C                STA $0C
A5 0E                LDA $0E
69 00 00             ADC #$0000
85 0E                STA $0E

// Dest+=600
A5 10                LDA $10
18                   CLC                  
69 00 06             ADC #$0600
85 10                STA $10
A5 12                LDA $12
69 00 00             ADC #$0000
85 12                STA $12

CA                   DEX
D0 D5			     BNE EachPlayer

6B                   RTL
