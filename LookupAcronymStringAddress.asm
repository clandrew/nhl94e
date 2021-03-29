// Patched into $9F/BD66 - $9F/BD8D

AD 0F 0D    LDA $0D0F  	     ; Original code does this, we preserve it.
48          PHA                     

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

22 BE C7 9E JSL $9EC7BE[$9E:C7BE]  ; Call LoadUtilityString()