#include "Util.h"
#include "Decompress2.h"
#include <vector>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <iomanip>

std::ofstream debugLog;

void OpenDebugLog(char const* fileName)
{
    debugLog.open(fileName, std::ofstream::out | std::ofstream::trunc);
}

void CloseDebugLog()
{
    debugLog.close();
}

std::vector<unsigned char> LoadBinaryFile8(char const* fileName)
{
    std::vector<unsigned char> result;

    FILE* file{};
    fopen_s(&file, fileName, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        result.resize(length);
        fseek(file, 0, SEEK_SET);
        fread(result.data(), 1, length, file);
        fclose(file);
    }

    return result;
}

std::vector<unsigned short> LoadBinaryFile16(char const* fileName)
{
    std::vector<unsigned short> result;

    FILE* file{};
    fopen_s(&file, fileName, "rb");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    result.resize(length);
    fseek(file, 0, SEEK_SET);
    fread(result.data(), 1, length, file);
    fclose(file);

    return result;
}

void DebugPrintRegs(unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << " A:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << a;
    debugLog << " X:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << x;
    debugLog << " Y:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << y;
    debugLog << " P:envmxdizc";
}

void DebugPrintFinalize()
{
    debugLog << "\n";
    debugLog.flush();
}

void DebugPrint(const char* asmText, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << asmText;
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithPCAndImm8(unsigned short pc, const char* asmByte, const char* asmOp, unsigned char imm8, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";
    debugLog << asmByte << ' ' << std::hex << std::setw(2) << std::setfill('0') << (int)imm8 << "       ";
    debugLog << asmOp << " #$" << std::hex << std::setw(2) << std::setfill('0') << (int)imm8 << "               ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithBankAndIndex(const char* asmPrefix, unsigned char bank, unsigned short index, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << asmPrefix;
    debugLog << "[$";
    debugLog << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)bank;
    debugLog << ":";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithIndex(const char* asmPrefix, unsigned short index, unsigned short a, unsigned short x, unsigned short y, bool longAddress)
{
    debugLog << asmPrefix;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index;
    debugLog << "]";
    if (!longAddress)
    {
        debugLog << "  ";
    }
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithPC(unsigned short pc, const char* asmText, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";
    debugLog << asmText;
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithPCAndIndex(unsigned short pc, const char* asmText, unsigned short index, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";
    debugLog << asmText;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithPCAndBankAndIndex(unsigned short pc, const char* asmText, unsigned char bank, unsigned short index, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";
    debugLog << asmText;
    debugLog << "[$";
    debugLog << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)bank;
    debugLog << ":";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintSBCAbsolute(unsigned short pc, unsigned char bank, unsigned short index, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";

    unsigned short indexAcc = index;
    unsigned char indexLow = indexAcc & 0xFF;
    indexAcc >>= 8;
    unsigned char indexHigh = indexAcc;

    debugLog << "ED ";
    debugLog << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)indexLow << " ";
    debugLog << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)indexHigh << "    SBC $";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index << "  [$";
    debugLog << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)bank << ":";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index << "]  ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintJMPAbsolute0760(unsigned short pc, unsigned short mem0760Value, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " 6C 60 07    JMP ($0760)[$80:";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << mem0760Value << "]  ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

unsigned short ExchangeShortHighAndLow(unsigned short s)
{
    unsigned short part0 = s & 0x00FF;
    unsigned short part1 = s & 0xFF00;
    part0 <<= 8;
    part1 >>= 8;
    s = part0 | part1;
    return s;
}

unsigned short IncLow8(unsigned short s)
{
    unsigned char ch = s & 0xFF;
    ++ch;
    s = ch;
    return s;
}

void RotateLeft(unsigned short* pS, bool* pC)
{
    bool c = *pS >= 0x8000;
    *pS = *pS << 1;
    if (*pC)
    {
        *pS |= 0x1;
    }
    *pC = c;
}

std::string GetTeamName(Team team)
{
    char const* names[] =
    {
        "Anaheim",
        "Boston",
        "Buffalo",
        "Calgary",
        "Chicago",
        "Dallas",
        "Detroit",
        "Edmonton",
        "Florida",
        "Hartford",
        "LAKings",
        "Montreal",
        "NewJersey",
        "NYIslanders",
        "NYRangers",
        "Ottawa",
        "Philly",
        "Pittsburgh",
        "Quebec",
        "SanJose",
        "StLouis",
        "TampaBay",
        "Toronto",
        "Vancouver",
        "Washington",
        "Winnipeg",
        "AllStarsEast",
        "AllStarsWest",
    };

    return names[(int)team];
}

std::string GetPlayerName(int sourceDataLocation)
{
    struct Entry
    {
        int IndexWithinTeam;
        std::string PlayerName;
        int pProfileImageLocation;
        int ProfileImageLocation;
    } entries[] =
    {
        {0x0,     "Guy Herbert", 0x9dcd53, 0x99ac36  },
        {0x1,	"Ron Tugnutt",	0x9dcd57,	0x99c64e	},
        {0x2,	"Steven King",	0x9dcd5b,	0x99e1e8	},
        {0x3,	"Troy Loney",	0x9dcd5f,	0x979931	},
        {0x4,	"Stu Grimson",	0x9dcd63,	0x9ab13d	},
        {0x5,	"Terry Yake",	0x9dcd67,	0x98bcf3	},
        {0x6,	"Bob Corkum",	0x9dcd6b,	0x8ffe74	},
        {0x7,	"Anatoli Semenov",	0x9dcd6f,	0x98d19d	},
        {0x8,	"Lonnie Loach",	0x9dcd73,	0x99e1e8	},
        {0x9,	"Robin Bawa",	0x9dcd77,	0x8ffe74	},
        {0xa,	"Tim Sweeney",	0x9dcd7b,	0x9ab13d	},
        {0xb,	"Alexei Kasatonov",	0x9dcd7f,	0x98cd0a	},
        {0xc,	"Sean Hill",	0x9dcd83,	0x8ffe74	},
        {0xd,	"Randy Ladouceur",	0x9dcd87,	0x99bfd5	},
        {0xe,	"David Williams",	0x9dcd8b,	0x99e1e8	},
        {0xf,	"Bill Houlder",	0x9dcd8f,	0x8ffe74	},
        {0x10,	"Bobby Dollas",	0x9dcd93,	0x9ab13d	},
        {0x11,	"Dennis Vial",	0x9dcd97,	0x99e1e8	},

        //Team index 1,
        {0x0,	"Andy Moog",	0x9dcdbb,	0x97f28a	},
        {0x1,	"John Blue",	0x9dcdbf,	0x99c64e	},
        {0x2,	"Adam Oates",	0x9dcdc3,	0x98f814	},
        {0x3,	"Dave Poulin",	0x9dcdc7,	0x99e1e8	},
        {0x4,	"Vladimir Ruzicka",	0x9dcdcb,	0x8ffe74	},
        {0x5,	"Ted Donato",	0x9dcdcf,	0x9ab13d	},
        {0x6,	"Joe Juneau",	0x9dcdd3,	0x98e3cd	},
        {0x7,	"Dmitri Kvartalnov",	0x9dcdd7,	0x99e1e8	},
        {0x8,	"Dave Reid",	0x9dcddb,	0x8ffe74	},
        {0x9,	"Gregori Pantaleyev",	0x9dcddf,	0x9ab13d	},
        {0xa,	"Brent Hughes",	0x9dcde3,	0x99e1e8	},
        {0xb,	"Cam Neely",	0x9dcde7,	0x999867	},
        {0xc,	"Stephen Leach",	0x9dcdeb,	0x8ffe74	},
        {0xd,	"Stephen Heinze",	0x9dcdef,	0x9ab13d	},
        {0xe,	"C.J. Young",	0x9dcdf3,	0x99e1e8	},
        {0xf,	"Darin Kimble",	0x9dcdf7,	0x8ffe74	},
        {0x10,	"Peter Douris",	0x9dcdfb,	0x9ab13d	},
        {0x11,	"Ray Bourque",	0x9dcdff,	0x998000	},
        {0x12,	"Don Sweeney",	0x9dce03,	0x99d102	},
        {0x13,	"Glen Wesley",	0x9dce07,	0x8ffe74	},
        {0x14,	"David Shaw",	0x9dce0b,	0x9ab13d	},
        {0x15,	"Gordie Roberts",	0x9dce0f,	0x99e1e8	},
        {0x16,	"Glen Feathrston",	0x9dce13,	0x8ffe74	},
        {0x17,	"Jim Wiemer",	0x9dce17,	0x9ab13d	},

        // Team index 2 },
        {0x0,	"Grant Fuhr",	0x9dce23,	0x998d5d },
        {0x1,	"Tom Draper",	0x9dce27,	0x99c64e },
        {0x2,	"Dominik Hasek",	0x9dce2b,	0x99c64e },
        {0x3,	"Pat LaFontaine",	0x9dce2f,	0x99b2c7 },
        {0x4,	"Dale Hawerchuk",	0x9dce33,	0x989a18 },
        {0x5,	"Bob Sweeney",	0x9dce37,	0x99e1e8 },
        {0x6,	"Dave Hannan",	0x9dce3b,	0x8ffe74 },
        {0x7,	"Randy Wood",	0x9dce3f,	0x9ab13d },
        {0x8,	"Yuri Khmylev",	0x9dce43,	0x99e1e8 },
        {0x9,	"Brad May",	0x9dce47,	0x8ffe74 },
        {0xa,	"Bob Errey",	0x9dce4b,	0x9ab13d },
        {0xb,	"Rob Ray",	0x9dce4f,	0x99e1e8 },
        {0xc,	"Alexnder Mogilny",	0x9dce53,	0x99c1fe },
        {0xd,	"Wayne Presley",	0x9dce57,	0x8ffe74 },
        {0xe,	"Donald Audette",	0x9dce5b,	0x9ab13d },
        {0xf,	"Colin Patterson",	0x9dce5f,	0x99e1e8 },
        {0x10,	"Doug Bodger",	0x9dce63,	0x98a5cd },
        {0x11,	"Petr Svoboda",	0x9dce67,	0x98b16a },
        {0x12,	"Richard Smehlik",	0x9dce6b,	0x8ffe74 },
        {0x13,	"Ken Sutton",	0x9dce6f,	0x9ab13d },
        {0x14,	"Grant Ledyard",	0x9dce73,	0x99e1e8 },
        {0x15,	"Gord Donnelly",	0x9dce77,	0x8ffe74 },
        {0x16,	"Randy Moller",	0x9dce7b,	0x9ab13d },
        {0x17,	"Keith Carney",	0x9dce7f,	0x99e1e8 },

        // Team index 3 },
        {0x0,	"Mike Vernon",	0x9dce8b,	0x97ba0d },
        {0x1,	"Jeff Reese",	0x9dce8f,	0x99c64e },
        {0x2,	"Joe Nieuwendyk",	0x9dce93,	0x99ccba },
        {0x3,	"Robert Reichel",	0x9dce97,	0x99e1e8 },
        {0x4,	"Joel Otto",	0x9dce9b,	0x8ffe74 },
        {0x5,	"Gary Roberts",	0x9dce9f,	0x99aa05 },
        {0x6,	"Paul Ranheim",	0x9dcea3,	0x9ab13d },
        {0x7,	"Brent Ashton",	0x9dcea7,	0x99e1e8 },
        {0x8,	"Chris Lindberg",	0x9dceab,	0x8ffe74 },
        {0x9,	"Craig Berube",	0x9dceaf,	0x9ab13d },
        {0xa,	"Theoren Fleury",	0x9dceb3,	0x97eb52 },
        {0xb,	"Sergei Makarov",	0x9dceb7,	0x99e1e8 },
        {0xc,	"Greg Paslawski",	0x9dcebb,	0x8ffe74 },
        {0xd,	"Ronnie Stern",	0x9dcebf,	0x9ab13d },
        {0xe,	"Gary Suter",	0x9dcec3,	0x978e95 },
        {0xf,	"Al MacInnis",	0x9dcec7,	0x97e8e8 },
        {0x10,	"Roger Johansson",	0x9dcecb,	0x99e1e8 },
        {0x11,	"Trent Yawney",	0x9dcecf,	0x8ffe74 },
        {0x12,	"Frank Musil",	0x9dced3,	0x9ab13d },
        {0x13,	"Michel Petit",	0x9dced7,	0x99e1e8 },
        {0x14,	"Kevin Dahl",	0x9dcedb,	0x8ffe74 },
        {0x15,	"Chris Dahlquist",	0x9dcedf,	0x9ab13d },
        {0x16,	"Greg Smyth",	0x9dcee3,	0x99e1e8 },

        // Team index 4 },
        {0x0,	"Ed Belfour",	0x9dcef3,	0x97a600 },
        {0x1,	"Jim Waite",	0x9dcef7,	0x99c64e },
        {0x2,	"Jeremy Roenick",	0x9dcefb,	0x98ea95 },
        {0x3,	"Christan Ruuttu",	0x9dceff,	0x9ab13d },
        {0x4,	"Brent Sutter",	0x9dcf03,	0x99e1e8 },
        {0x5,	"Troy Murray",	0x9dcf07,	0x8ffe74 },
        {0x6,	"Michel Goulet",	0x9dcf0b,	0x98a821 },
        {0x7,	"Stephane Matteau",	0x9dcf0f,	0x9ab13d }, 
        {0x8,	"Greg Gilbert",	0x9dcf13,	0x99e1e8 },
        {0x9,	"Jocelyn Lemieux",	0x9dcf17,	0x8ffe74 },
        {0xa,	"Steve Larmer",	0x9dcf1b,	0x97cdf3 },
        {0xb,	"Dirk Graham",	0x9dcf1f,	0x9ab13d },
        {0xc,	"Brian Noonan",	0x9dcf23,	0x99e1e8 },
        {0xd,	"Dave Christian",	0x9dcf27,	0x8ffe74 },
        {0xe,	"Joe Murphy",	0x9dcf2b,	0x9ab13d },
        {0xf,	"Rob Brown",	0x9dcf2f,	0x99e1e8 },
        {0x10,	"Chris Chelios",	0x9dcf33,	0x99b4f7 },
        {0x11,	"Steve Smith",	0x9dcf37,	0x998b25 },
        {0x12,	"Bryan Marchment",	0x9dcf3b,	0x8ffe74 },
        {0x13,	"Frantsek Kucera",	0x9dcf3f,	0x9ab13d },
        {0x14,	"Craig Muni",	0x9dcf43,	0x99e1e8 },
        {0x15,	"Keith Brown",	0x9dcf47,	0x8ffe74 },
        {0x16,	"Cam Russell",	0x9dcf4b,	0x9ab13d },
        {0x17,	"Adam Bennett",	0x9dcf4f,	0x99e1e8 },

        // Team index 5 },
        {0x0,	"Jon Casey",	0x9dcf5b,	0x96fd81 },
        {0x1,	"Darcy Wakaluk",	0x9dcf5f,	0x99c64e },
        {0x2,	"Mike Modano",	0x9dcf63,	0x97ad8b },
        {0x3,	"Dave Gagner",	0x9dcf67,	0x9a8c2c },
        {0x4,	"Neal Broten",	0x9dcf6b,	0x8ffe74 },
        {0x5,	"Brent Gilchrist",	0x9dcf6f,	0x9ab13d },
        {0x6,	"Mike McPhee",	0x9dcf73,	0x99e1e8 },
        {0x7,	"Gaetan Duchesne",	0x9dcf77,	0x8ffe74 },
        {0x8,	"Brian Propp",	0x9dcf7b,	0x9ab13d },
        {0x9,	"Russ Courtnall",	0x9dcf7f,	0x97f023 },
        {0xa,	"Ulf Dahlen",	0x9dcf83,	0x99e1e8 },
        {0xb,	"Mike Craig",	0x9dcf87,	0x8ffe74 },
        {0xc,	"Trent Klatt",	0x9dcf8b,	0x9ab13d },
        {0xd,	"Shane Churla",	0x9dcf8f,	0x99e1e8 },
        {0xe,	"Stewart Gavin",	0x9dcf93,	0x8ffe74 },
        {0xf,	"Mark Tinordi",	0x9dcf97,	0x98ecd6 },
        {0x10,	"Tommy Sjodin",	0x9dcf9b,	0x98a120 },
        {0x11,	"Jim Johnson",	0x9dcf9f,	0x9ab13d },
        {0x12,	"Derian Hatcher",	0x9dcfa3,	0x99e1e8 },
        {0x13,	"Craig Ludwig",	0x9dcfa7,	0x8ffe74 },
        {0x14,	"Richard Matvichuk",	0x9dcfab,	0x9ab13d },
        {0x15,	"Mark Osiecki",	0x9dcfaf,	0x99e1e8 },
        {0x16,	"Brad Berry",	0x9dcfb3,	0x8ffe74 },
        {0x17,	"Enrico Ciccone",	0x9dcfb7,	0x9ab13d },

        // Team index 6 },
        {0x0,	"Tim Cheveldae",	0x9dcfc3,	0x98f5d7 },
        {0x1,	"Vincent Riendeau",	0x9dcfc7,	0x99c64e },
        {0x2,	"Steve Yzerman",	0x9dcfcb,	0x99c426 },
        {0x3,	"Sergei Fedorov",	0x9dcfcf,	0x90fde2 },
        {0x4,	"Dallas Drake",	0x9dcfd3,	0x8ffe74 },
        {0x5,	"Mike Sillinger",	0x9dcfd7,	0x9ab13d },
        {0x6,	"Vachslav Kozlov",	0x9dcfdb,	0x99e1e8 },
        {0x7,	"Paul Ysebaert",	0x9dcfdf,	0x8ffe74 },
        {0x8,	"Shawn Burr",	0x9dcfe3,	0x9ab13d },
        {0x9,	"Keith Primeau",	0x9dcfe7,	0x99e1e8 },
        {0xa,	"Gerard Gallant",	0x9dcfeb,	0x8ffe74 },
        {0xb,	"John Ogrodnick",	0x9dcfef,	0x9ab13d },
        {0xc,	"Dino Ciccarelli",	0x9dcff3,	0x979bc5 },
        {0xd,	"Ray Sheppard",	0x9dcff7,	0x99e1e8 },
        {0xe,	"Bob Probert",	0x9dcffb,	0x8ffe74 },
        {0xf,	"Sheldon Kennedy",	0x9dcfff,	0x9ab13d },
        {0x10,	"Jim Hiller",	0x9dd003,	0x99e1e8 },
        {0x11,	"Paul Coffey",	0x9dd007,	0x9a940f },
        {0x12,	"Steve Chiasson",	0x9dd00b,	0x99fab4 },
        {0x13,	"Yves Racine",	0x9dd00f,	0x8ffe74 },
        {0x14,	"Nicklas Lidstrom",	0x9dd013,	0x9ab13d },
        {0x15,	"Mark Howe",	0x9dd017,	0x99e1e8 },
        {0x16,	"Vladimir Konstantov",	0x9dd01b,	0x8ffe74 },
        {0x17,	"Steve Konroyd",	0x9dd01f,	0x9ab13d },
        {0x18,	"Brad McCrimmon",	0x9dd023,	0x99e1e8 },

        // Team index 7 },
        {0x0,	"Bill Ranford",	0x9dd02b,	0x9993fd },
        {0x1,	"Doug Weight",	0x9dd02f,	0x98d876 },
        {0x2,	"Todd Elik",	0x9dd033,	0x8ffe74 },
        {0x3,	"Craig MacTavish",	0x9dd037,	0x9ab13d },
        {0x4,	"Kevin Todd",	0x9dd03b,	0x99e1e8 },
        {0x5,	"Shjon Podein",	0x9dd03f,	0x8ffe74 },
        {0x6,	"Mike Hudson",	0x9dd043,	0x9ab13d },
        {0x7,	"Shayne Corson",	0x9dd047,	0x97bc8c },
        {0x8,	"Craig Simpson",	0x9dd04b,	0x99e1e8 },
        {0x9,	"Zdeno Ciger",	0x9dd04f,	0x8ffe74 },
        {0xa,	"Kelly Buchberger",	0x9dd053,	0x9ab13d },
        {0xb,	"Martin Gelinas",	0x9dd057,	0x99e1e8 },
        {0xc,	"Louie DeBrusk",	0x9dd05b,	0x8ffe74 },
        {0xd,	"Petr Klima",	0x9dd05f,	0x97c682 },
        {0xe,	"Steven Rice",	0x9dd063,	0x9ab13d },
        {0xf,	"Dave Manson",	0x9dd067,	0x98898f },
        {0x10,	"Igor Kravchuk",	0x9dd06b,	0x99b097 },
        {0x11,	"Brian Benning",	0x9dd06f,	0x99e1e8 },
        {0x12,	"Geoff Smith",	0x9dd073,	0x9ab13d },
        {0x13,	"Brian Glynn",	0x9dd077,	0x99e1e8 },
        {0x14,	"Luke Richardson",	0x9dd07b,	0x8ffe74 },
        {0x15,	"Chris Joseph",	0x9dd07f,	0x9ab13d },
        {0x16,	"Brad Werenka",	0x9dd083,	0x99e1e8 },

        // Team index 8 },
        {0x0,	"John Vanbiesbrk",	0x9dd093,	0x97c188 },
        {0x1,	"Mark Fitzpatrik",	0x9dd097,	0x99c64e },
        {0x2,	"Tom Fitzgerald",	0x9dd09b,	0x99e1e8 },
        {0x3,	"Scott Mellanby",	0x9dd09f,	0x998479 },
        {0x4,	"Brian Skrudland",	0x9dd0a3,	0x989ec9 },
        {0x5,	"Mike Hough",	0x9dd0a7,	0x99e1e8 },
        {0x6,	"Dave Lowry",	0x9dd0ab,	0x8ffe74 },
        {0x7,	"Andrei Lomakin",	0x9dd0af,	0x97a0e8 },
        {0x8,	"Randy Gilhen",	0x9dd0b3,	0x99e1e8 },
        {0x9,	"Jesse Belanger",	0x9dd0b7,	0x8ffe74 },
        {0xa,	"Bill Lindsay",	0x9dd0bb,	0x9ab13d },
        {0xb,	"Joe Cirella",	0x9dd0bf,	0x97f9bc },
        {0xc,	"Alexnder Godynyuk",	0x9dd0c3,	0x8ffe74 },
        {0xd,	"Gord Murphy",	0x9dd0c7,	0x97d06c },
        {0xe,	"Gord Hynes",	0x9dd0cb,	0x99e1e8 },
        {0xf,	"Milan Tichy",	0x9dd0cf,	0x8ffe74 },
        {0x10,	"Stephane Richer",	0x9dd0d3,	0x9ab13d },

        // Team index 9 },
        {0x0,	"Sean Burke",	0x9dd0fb,	0x98dd01 },
        {0x1,	"Mario Gosselin",	0x9dd0ff,	0x99c64e },
        {0x2,	"Frank Pietrngelo",	0x9dd103,	0x99c64e },
        {0x3,	"Andrew Cassels",	0x9dd107,	0x99ee68 },
        {0x4,	"Mikael Nylander",	0x9dd10b,	0x99e1e8 },
        {0x5,	"Robert Kron",	0x9dd10f,	0x8ffe74 },
        {0x6,	"Robert Petrovicky",	0x9dd113,	0x9ab13d },
        {0x7,	"Geoff Sanderson",	0x9dd117,	0x98acc7 },
        {0x8,	"Patrick Poulin",	0x9dd11b,	0x99e1e8 },
        {0x9,	"Yvon Corriveau",	0x9dd11f,	0x8ffe74 },
        {0xa,	"Jim McKenzie",	0x9dd123,	0x9ab13d },
        {0xb,	"Randy Cunnyworth",	0x9dd127,	0x99e1e8 },
        {0xc,	"Pat Verbeek",	0x9dd12b,	0x98b608 },
        {0xd,	"Mark Janssens",	0x9dd12f,	0x8ffe74 },
        {0xe,	"Nick Kypreos",	0x9dd133,	0x9ab13d },
        {0xf,	"Mark Greig",	0x9dd137,	0x99e1e8 },
        {0x10,	"Jamie Leach",	0x9dd13b,	0x8ffe74 },
        {0x11,	"Zarley Zalapski",	0x9dd13f,	0x999f04 },
        {0x12,	"Eric Weinrich",	0x9dd143,	0x98af19 },
        {0x13,	"Adam Burt",	0x9dd147,	0x99e1e8 },
        {0x14,	"Dan Keczmer",	0x9dd14b,	0x8ffe74 },
        {0x15,	"Doug Houda",	0x9dd14f,	0x9ab13d },
        {0x16,	"Allen Pedersen",	0x9dd153,	0x99e1e8 },

        // Team index a },
        {0x0,	"Kelly Hrudey",	0x9dd163,	0x97e40b },
        {0x1,	"Robb Stauber",	0x9dd167,	0x99c64e },
        {0x2,	"Rick Knickle",	0x9dd16b,	0x99c64e },
        {0x3,	"Wayne Gretzky",	0x9dd16f,	0x97df28 },
        {0x4,	"Jimmy Carson",	0x9dd173,	0x8ffe74 },
        {0x5,	"Corey Millen",	0x9dd177,	0x9ab13d },
        {0x6,	"Gary Shuchuk",	0x9dd17b,	0x99e1e8 },
        {0x7,	"Luc Robitaille",	0x9dd17f,	0x98d62e },
        {0x8,	"Tony Granato",	0x9dd183,	0x8ffe74 },
        {0x9,	"Pat Conacher",	0x9dd187,	0x9ab13d },
        {0xa,	"Warren Rychel",	0x9dd18b,	0x99e1e8 },
        {0xb,	"Tomas Sandstrom",	0x9dd18f,	0x97d7cc },
        {0xc,	"Jari Kurri",	0x9dd193,	0x8ffe74 },
        {0xd,	"Mike Donnelly",	0x9dd197,	0x9ab13d },
        {0xe,	"Dave Taylor",	0x9dd19b,	0x99e1e8 },
        {0xf,	"Rob Blake",	0x9dd19f,	0x97a887 },
        {0x10,	"Marty McSorley",	0x9dd1a3,	0x97c8fe },
        {0x11,	"Alexei Zhitnik",	0x9dd1a7,	0x8ffe74 },
        {0x12,	"Darryl Sydor",	0x9dd1ab,	0x99e1e8 },
        {0x13,	"Charlie Huddy",	0x9dd1af,	0x8ffe74 },
        {0x14,	"Mark Hardy",	0x9dd1b3,	0x9ab13d },
        {0x15,	"Brent Thompson",	0x9dd1b7,	0x99e1e8 },
        {0x16,	"Tim Watters",	0x9dd1bb,	0x8ffe74 },
        {0x17,	"Rene Chapdlaine",	0x9dd1bf,	0x9ab13d },

        // Team index b },
        {0x0,	"Patrick Roy",	0x9dd1cb,	0x9a862e },
        {0x1,	"Andre Racicot",	0x9dd1cf,	0x99c64e },
        {0x2,	"Kirk Muller",	0x9dd1d3,	0x99f8ac },
        {0x3,	"Stephan Lebeau",	0x9dd1d7,	0x99e1e8 },
        {0x4,	"Denis Savard",	0x9dd1db,	0x8ffe74 },
        {0x5,	"Guy Carbonneau",	0x9dd1df,	0x9ab13d },
        {0x6,	"Vincent Damphousse",	0x9dd1e3,	0x97d557 },
        {0x7,	"Gilbert Dionne",	0x9dd1e7,	0x99e1e8 },
        {0x8,	"John Leclair",	0x9dd1eb,	0x8ffe74 },
        {0x9,	"Benoit Brunet",	0x9dd1ef,	0x9ab13d },
        {0xa,	"Mario Roberge",	0x9dd1f3,	0x99e1e8 },
        {0xb,	"Brian Bellows",	0x9dd1f7,	0x98cac0 },
        {0xc,	"Mike Keane",	0x9dd1fb,	0x8ffe74 },
        {0xd,	"Gary Leeman",	0x9dd1ff,	0x9ab13d },
        {0xe,	"Todd Ewen",	0x9dd203,	0x99e1e8 },
        {0xf,	"Ed Ronan",	0x9dd207,	0x8ffe74 },
        {0x10,	"Eric Desjardins",	0x9dd20b,	0x9988ec },
        {0x11,	"Matt Schneider",	0x9dd20f,	0x99dfcf },
        {0x12,	"Patrice Brisebois",	0x9dd213,	0x9ab13d },
        {0x13,	"Kevin Haller",	0x9dd217,	0x99e1e8 },
        {0x14,	"Rob Ramage",	0x9dd21b,	0x8ffe74 },
        {0x15,	"J.J. Daigneault",	0x9dd21f,	0x9ab13d },
        {0x16,	"Lyle Odelein",	0x9dd223,	0x99e1e8 },
        {0x17,	"Donald Dufresne",	0x9dd227,	0x8ffe74 },



        // Team index c },
        {0x0,	"Chris Terreri",	0x9dd233,	0x9aa329 },
        {0x1,	"Craig Billington",	0x9dd237,	0x99c64e },
        {0x2,	"Alexnder Semak",	0x9dd23b,	0x99d545 },
        {0x3,	"Bernie Nicholls",	0x9dd23f,	0x9ab13d },
        {0x4,	"Peter Stastny",	0x9dd243,	0x99e1e8 },
        {0x5,	"Janne Ojanen",	0x9dd247,	0x8ffe74 },
        {0x6,	"Valeri Zelepukin",	0x9dd24b,	0x9ab13d },
        {0x7,	"John MacLean",	0x9dd24f,	0x99e1e8 },
        {0x8,	"Bobby Holik",	0x9dd253,	0x8ffe74 },
        {0x9,	"Tom Chorske",	0x9dd257,	0x9ab13d },
        {0xa,	"Troy Mallette",	0x9dd25b,	0x99e1e8 },
        {0xb,	"Stephane Richer",	0x9dd25f,	0x9aa6d6 },
        {0xc,	"Claude Lemieux",	0x9dd263,	0x97ab0a },
        {0xd,	"Bill Guerin",	0x9dd267,	0x8ffe74 },
        {0xe,	"Randy McKay",	0x9dd26b,	0x9ab13d },
        {0xf,	"Scott Pellerin",	0x9dd26f,	0x99e1e8 },
        {0x10,	"Dave Barr",	0x9dd273,	0x8ffe74 },
        {0x11,	"Scott Stevens",	0x9dd277,	0x98f158 },
        {0x12,	"Vachslav Fetisov",	0x9dd27b,	0x988264 },
        {0x13,	"Bruce Driver",	0x9dd27f,	0x9ab13d },
        {0x14,	"Scott Niedrmayer",	0x9dd283,	0x99e1e8 },
        {0x15,	"Ken Daneyko",	0x9dd287,	0x8ffe74 },
        {0x16,	"Tommy Albelin",	0x9dd28b,	0x9ab13d },
        {0x17,	"Myles O'Connor",	0x9dd28f,	0x99e1e8 },

        // Team index d },
        {0x0,	"Glenn Healy",	0x9dd29b,	0x9986b3 },
        {0x1,	"Pierre Turgeon",	0x9dd29f,	0x9a8225 },
        {0x2,	"Benoit Hogue",	0x9dd2a3,	0x9991c8 },
        {0x3,	"Marty McInnis",	0x9dd2a7,	0x99e1e8 },
        {0x4,	"Travis Green",	0x9dd2ab,	0x8ffe74 },
        {0x5,	"Ray Ferraro",	0x9dd2af,	0x9ab13d },
        {0x6,	"Claude Loiselle",	0x9dd2b3,	0x99e1e8 },
        {0x7,	"Steve Thomas",	0x9dd2b7,	0x98a377 },
        {0x8,	"Derek King",	0x9dd2bb,	0x8ffe74 },
        {0x9,	"Dave Volek",	0x9dd2bf,	0x9ab13d },
        {0xa,	"Patrick Flatley",	0x9dd2c3,	0x99e1e8 },
        {0xb,	"Brian Mullen",	0x9dd2c7,	0x8ffe74 },
        {0xc,	"Brad Dalgarno",	0x9dd2cb,	0x9ab13d },
        {0xd,	"Mick Vukota",	0x9dd2cf,	0x99e1e8 },
        {0xe,	"Vladimir Malakhov",	0x9dd2d3,	0x99f079 },
        {0xf,	"Darius Kasparitis",	0x9dd2d7,	0x97c405 },
        {0x10,	"Jeff Norton",	0x9dd2db,	0x8ffe74 },
        {0x11,	"Uwe Krupp",	0x9dd2df,	0x9ab13d },
        {0x12,	"Tom Kurvers",	0x9dd2e3,	0x8ffe74 },
        {0x13,	"Scott Lachance",	0x9dd2e7,	0x9ab13d },
        {0x14,	"Dennis Vaske",	0x9dd2eb,	0x99e1e8 },
        {0x15,	"Richard Pilon",	0x9dd2ef,	0x8ffe74 },

        // Team index e },
        {0x0,	"Mike Richter",	0x9dd303,	0x9a8e26 },
        {0x1,	"Mark Messier",	0x9dd307,	0x98bf41 },
        {0x2,	"Sergei Nemchinov",	0x9dd30b,	0x8ffe74 },
        {0x3,	"Darren Turcotte",	0x9dd30f,	0x9ab13d },
        {0x4,	"Ed Olczyk",	0x9dd313,	0x99e1e8 },
        {0x5,	"Esa Tikkanen",	0x9dd317,	0x97b28d },
        {0x6,	"Adam Graves",	0x9dd31b,	0x8ffe74 },
        {0x7,	"Phil Bourque",	0x9dd31f,	0x9ab13d },
        {0x8,	"Jan Erixon",	0x9dd323,	0x99e1e8 },
        {0x9,	"Mike Gartner",	0x9dd327,	0x98cf54 },
        {0xa,	"Tony Amonte",	0x9dd32b,	0x8ffe74 },
        {0xb,	"Alexei Kovalev",	0x9dd32f,	0x9ab13d },
        {0xc,	"Paul Broten",	0x9dd333,	0x99e1e8 },
        {0xd,	"Joey Kocur",	0x9dd337,	0x8ffe74 },
        {0xe,	"Mike Hartman",	0x9dd33b,	0x9ab13d },
        {0xf,	"Brian Leetch",	0x9dd33f,	0x979406 },
        {0x10,	"James Patrick",	0x9dd343,	0x9884c8 },
        {0x11,	"Sergei Zubov",	0x9dd347,	0x99e1e8 },
        {0x12,	"Jeff Beukeboom",	0x9dd34b,	0x8ffe74 },
        {0x13,	"Kevin Lowe",	0x9dd34f,	0x9ab13d },
        {0x14,	"Peter Andersson",	0x9dd353,	0x99e1e8 },
        {0x15,	"Jay Wells",	0x9dd357,	0x8ffe74 },

        // Team index f },
        {0x0,	"Peter Sidorkwicz",	0x9dd36b,	0x8cfdec },
        {0x1,	"Daniel Berthiaume",	0x9dd36f,	0x99c64e },
        {0x2,	"Jamie Baker",	0x9dd373,	0x99e619 },
        {0x3,	"Mark Lamb",	0x9dd377,	0x99e1e8 },
        {0x4,	"Neil Brady",	0x9dd37b,	0x8ffe74 },
        {0x5,	"Mark Freer",	0x9dd37f,	0x9ab13d },
        {0x6,	"Laurie Boschman",	0x9dd383,	0x99e1e8 },
        {0x7,	"David Archibald",	0x9dd387,	0x8ffe74 },
        {0x8,	"Rob Murphy",	0x9dd38b,	0x9ab13d },
        {0x9,	"Sylvain Turgeon",	0x9dd38f,	0x99ae67 },
        {0xa,	"Mike Peluso",	0x9dd393,	0x99e1e8 },
        {0xb,	"Doug Smail",	0x9dd397,	0x8ffe74 },
        {0xc,	"Jeff Lazaro",	0x9dd39b,	0x9ab13d },
        {0xd,	"Darcy Loewen",	0x9dd39f,	0x99e1e8 },
        {0xe,	"Bob Kudelski",	0x9dd3a3,	0x99f498 },
        {0xf,	"Jody Hull",	0x9dd3a7,	0x8ffe74 },
        {0x10,	"Andrew McBain",	0x9dd3ab,	0x9ab13d },
        {0x11,	"Tomas Jelinek",	0x9dd3af,	0x99e1e8 },
        {0x12,	"Norm Maciver",	0x9dd3b3,	0x99ea44 },
        {0x13,	"Brad Shaw",	0x9dd3b7,	0x99db9b },
        {0x14,	"Darren Rumble",	0x9dd3bb,	0x8ffe74 },
        {0x15,	"Chris Luongo",	0x9dd3bf,	0x9ab13d },
        {0x16,	"Ken Hammond",	0x9dd3c3,	0x99e1e8 },
        {0x17,	"Gord Dineen",	0x9dd3c7,	0x8ffe74 },
        {0x18,	"Brad Marsh",	0x9dd3cb,	0x9ab13d },

        // Team index 10 },
        {0x0,	"Tommy Soderstrom",	0x9dd3d3,	0x988e50 },
        {0x1,	"Dominic Roussel",	0x9dd3d7,	0x99c64e },
        {0x2,	"Steph Beauregard",	0x9dd3db,	0x99c64e },
        {0x3,	"Eric Lindros",	0x9dd3df,	0x98dabc },
        {0x4,	"Rod BrindAmour",	0x9dd3e3,	0x98fc8e },
        {0x5,	"Pelle Eklund",	0x9dd3e7,	0x99e1e8 },
        {0x6,	"Josef Beranek",	0x9dd3eb,	0x8ffe74 },
        {0x7,	"Keith Acton",	0x9dd3ef,	0x9ab13d },
        {0x8,	"Vachslav Butsayev",	0x9dd3f3,	0x99e1e8 },
        {0x9,	"Brent Fedyk",	0x9dd3f7,	0x8ffe74 },
        {0xa,	"Doug Evans",	0x9dd3fb,	0x9ab13d },
        {0xb,	"Dave Snuggerud",	0x9dd3ff,	0x99e1e8 },
        {0xc,	"Claude Boivin",	0x9dd403,	0x8ffe74 },
        {0xd,	"Mark Recchi",	0x9dd407,	0x9897bf },
        {0xe,	"Kevin Dineen",	0x9dd40b,	0x9ab13d },
        {0xf,	"Dave Brown",	0x9dd40f,	0x99e1e8 },
        {0x10,	"Garry Galley",	0x9dd413,	0x98c18e },
        {0x11,	"Dimitri Yushkevich",	0x9dd417,	0x99e831 },
        {0x12,	"Greg Hawgood",	0x9dd41b,	0x8ffe74 },
        {0x13,	"Terry Carkner",	0x9dd41f,	0x9ab13d },
        {0x14,	"Ric Nattress",	0x9dd423,	0x99e1e8 },
        {0x15,	"Ryan McGill",	0x9dd427,	0x8ffe74 },
        {0x16,	"Shawn Cronin",	0x9dd42b,	0x9ab13d },

        // Team index 11 },
        {0x0,	"Tom Barrasso",	0x9dd43b,	0x99f6a3 },
        {0x1,	"Ken Wregget",	0x9dd43f,	0x99c64e },
        {0x2,	"Mario Lemieux",	0x9dd443,	0x9aa500 },
        {0x3,	"Ron Francis",	0x9dd447,	0x99e1e8 },
        {0x4,	"Shawn McEachern",	0x9dd44b,	0x8ffe74 },
        {0x5,	"Mike Stapleton",	0x9dd44f,	0x9ab13d },
        {0x6,	"Kevin Stevens",	0x9dd453,	0x98e610 },
        {0x7,	"Dave Tippett",	0x9dd457,	0x99e1e8 },
        {0x8,	"Jeff Daniels",	0x9dd45b,	0x8ffe74 },
        {0x9,	"Jaromir Jagr",	0x9dd45f,	0x9a9f70 },
        {0xa,	"Rick Tocchet",	0x9dd463,	0x9ab13d },
        {0xb,	"Joe Mullen",	0x9dd467,	0x99e1e8 },
        {0xc,	"Martin Straka",	0x9dd46b,	0x8ffe74 },
        {0xd,	"Mike Needham",	0x9dd46f,	0x9ab13d },
        {0xe,	"Jay Caufield",	0x9dd473,	0x99e1e8 },
        {0xf,	"Larry Murphy",	0x9dd477,	0x9a8a31 },
        {0x10,	"Ulf Samuelsson",	0x9dd47b,	0x989566 },
        {0x11,	"Jim Paek",	0x9dd47f,	0x8ffe74 },
        {0x12,	"Paul Stanton",	0x9dd483,	0x9ab13d },
        {0x13,	"Peter Taglianeti",	0x9dd487,	0x99e1e8 },
        {0x14,	"Mike Ramsey",	0x9dd48b,	0x8ffe74 },
        {0x15,	"Kjell Samuelsson",	0x9dd48f,	0x9ab13d },
        {0x16,	"Grant Jennings",	0x9dd493,	0x99e1e8 },
        {0x17,	"Bryan Fogarty",	0x9dd497,	0x8ffe74 },

        // Team index 12 },
        {0x0,	"Ron Hextall",	0x9dd4a3,	0x98ef17 },
        {0x1,	"Stephane Fiset",	0x9dd4a7,	0x99c64e },
        {0x2,	"Joe Sakic",	0x9dd4ab,	0x99a5a0 },
        {0x3,	"Mike Ricci",	0x9dd4af,	0x8ffe74 },
        {0x4,	"Claude Lapointe",	0x9dd4b3,	0x9ab13d },
        {0x5,	"Martin Rucinsky",	0x9dd4b7,	0x99e1e8 },
        {0x6,	"Valeri Kamensky",	0x9dd4bb,	0x8ffe74 },
        {0x7,	"Gino Cavallini",	0x9dd4bf,	0x9ab13d },
        {0x8,	"Scott Pearson",	0x9dd4c3,	0x99e1e8 },
        {0x9,	"Chris Simon",	0x9dd4c7,	0x8ffe74 },
        {0xa,	"Tony Twist",	0x9dd4cb,	0x9ab13d },
        {0xb,	"Mats Sundin",	0x9dd4cf,	0x9a901f },
        {0xc,	"Owen Nolan",	0x9dd4d3,	0x9a801f },
        {0xd,	"Andrei Kovalenko",	0x9dd4d7,	0x99e1e8 },
        {0xe,	"Scott Young",	0x9dd4db,	0x8ffe74 },
        {0xf,	"Steve Duchesne",	0x9dd4df,	0x99d325 },
        {0x10,	"Curtis Leschyshyn",	0x9dd4e3,	0x97a374 },
        {0x11,	"Alexei Gusarov",	0x9dd4e7,	0x9ab13d },
        {0x12,	"Kerry Huffman",	0x9dd4eb,	0x99e1e8 },
        {0x13,	"Adam Foote",	0x9dd4ef,	0x8ffe74 },
        {0x14,	"Steven Finn",	0x9dd4f3,	0x9ab13d },
        {0x15,	"Mikhail Tatarinov",	0x9dd4f7,	0x99e1e8 },
        {0x16,	"Craig Wolanin",	0x9dd4fb,	0x8ffe74 },

        // Team index 13 },
        {0x0,	"Arturs Irbe",	0x9dd50b,	0x98aa74 },
        {0x1,	"Jeff Hackett",	0x9dd50f,	0x99c64e },
        {0x2,	"Brian Hayward",	0x9dd513,	0x99c64e },
        {0x3,	"Kelly Kisio",	0x9dd517,	0x98b3b9 },
        {0x4,	"Rob Gaudreau",	0x9dd51b,	0x9ab13d },
        {0x5,	"Dean Evason",	0x9dd51f,	0x99e1e8 },
        {0x6,	"Mike Sullivan",	0x9dd523,	0x8ffe74 },
        {0x7,	"Perry Berezan",	0x9dd527,	0x9ab13d },
        {0x8,	"Johan Garpenlov",	0x9dd52b,	0x98fa51 },
        {0x9,	"Jeff Odgers",	0x9dd52f,	0x99e1e8 },
        {0xa,	"John Carter",	0x9dd533,	0x8ffe74 },
        {0xb,	"David Maley",	0x9dd537,	0x9ab13d },
        {0xc,	"Pat Falloon",	0x9dd53b,	0x98930c },
        {0xd,	"Ed Courtenay",	0x9dd53f,	0x99e1e8 },
        {0xe,	"Mark Pederson",	0x9dd543,	0x8ffe74 },
        {0xf,	"Doug Wilson",	0x9dd547,	0x98872c },
        {0x10,	"Neil Wilkinson",	0x9dd54b,	0x97f757 },
        {0x11,	"Sandis Ozolinsh",	0x9dd54f,	0x9ab13d },
        {0x12,	"Tom Pederson",	0x9dd553,	0x99e1e8 },
        {0x13,	"Doug Zmolek",	0x9dd557,	0x8ffe74 },
        {0x14,	"Jay More",	0x9dd55b,	0x9ab13d },
        {0x15,	"Peter Ahola",	0x9dd55f,	0x99e1e8 },
        {0x16,	"Rob Zettler",	0x9dd563,	0x8ffe74 },

        // Team index 14 },
        {0x0,	"Curtis Joseph",	0x9dd573,	0x9a9218 },
        {0x1,	"Craig Janney",	0x9dd577,	0x9a99c9 },
        {0x2,	"Nelson Emerson",	0x9dd57b,	0x9ab13d },
        {0x3,	"Ron Sutter",	0x9dd57f,	0x99e1e8 },
        {0x4,	"Ron Wilson",	0x9dd583,	0x8ffe74 },
        {0x5,	"Bob Bassen",	0x9dd587,	0x9ab13d },
        {0x6,	"Philippe Bozon",	0x9dd58b,	0x99e1e8 },
        {0x7,	"Igor Korolev",	0x9dd58f,	0x8ffe74 },
        {0x8,	"Basil McRae",	0x9dd593,	0x9ab13d },
        {0x9,	"Brendan Shanahan",	0x9dd597,	0x99a36c },
        {0xa,	"Brett Hull",	0x9dd59b,	0x98f399 },
        {0xb,	"Kevin Miller",	0x9dd59f,	0x99e1e8 },
        {0xc,	"Rich Sutter",	0x9dd5a3,	0x8ffe74 },
        {0xd,	"Kelly Chase",	0x9dd5a7,	0x9ab13d },
        {0xe,	"Jeff Brown",	0x9dd5ab,	0x98e18a },
        {0xf,	"Garth Butcher",	0x9dd5af,	0x99e401 },
        {0x10,	"Doug Crossman",	0x9dd5b3,	0x99e1e8 },
        {0x11,	"Rick Zombo",	0x9dd5b7,	0x8ffe74 },
        {0x12,	"Stephane Quintal",	0x9dd5bb,	0x9ab13d },
        {0x13,	"Lee Norwood",	0x9dd5bf,	0x99e1e8 },
        {0x14,	"Bret Hedican",	0x9dd5c3,	0x8ffe74 },
        {0x15,	"Curt Giles",	0x9dd5c7,	0x9ab13d },
        {0x16,	"Murray Baron",	0x9dd5cb,	0x99e1e8 },

        // Team index 15 },
        {0x0,	"Wendell Young",	0x9dd5db,	0x97edbc },
        {0x1,	"Pat Jablonski",	0x9dd5df,	0x99c64e },
        {0x2,	"J.C. Bergeron",	0x9dd5e3,	0x99c64e },
        {0x3,	"Brian Bradley",	0x9dd5e7,	0x99bdac },
        {0x4,	"Chris Kontos",	0x9dd5eb,	0x99a138 },
        {0x5,	"Adam Creighton",	0x9dd5ef,	0x9ab13d },
        {0x6,	"Marc Bureau",	0x9dd5f3,	0x99e1e8 },
        {0x7,	"Rob DiMaio",	0x9dd5f7,	0x8ffe74 },
        {0x8,	"Steve Kasper",	0x9dd5fb,	0x9ab13d },
        {0x9,	"Jason Lafreniere",	0x9dd5ff,	0x99e1e8 },
        {0xa,	"Mikael Andersson",	0x9dd603,	0x99f289 },
        {0xb,	"Rob Zamuner",	0x9dd607,	0x8ffe74 },
        {0xc,	"Steve Maltais",	0x9dd60b,	0x9ab13d },
        {0xd,	"John Tucker",	0x9dd60f,	0x99e1e8 },
        {0xe,	"Danton Cole",	0x9dd613,	0x8ffe74 },
        {0xf,	"Tim Bergland",	0x9dd617,	0x9ab13d },
        {0x10,	"Stan Drulia",	0x9dd61b,	0x99e1e8 },
        {0x11,	"Bob Beers",	0x9dd61f,	0x988000 },
        {0x12,	"Roman Hamrlik",	0x9dd623,	0x9a8832 },
        {0x13,	"Shawn Chambers",	0x9dd627,	0x8ffe74 },
        {0x14,	"Marc Bergevin",	0x9dd62b,	0x9ab13d },
        {0x15,	"Joe Reekie",	0x9dd62f,	0x99e1e8 },
        {0x16,	"Chris Lipuma",	0x9dd633,	0x8ffe74 },
        {0x17,	"Matt Hervey",	0x9dd637,	0x9ab13d },

        // Team index 16 },
        {0x0,	"Felix Potvin",	0x9dd643,	0x998f93 },
        {0x1,	"Daren Puppa",	0x9dd647,	0x99c64e },
        {0x2,	"Doug Gilmour",	0x9dd64b,	0x9aa8ab },
        {0x3,	"John Cullen",	0x9dd64f,	0x99e1e8 },
        {0x4,	"Mike Krushelski",	0x9dd653,	0x8ffe74 },
        {0x5,	"Peter Zezel",	0x9dd657,	0x9ab13d },
        {0x6,	"Dave McLlwain",	0x9dd65b,	0x99e1e8 },
        {0x7,	"Dave Andreychuk",	0x9dd65f,	0x99fcba },
        {0x8,	"Wendel Clark",	0x9dd663,	0x8ffe74 },
        {0x9,	"Mark Osborne",	0x9dd667,	0x9ab13d },
        {0xa,	"Bill Berg",	0x9dd66b,	0x99e1e8 },
        {0xb,	"Nikolai Borshevsky",	0x9dd66f,	0x979e57 },
        {0xc,	"Glenn Anderson",	0x9dd673,	0x8ffe74 },
        {0xd,	"Rob Pearson",	0x9dd677,	0x9ab13d },
        {0xe,	"Mike Foligno",	0x9dd67b,	0x99e1e8 },
        {0xf,	"Ken Baumgartnr",	0x9dd67f,	0x8ffe74 },
        {0x10,	"Todd Gill",	0x9dd683,	0x98d3e6 },
        {0x11,	"Dave Ellett",	0x9dd687,	0x99d981 },
        {0x12,	"Dimitri Mironov",	0x9dd68b,	0x9ab13d },
        {0x13,	"Drake Berehowsky",	0x9dd68f,	0x99e1e8 },
        {0x14,	"Jamie Macoun",	0x9dd693,	0x8ffe74 },
        {0x15,	"Bob Rouse",	0x9dd697,	0x9ab13d },
        {0x16,	"Sylvain Lefebvre",	0x9dd69b,	0x99e1e8 },
        {0x17,	"Bob McGill",	0x9dd69f,	0x8ffe74 },

        // Team index 17 },
        {0x0,	"Kirk McLean",	0x9dd6ab,	0x98c3db },
        {0x1,	"Kay Whitmore",	0x9dd6af,	0x99c64e },
        {0x2,	"Cliff Ronning",	0x9dd6b3,	0x99b954 },
        {0x3,	"Petr Nedved",	0x9dd6b7,	0x8ffe74 },
        {0x4,	"Tom Fergus",	0x9dd6bb,	0x99e1e8 },
        {0x5,	"Geoff Courtnall",	0x9dd6bf,	0x98baa5 },
        {0x6,	"Murray Craven",	0x9dd6c3,	0x9ab13d },
        {0x7,	"Greg Adams",	0x9dd6c7,	0x99e1e8 },
        {0x8,	"Sergio Momesso",	0x9dd6cb,	0x8ffe74 },
        {0x9,	"Gino Odjick",	0x9dd6cf,	0x9ab13d },
        {0xa,	"Garry Valk",	0x9dd6d3,	0x99e1e8 },
        {0xb,	"Pavel Bure",	0x9dd6d7,	0x98b857 },
        {0xc,	"Trevor Linden",	0x9dd6db,	0x8ffe74 },
        {0xd,	"Dixon Ward",	0x9dd6df,	0x9ab13d },
        {0xe,	"Jim Sandlak",	0x9dd6e3,	0x99e1e8 },
        {0xf,	"Tim Hunter",	0x9dd6e7,	0x8ffe74 },
        {0x10,	"Jyrki Lumme",	0x9dd6eb,	0x99ddb5 },
        {0x11,	"Doug Lidster",	0x9dd6ef,	0x97bf0a },
        {0x12,	"Adrien Plavsic",	0x9dd6f3,	0x9ab13d },
        {0x13,	"Jiri Slegr",	0x9dd6f7,	0x99e1e8 },
        {0x14,	"Gerald Diduck",	0x9dd6fb,	0x8ffe74 },
        {0x15,	"Dave Babych",	0x9dd6ff,	0x9ab13d },
        {0x16,	"Dana Murzyn",	0x9dd703,	0x99e1e8 },
        {0x17,	"Robert Dirk",	0x9dd707,	0x8ffe74 },

        // Team index 18 },
        {0x0,	"Don Beaupre",	0x9dd713,	0x97dcb5 },
        {0x1,	"Rick Tabaracci",	0x9dd717,	0x99c64e },
        {0x2,	"Mike Ridley",	0x9dd71b,	0x97b00c },
        {0x3,	"Dimitri Khristich",	0x9dd71f,	0x988bf0 },
        {0x4,	"Dale Hunter",	0x9dd723,	0x99e1e8 },
        {0x5,	"Michal Pivonka",	0x9dd727,	0x8ffe74 },
        {0x6,	"Steve Konowlchuk",	0x9dd72b,	0x9ab13d },
        {0x7,	"Reggie Savage",	0x9dd72f,	0x99e1e8 },
        {0x8,	"Bob Carpenter",	0x9dd733,	0x8ffe74 },
        {0x9,	"Todd Krygier",	0x9dd737,	0x9ab13d },
        {0xa,	"Alan May",	0x9dd73b,	0x99e1e8 },
        {0xb,	"Peter Bondra",	0x9dd73f,	0x99ec56 },
        {0xc,	"Pat Elynuik",	0x9dd743,	0x8ffe74 },
        {0xd,	"Kelly Miller",	0x9dd747,	0x9ab13d },
        {0xe,	"Keith Jones",	0x9dd74b,	0x99e1e8 },
        {0xf,	"Paul MacDermid",	0x9dd74f,	0x8ffe74 },
        {0x10,	"Kevin Hatcher",	0x9dd753,	0x98e853 },
        {0x11,	"Al Iafrate",	0x9dd757,	0x98c628 },
        {0x12,	"Sylvain Cote",	0x9dd75b,	0x9ab13d },
        {0x13,	"Calle Johansson",	0x9dd75f,	0x99e1e8 },
        {0x14,	"Paul Cavallini",	0x9dd763,	0x8ffe74 },
        {0x15,	"Shawn Anderson",	0x9dd767,	0x9ab13d },
        {0x16,	"Jason Woolley",	0x9dd76b,	0x99e1e8 },
        {0x17,	"Rod Langway",	0x9dd76f,	0x8ffe74 },

        // Team index 19 },
        {0x0,	"Bob Essensa",	0x9dd77b,	0x999632 },
        {0x1,	"Jim Hrivnak",	0x9dd77f,	0x99c64e },
        {0x2,	"Alexei Zhamnov",	0x9dd783,	0x99a7d3 },
        {0x3,	"Thomas Steen",	0x9dd787,	0x97d2e2 },
        {0x4,	"Luciano Borsato",	0x9dd78b,	0x9ab13d },
        {0x5,	"Mike Eagles",	0x9dd78f,	0x99e1e8 },
        {0x6,	"Stu Barnes",	0x9dd793,	0x8ffe74 },
        {0x7,	"Darrin Shannon",	0x9dd797,	0x9ab13d },
        {0x8,	"Keith Tkachuk",	0x9dd79b,	0x99e1e8 },
        {0x9,	"Kris King",	0x9dd79f,	0x8ffe74 },
        {0xa,	"Russ Romaniuk",	0x9dd7a3,	0x9ab13d },
        {0xb,	"Andy Brickley",	0x9dd7a7,	0x99e1e8 },
        {0xc,	"Teemu Selanne",	0x9dd7ab,	0x9a842a },
        {0xd,	"Evgeny Davydov",	0x9dd7af,	0x8ffe74 },
        {0xe,	"John Druce",	0x9dd7b3,	0x9ab13d },
        {0xf,	"Bryan Erickson",	0x9dd7b7,	0x99e1e8 },
        {0x10,	"Tie Domi",	0x9dd7bb,	0x8ffe74 },
        {0x11,	"Phil Housley",	0x9dd7bf,	0x9a9d91 },
        {0x12,	"Teppo Numminen",	0x9dd7c3,	0x99823d },
        {0x13,	"Fredrik Olausson",	0x9dd7c7,	0x9ab13d },
        {0x14,	"Sergei Bautin",	0x9dd7cb,	0x99e1e8 },
        {0x15,	"Igor Ulanov",	0x9dd7cf,	0x8ffe74 },
        {0x16,	"Mike Lalor",	0x9dd7d3,	0x9ab13d },
        {0x17,	"Dean Kennedy",	0x9dd7d7,	0x99e1e8 },
        {0x18,	"Randy Carlyle",	0x9dd7db,	0x8ffe74 },
    };

    for (int i = 0; i < _countof(entries); ++i)
    {
        if (entries[i].ProfileImageLocation == sourceDataLocation)
            return entries[i].PlayerName;
    }

    return "{INVALID}";
}