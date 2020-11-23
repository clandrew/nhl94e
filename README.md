# nhl94e - NHL'94 Player name and statistics ROM editor
This is a player name and statistics editor for the game "NHL '94" for the SNES. Features:
* Easy-to-use Windows interface
* Includes editing of player numbers
* Your custom player names can be different length from original

It works by mix of data and code modification. The stats changing is a dead simple data change, while custom player names involves some code change. Find more technical information about changing players' names here:
[https://github.com/clandrew/nhl94e/blob/main/docs/PlayerNames.txt](https://github.com/clandrew/nhl94e/blob/main/docs/PlayerNames.txt)

Changing of players' names involves ROM expansion. No worries, the program does the expansion for you automatically.

In the editor:

![Example image](https://raw.githubusercontent.com/clandrew/nhl94e/main/images/Screen1.PNG "Example image.")

Patched game result:

![Example image](https://raw.githubusercontent.com/clandrew/nhl94e/main/images/Screen2.png "Example image.")

## Build
The source code is organized as a Visual Studio 2019 built for x86-64 architecture. It's a C++/CLI Windows Forms application. Normally I don't use C++/CLI but it started out as a command-line tool written in C++. When I wanted to add GUI this made the most sense.

The program reads a source code file "LookupPlayerNameDet.asm" when doing code patching. This source code file lists out machine code with assembly in comments. The assembly doesn't do anything and is there for descriptive purposes. The project is set up this way to make it compatible with some general-purpose 65816 detouring code I wrote a while ago and used for this.
