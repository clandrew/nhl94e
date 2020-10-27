#include "pch.h"
#include "Form1.h"
#include "AddressMapping.h"
#include "Utils.h"

enum class Team
{
    Anaheim = 0x0,
    Boston = 0x1,
    Buffalo = 0x2,
    Calgary = 0x3,
    Chicago = 0x4,
    Dallas = 0x5,
    Detroit = 0x6,
    Edmonton = 0x7,
    Florida = 0x8,
    Hartford = 0x9,
    LAKings = 0xA,
    Montreal = 0xB,
    NewJersey = 0xC,
    NYIslanders = 0xD,
    NYRangers = 0xE,
    Ottawa = 0xF,
    Philly = 0x10,
    Pittsburgh = 0x11,
    Quebec = 0x12,
    SanJose = 0x13,
    StLouis = 0x14,
    TampaBay = 0x15,
    Toronto = 0x16,
    Vancouver = 0x17,
    Washington = 0x18,
    Winnepeg = 0x19,
    AllStarsEast = 0x1A,
    AllStarsWest = 0x1B
};

std::vector<unsigned char> romData;

enum class Handedness
{
    Left, Right
};

struct PlayerData
{
    int OriginalROMAddress;
    int ReplacedROMAddressForRename;
    std::string Name;

    int PlayerNumber;

    int WeightFactor;
    int WeightInPounds;

    int BaseAgility;
    int BaseSpeed;

    int BaseOffAware;
    int BaseDefAware;

    int BaseShotPower;

    int BaseChecking;

    int HandednessValue;
    Handedness WhichHandedness;

    int BaseStickHandling;
    int BaseShotAccuracy;
    int BaseEndurance;
    int DontKnow;
    int BasePassAccuracy;
    int BaseAggression;
};

struct TeamData
{
    int SourceDataROMAddress;
    int DestDataROMAddress;
    std::vector<unsigned char> Header;
    std::string TeamCity;
    std::string Acronym;
    std::string TeamName;
    std::string Venue;
    std::vector<PlayerData> Players;

    PlayerData* GetPlayer(std::string name)
    {
        for (int i = 0; i < Players.size(); ++i)
        {
            if (Players[i].Name.find(name) != -1)
            {
                return &Players[i];
            }
        }
        return nullptr;
    }
};


class RomDataIterator
{
    int m_fileOffset;

public:
    RomDataIterator(int initialFileOffset)
        : m_fileOffset(initialFileOffset)
    {
    }

    int GetROMOffset()
    {
        return FileOffsetToROMAddress(m_fileOffset);
    }

    void SkipHeader()
    {
        unsigned char headerLength0 = romData[m_fileOffset + 0];
        unsigned char headerLength1 = romData[m_fileOffset + 1];
        m_fileOffset += 2;

        assert(headerLength0 > 0x2 && headerLength1 == 0x0);

        int headerLengthPlusLengthWord = (headerLength1 << 8) | (headerLength0);
        assert(headerLengthPlusLengthWord > 2);
        int headerLength = headerLengthPlusLengthWord - 2;

        m_fileOffset += headerLength;
    }

    std::vector<unsigned char> LoadHeader()
    {
        unsigned char headerLength0 = romData[m_fileOffset + 0];
        unsigned char headerLength1 = romData[m_fileOffset + 1];
        m_fileOffset += 2;

        assert(headerLength0 > 0x2 && headerLength1 == 0x0);

        int headerLengthPlusLengthWord = (headerLength1 << 8) | (headerLength0);
        assert(headerLengthPlusLengthWord > 2);
        int headerLength = headerLengthPlusLengthWord - 2;

        std::vector<unsigned char> result;
        for (int i = 0; i < headerLength; ++i)
        {
            result.push_back(romData[m_fileOffset + i]);
        }
        m_fileOffset += headerLength;

        return result;
    }

    void SaveHeader(std::vector<unsigned char> const& header)
    {
        int headerLength = static_cast<int>(header.size());
        int headerLengthPlusLengthWord = headerLength + 2;
        unsigned char headerLengthPlusLengthWord0 = headerLengthPlusLengthWord & 0xFF;
        unsigned char headerLengthPlusLengthWord1 = (headerLengthPlusLengthWord >> 8) & 0xFF;

        romData[m_fileOffset + 0] = headerLengthPlusLengthWord0;
        romData[m_fileOffset + 1] = headerLengthPlusLengthWord1;
        m_fileOffset += 2;

        for (int i = 0; i < headerLength; ++i)
        {
            romData[m_fileOffset + i] = header[i];
        }
        m_fileOffset += headerLength;
    }

    std::string LoadROMString()
    {
        unsigned char stringLength0 = romData[m_fileOffset + 0];
        unsigned char stringLength1 = romData[m_fileOffset + 1];
        m_fileOffset += 2;

        // Special sentinel values are used to denote empty string
        if (stringLength0 == 0x02 && stringLength1 == 0x0)
            return "";

        int stringLengthPlusLengthWord = (stringLength1 << 8) | (stringLength0);
        assert(stringLengthPlusLengthWord > 2);
        int stringLength = stringLengthPlusLengthWord - 2;

        std::string result;
        for (int i = 0; i < stringLength; ++i)
        {
            result.push_back(romData[m_fileOffset + i]);
        }
        m_fileOffset += stringLength;

        return result;
    }

    void SaveROMString(std::string const& str)
    {
        int strLength = static_cast<int>(str.size());
        int strLengthPlusLengthWord = strLength + 2;
        unsigned char strLengthPlusLengthWord0 = strLengthPlusLengthWord & 0xFF;
        unsigned char strLengthPlusLengthWord1 = (strLengthPlusLengthWord >> 8) & 0xFF;

        romData[m_fileOffset + 0] = strLengthPlusLengthWord0;
        romData[m_fileOffset + 1] = strLengthPlusLengthWord1;
        m_fileOffset += 2;

        for (int i = 0; i < strLength; ++i)
        {
            romData[m_fileOffset + i] = str[i];
        }
        m_fileOffset += strLength;
    }

    int LoadDecimalNumber()
    {
        unsigned char numberByte = romData[m_fileOffset];
        unsigned char numberTens = (numberByte >> 4) & 0xF;
        unsigned char numberOnes = (numberByte & 0xF);
        ++m_fileOffset;
        return (numberTens * 10) + numberOnes;
    }

    void SaveDecimalNumber(int n)
    {
        unsigned char numberTens = n / 10;
        unsigned char numberOnes = n % 10;
        unsigned char resultByte = (numberTens << 4) | numberOnes;
        romData[m_fileOffset] = resultByte;
        ++m_fileOffset;
    }

    void LoadHalfByteNumbers(int* a, int* b)
    {
        unsigned char stat = romData[m_fileOffset];
        unsigned char weightFactor = (stat >> 4) & 0xF;
        *a = weightFactor;

        unsigned char baseAgility = (stat & 0xF);
        *b = baseAgility;

        ++m_fileOffset;
    }

    void SaveHalfByteNumbers(int a, int b)
    {
        assert(a <= 0xf);
        assert(b <= 0xf);
        unsigned char resultByte = (a << 4) | b;
        romData[m_fileOffset] = resultByte;
        ++m_fileOffset;
    }

    void SaveDelimiter()
    {
        romData[m_fileOffset] = 0x02;
        ++m_fileOffset;
        romData[m_fileOffset] = 0x00;
        ++m_fileOffset;
    }

    void SaveLongAddress4Bytes(int addr) // big endian-to-little-endian
    {
        romData[m_fileOffset] = addr & 0xFF;
        addr >>= 8;
        ++m_fileOffset;

        romData[m_fileOffset] = addr & 0xFF;
        addr >>= 8;
        ++m_fileOffset;

        romData[m_fileOffset] = addr & 0xFF;
        addr >>= 8;
        ++m_fileOffset;

        romData[m_fileOffset] = 0;
        ++m_fileOffset;

        assert(addr == 0); // 24-byte address expected
    }

    void SaveByte(int b)
    {
        assert(b > 0 && b < 256);
        romData[m_fileOffset] = b;
        ++m_fileOffset;
    }
};

TeamData GetTeamData(int playerDataAddress)
{
    TeamData result;

    result.SourceDataROMAddress = playerDataAddress;

    RomDataIterator iter(ROMAddressToFileOffset(playerDataAddress));

    result.Header = iter.LoadHeader();

    for (int playerIndex = 0; playerIndex < 30; ++playerIndex)
    {
        PlayerData p{};
        p.OriginalROMAddress = iter.GetROMOffset();
        p.ReplacedROMAddressForRename = 0;
        p.Name = iter.LoadROMString();

        // Convention: empty name string means we reached the end.
        if (p.Name.length() == 0)
            break;

        p.PlayerNumber = iter.LoadDecimalNumber();

        iter.LoadHalfByteNumbers(&p.WeightFactor, &p.BaseAgility);
        p.WeightInPounds = 140 + (p.WeightFactor * 8);

        iter.LoadHalfByteNumbers(&p.BaseSpeed, &p.BaseOffAware);
        iter.LoadHalfByteNumbers(&p.BaseDefAware, &p.BaseShotPower);

        iter.LoadHalfByteNumbers(&p.BaseChecking, &p.HandednessValue);
        p.WhichHandedness = p.HandednessValue % 2 == 0 ? Handedness::Left : Handedness::Right;

        iter.LoadHalfByteNumbers(&p.BaseStickHandling, &p.BaseShotAccuracy);
        iter.LoadHalfByteNumbers(&p.BaseEndurance, &p.DontKnow);
        iter.LoadHalfByteNumbers(&p.BasePassAccuracy, &p.BaseAggression);

        result.Players.push_back(p);
    }

    result.TeamCity = iter.LoadROMString();
    result.Acronym = iter.LoadROMString();
    result.TeamName = iter.LoadROMString();
    result.Venue = iter.LoadROMString();

    return result;
}

std::vector<TeamData> LoadPlayerNamesAndStats()
{
    // Look up the player data pointer table, stored at 0x9CA5E7 in ROM memory map or 0xE25E7 in the ROM file.
    // This is a 28-element array with one element per team.
    // Each element is a pointer to where the team's player data is.

    std::vector<int> playerDataPointers;

    {
        int playerDataPointerTableROMOffset = 0x9CA5E7;
        int fileOffset = ROMAddressToFileOffset(playerDataPointerTableROMOffset);

        for (int teamIndex = 0; teamIndex < 28; ++teamIndex)
        {
            unsigned char b0 = romData[fileOffset + 0];
            unsigned char b1 = romData[fileOffset + 1];
            unsigned char b2 = romData[fileOffset + 2];
            unsigned char b3 = romData[fileOffset + 3];

            int pointer = (b3 << 24) | (b2 << 16) | (b1 << 8) | (b0 << 0);
            playerDataPointers.push_back(pointer);

            fileOffset += 4;
        }
    }

    std::vector<TeamData> result;
    for (int teamIndex = 0; teamIndex < 28; ++teamIndex)
    {
        result.push_back(GetTeamData(playerDataPointers[teamIndex]));
    }

    return result;
}

System::Void CppCLRWinformsProjekt::Form1::exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	this->Close();
}

System::Void CppCLRWinformsProjekt::Form1::Form1_Load(System::Object^ sender, System::EventArgs^ e)
{
	std::wstring romFilename = L"E:\\Emulation\\SNES\\Images\\nhl94e.sfc";
	romData = LoadBytesFromFile(romFilename.c_str());

    // Check size
    size_t expectedSize = 0x400000;
    if (romData.size() != expectedSize)
    {
        std::wostringstream sstream;
        sstream << "This program is designed to work on a Super Nintendo NHL '94 ROM which has been expanded to 32 Mbit (4MB).\n";
        sstream << "Use Lunar Expand or other similar tools to perform expansion.\n";
        sstream << "Unexpected size detected: found " << romData.size() << " bytes, expected " << expectedSize << " bytes.\n";

        System::String^ dialogString = gcnew System::String(sstream.str().c_str());
        MessageBox::Show(dialogString);
    }

    std::vector<TeamData> allTeams = LoadPlayerNamesAndStats();

    for (size_t teamIndex = 0; teamIndex < allTeams.size(); ++teamIndex)
    {
        TeamData const& team = allTeams[teamIndex];

        if (teamIndex != (int)Team::Montreal)
            continue;
    
        System::Windows::Forms::TabControl^ tabControl1;
        System::Windows::Forms::TabPage^ tabPage1;
        System::Windows::Forms::DataGridView^ dataGridView1;
        System::Windows::Forms::DataGridViewTextBoxColumn^ Column1;
        System::Windows::Forms::DataGridViewTextBoxColumn^ Column2;

        tabControl1 = (gcnew System::Windows::Forms::TabControl());
        tabPage1 = (gcnew System::Windows::Forms::TabPage());
        dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
        Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
        Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
        tabControl1->SuspendLayout();
        tabPage1->SuspendLayout();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(dataGridView1))->BeginInit();
        this->SuspendLayout();

        tabControl1->Controls->Add(tabPage1);
        tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
        tabControl1->Location = System::Drawing::Point(0, 24);
        tabControl1->Name = L"tabControl1";
        tabControl1->SelectedIndex = 0;
        tabControl1->Size = System::Drawing::Size(409, 522);
        tabControl1->TabIndex = 2;

        tabPage1->Controls->Add(dataGridView1);
        tabPage1->Location = System::Drawing::Point(4, 22);
        tabPage1->Name = L"tabPage1";
        tabPage1->Padding = System::Windows::Forms::Padding(3);
        tabPage1->Size = System::Drawing::Size(401, 496);
        tabPage1->TabIndex = 0;
        tabPage1->Text = gcnew System::String(team.Acronym.c_str());
        tabPage1->UseVisualStyleBackColor = true;

        dataGridView1->AllowUserToAddRows = false;
        dataGridView1->AllowUserToDeleteRows = false;
        dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
        dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column1 });
        dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column2 });
        dataGridView1->Dock = System::Windows::Forms::DockStyle::Fill;
        dataGridView1->Location = System::Drawing::Point(3, 3);
        dataGridView1->Name = L"dataGridView1";
        dataGridView1->Size = System::Drawing::Size(395, 490);
        dataGridView1->TabIndex = 0;

        Column1->HeaderText = L"Player Name";
        Column1->Name = L"Column1";

        Column2->HeaderText = L"#";
        Column2->Name = L"Column2";
        Column2->Width = 25;


        dataGridView1->Rows->Clear();

        for (size_t playerIndex = 0; playerIndex < team.Players.size(); ++playerIndex)
        {
            PlayerData const& player = team.Players[playerIndex];

            System::String^ playerNameString = gcnew System::String(player.Name.c_str());

            std::ostringstream strm;
            strm << player.PlayerNumber;
            System::String^ playerNumberString = gcnew System::String(strm.str().c_str());

            dataGridView1->Rows->Add(gcnew cli::array<System::String^>(2) { playerNameString, playerNumberString });
        }

        this->Controls->Add(tabControl1);
        tabControl1->ResumeLayout(false);
        tabPage1->ResumeLayout(false);
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(dataGridView1))->EndInit();
        this->ResumeLayout(false);
        this->PerformLayout();
    }



}