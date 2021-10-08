# nhl94e - NHL'94 player name and statistics ROM editor
This is a player name and statistics editor for the game "NHL '94" for the SNES. Features:
* Easy-to-use Windows interface
* Includes editing of player numbers
* Your custom player names can be different length from original

It works by mix of data and code modification. The stats changing is a dead simple data change, while custom player names uses code change. Find more technical information about changing players' names here:
[https://github.com/clandrew/nhl94e/blob/main/docs/PlayerNames.txt](https://github.com/clandrew/nhl94e/blob/main/docs/PlayerNames.txt)

Changing of players' names involves ROM expansion. No worries, the program does the expansion for you automatically.

Example in the editor:

![Example image](https://raw.githubusercontent.com/clandrew/nhl94e/main/images/Screen1.PNG "Example image.")

Patched game result:

![Example image](https://raw.githubusercontent.com/clandrew/nhl94e/main/images/Screen2.png "Example image.")
![Example image](https://raw.githubusercontent.com/clandrew/nhl94e/main/images/Screen3.png "Example image.")

You can also rename teams and team venues.

In the latest source, here's what works:
* Team renames that show up beside the "Home" and "Visitor" labels of the GAME SETUP screen
* Team renames in both player cards and Ron Barr's commentary
* Venue renames in both player cards and Ron Barr's commentary
* Can change color schemes of the subheader for a team
* Goalie renames show up in the goalie selection menu
* Can edit players' skin color in gameplay

## Build
The source code is organized as a Visual Studio 2019 built for x86-64 architecture. It's a C++/CLI Windows Forms application. Normally I don't use C++/CLI but it started out as a command-line tool written in C++. When I wanted to add GUI this made the most sense.

The program reads a source code file "LookupPlayerNameDet.asm" when doing code patching. This source code file lists out machine code with assembly in comments. The assembly doesn't do anything and is there for descriptive purposes. The project is set up this way to make it compatible with some general-purpose 65816 detouring code I wrote a while ago and used for this.

I wrote another wrapper for code in a class called 'ObjectCode' which makes it easier to programmatically form a code patch. Most stuff uses this wrapper now rather than the above source file.
