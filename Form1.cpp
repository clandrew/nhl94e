#include "pch.h"
#include "Form1.h"
#include "AddressMapping.h"
#include "TeamData.h"
#include "Utils.h"
#include "Optional.h"

static std::vector<TeamData> s_allTeams;

enum class AllocationPurpose
{
    ROM,
    FreeSpace,
    PlayerRenamingDetour,
    PlayerRenamingLookupTable,
    PlayerRenameStrings
};

class RomData
{
    std::vector<unsigned char> m_data;
    std::vector<AllocationPurpose> m_allocationPurpose; // Coupled to m_data. Says what each byte is for

public:
    unsigned char Get(int index) const
    {
        return m_data[index];
    }

    void LoadBytesFromFile(const wchar_t* fileName)
    {
        m_data.clear();

        std::wstring sourcePath = fileName;

        FILE* file = {};
        _wfopen_s(&file, sourcePath.c_str(), L"rb");
        long retrievedFileSize = GetFileSize(file);

        m_data.resize(retrievedFileSize);
        fread(&m_data[0], 1, retrievedFileSize, file);
        fclose(file);
    }

    bool EnsureExpandedSize()
    {
        // Check size
        size_t expectedSize = 0x400000;

        size_t originalSize = m_data.size();
        bool expanded = originalSize == expectedSize;

        const int expansionIdLocation = 0x7FD7;

        if (originalSize == 0x100000)
        {
            // Expand on-demand.
            m_data[expansionIdLocation] = 0x0C;
            m_data.resize(expectedSize);
            for (int i = originalSize; i < expectedSize; ++i)
            {
                m_data[i] = 0;
            }
            return true;
        }
        else if (originalSize == expectedSize)
        {
            // Nothing to do
            return true;
        }
        else
        {
            std::wostringstream sstream;
            sstream << "This program is designed to work on a Super Nintendo NHL '94 ROM. However, the input has an unexpected size.\n";
            sstream << "Unexpected size detected: found " << m_data.size() << " bytes, expected " << expectedSize << " bytes.\n";

            System::String^ dialogString = gcnew System::String(sstream.str().c_str());
            System::Windows::Forms::MessageBox::Show(dialogString);
            return false;
        }
    }

    void InitializeWriteabilityMask()
    {
        // Called when you don't plan on resizing the rom data anymore.
        // Sets up a mask that allows us to debug break if you accidentally try to write the same place twice.
        // Or, if you try to overwrite ROM code by accident.

        // This mask doesn't prevent us from being able to over-write ROM code at all.
        // There are certain times we legitimately want to change ROM code.
        // Rather, this is to prevent changing ROM code accidentally, outside of intended, well-defined reasons.

        // If you need to change ROM code, use SetROMData.

        m_allocationPurpose.resize(m_data.size());

        for (size_t i = 0; i < m_allocationPurpose.size(); ++i)
        {
            if (i >= 0 && i < 0xA08000)
            {
                m_allocationPurpose[i] = AllocationPurpose::ROM; // ROM data. For the most part, we don't over-write this
            }
            else
            {
                m_allocationPurpose[i] = AllocationPurpose::FreeSpace;
            }
        }
    }

    void SetROMData(int index, unsigned char data)
    {
        assert(index < 0xA08000);
        m_data[index] = data;
    }

    void Set(int index, unsigned char data, AllocationPurpose purpose)
    {
        if (m_allocationPurpose[index] != purpose)
        {
            __debugbreak();
        }

        m_data[index] = data;
    }

    void SaveBytesToFile(const wchar_t* fileName)
    {
        std::wstring path = fileName;

        FILE* file;
        _wfopen_s(&file, path.c_str(), L"wb");
        fwrite(&m_data[0], 1, m_data.size(), file);
        fclose(file);
    }

} s_romData;

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
        unsigned char headerLength0 = s_romData.Get(m_fileOffset + 0);
        unsigned char headerLength1 = s_romData.Get(m_fileOffset + 1);
        m_fileOffset += 2;

        assert(headerLength0 > 0x2 && headerLength1 == 0x0);

        int headerLengthPlusLengthWord = (headerLength1 << 8) | (headerLength0);
        assert(headerLengthPlusLengthWord > 2);
        int headerLength = headerLengthPlusLengthWord - 2;

        m_fileOffset += headerLength;
    }

    std::vector<unsigned char> LoadHeader()
    {
        unsigned char headerLength0 = s_romData.Get(m_fileOffset + 0);
        unsigned char headerLength1 = s_romData.Get(m_fileOffset + 1);
        m_fileOffset += 2;

        assert(headerLength0 > 0x2 && headerLength1 == 0x0);

        int headerLengthPlusLengthWord = (headerLength1 << 8) | (headerLength0);
        assert(headerLengthPlusLengthWord > 2);
        int headerLength = headerLengthPlusLengthWord - 2;

        std::vector<unsigned char> result;
        for (int i = 0; i < headerLength; ++i)
        {
            result.push_back(s_romData.Get(m_fileOffset + i));
        }
        m_fileOffset += headerLength;

        return result;
    }

    // Unused
    /*
    void SaveHeader(std::vector<unsigned char> const& header)
    {
        int headerLength = static_cast<int>(header.size());
        int headerLengthPlusLengthWord = headerLength + 2;
        unsigned char headerLengthPlusLengthWord0 = headerLengthPlusLengthWord & 0xFF;
        unsigned char headerLengthPlusLengthWord1 = (headerLengthPlusLengthWord >> 8) & 0xFF;

        s_romData.Set(m_fileOffset + 0, headerLengthPlusLengthWord0);
        s_romData.Set(m_fileOffset + 1, headerLengthPlusLengthWord1);
        m_fileOffset += 2;

        for (int i = 0; i < headerLength; ++i)
        {
            s_romData.Set(m_fileOffset + i, header[i]);
        }
        m_fileOffset += headerLength;
    }*/

    StringWithSourceROMAddress LoadROMStringWithSourceROMAddress()
    {
        StringWithSourceROMAddress result{};

        result.SourceROMAddress = GetROMOffset();

        unsigned char stringLength0 = s_romData.Get(m_fileOffset + 0);
        unsigned char stringLength1 = s_romData.Get(m_fileOffset + 1);
        m_fileOffset += 2;

        // Special sentinel values are used to denote empty string
        if (stringLength0 == 0x02 && stringLength1 == 0x0)
            return result;

        int stringLengthPlusLengthWord = (stringLength1 << 8) | (stringLength0);
        assert(stringLengthPlusLengthWord > 2);
        int stringLength = stringLengthPlusLengthWord - 2;

        for (int i = 0; i < stringLength; ++i)
        {
            result.Str.push_back(s_romData.Get(m_fileOffset + i));
        }
        m_fileOffset += stringLength;

        return result;
    }

    std::string LoadROMString()
    {
        unsigned char stringLength0 = s_romData.Get(m_fileOffset + 0);
        unsigned char stringLength1 = s_romData.Get(m_fileOffset + 1);
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
            result.push_back(s_romData.Get(m_fileOffset + i));
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

        s_romData.Set(m_fileOffset + 0, strLengthPlusLengthWord0);
        s_romData.Set(m_fileOffset + 1, strLengthPlusLengthWord1);
        m_fileOffset += 2;

        for (int i = 0; i < strLength; ++i)
        {
            s_romData.Set(m_fileOffset + i, str[i]);
        }
        m_fileOffset += strLength;
    }

    void SkipROMString()
    {
        unsigned char stringLength0 = s_romData.Get(m_fileOffset + 0);
        unsigned char stringLength1 = s_romData.Get(m_fileOffset + 1);
        m_fileOffset += 2;

        // Special sentinel values are used to denote empty string
        if (stringLength0 == 0x02 && stringLength1 == 0x0)
            return;

        int stringLengthPlusLengthWord = (stringLength1 << 8) | (stringLength0);
        assert(stringLengthPlusLengthWord > 2);
        int stringLength = stringLengthPlusLengthWord - 2;

        m_fileOffset += stringLength;
    }

    int LoadDecimalNumber()
    {
        unsigned char numberByte = s_romData.Get(m_fileOffset);
        unsigned char numberTens = (numberByte >> 4) & 0xF;
        unsigned char numberOnes = (numberByte & 0xF);
        ++m_fileOffset;
        return (numberTens * 10) + numberOnes;
    }

    void SaveDecimalNumber(int n, bool isROMData)
    {
        unsigned char numberTens = n / 10;
        unsigned char numberOnes = n % 10;
        unsigned char resultByte = (numberTens << 4) | numberOnes;
        if (isROMData)
        {
            s_romData.SetROMData(m_fileOffset, resultByte);
        }
        else
        {
            s_romData.Set(m_fileOffset, resultByte);
        }
        ++m_fileOffset;
    }

    void SkipDecimalNumber()
    {
        ++m_fileOffset;
    }

    void LoadHalfByteNumbers(int* a, int* b)
    {
        unsigned char stat = s_romData.Get(m_fileOffset);
        unsigned char weightFactor = (stat >> 4) & 0xF;
        *a = weightFactor;

        unsigned char baseAgility = (stat & 0xF);
        *b = baseAgility;

        ++m_fileOffset;
    }

    void SaveHalfByteNumbers(int a, int b, bool isROMData)
    {
        assert(a <= 0xf);
        assert(b <= 0xf);
        assert(isROMData); // non ROM not supported bc nothing needs it
        unsigned char resultByte = (a << 4) | b;
        s_romData.SetROMData(m_fileOffset, resultByte);
        ++m_fileOffset;
    }

    void SkipHalfByteNumbers()
    {
        ++m_fileOffset;
    }

    void SaveDelimiter()
    {
        s_romData.Set(m_fileOffset, 0x02);
        ++m_fileOffset;
        s_romData.Set(m_fileOffset, 0x00);
        ++m_fileOffset;
    }

    void SaveLongAddress4Bytes(int addr) // big endian-to-little-endian
    {
        s_romData.Set(m_fileOffset, addr & 0xFF);
        addr >>= 8;
        ++m_fileOffset;

        s_romData.Set(m_fileOffset, addr & 0xFF);
        addr >>= 8;
        ++m_fileOffset;

        s_romData.Set(m_fileOffset, addr & 0xFF);
        addr >>= 8;
        ++m_fileOffset;

        s_romData.Set(m_fileOffset, 0);
        ++m_fileOffset;

        assert(addr == 0); // 24-byte address expected
    }

    void SaveByte(int b)
    {
        assert(b > 0 && b < 256);
        s_romData.Set(m_fileOffset, b);
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

        p.Name.Initialize(iter.GetROMOffset(), iter.LoadROMString());

        // Convention: empty name string means we reached the end.
        if (p.Name.Get().length() == 0)
            break;

        p.PlayerNumber.Initialize(iter.GetROMOffset(), iter.LoadDecimalNumber());

        int halfByteAddr{};

        int weightFactor, agility;
        halfByteAddr = iter.GetROMOffset();
        iter.LoadHalfByteNumbers(&weightFactor, &agility);
        p.WeightFactor.Initialize(halfByteAddr, weightFactor);
        p.BaseAgility.Initialize(halfByteAddr, agility);

        p.WeightInPounds = 140 + (weightFactor * 8);

        int speed, offAware;
        halfByteAddr = iter.GetROMOffset();
        iter.LoadHalfByteNumbers(&speed, &offAware);
        p.BaseSpeed.Initialize(halfByteAddr, speed);
        p.BaseOffAware.Initialize(halfByteAddr, offAware);

        int defAware, shotPower;
        halfByteAddr = iter.GetROMOffset();
        iter.LoadHalfByteNumbers(&defAware, &shotPower);
        p.BaseDefAware.Initialize(halfByteAddr, defAware);
        p.BaseShotPower.Initialize(halfByteAddr, shotPower);

        int checking, handednessValue;
        halfByteAddr = iter.GetROMOffset();
        iter.LoadHalfByteNumbers(&checking, &handednessValue);
        p.BaseChecking.Initialize(halfByteAddr, checking);
        p.HandednessValue.Initialize(halfByteAddr, handednessValue);
        p.WhichHandedness = handednessValue % 2 == 0 ? Handedness::Left : Handedness::Right;

        int stickHandling, shotAccuracy;
        halfByteAddr = iter.GetROMOffset();
        iter.LoadHalfByteNumbers(&stickHandling, &shotAccuracy);
        p.BaseStickHandling.Initialize(halfByteAddr, stickHandling);
        p.BaseShotAccuracy.Initialize(halfByteAddr, shotAccuracy);

        int endurance, roughnness;
        halfByteAddr = iter.GetROMOffset();
        iter.LoadHalfByteNumbers(&endurance, &roughnness);
        p.BaseEndurance.Initialize(halfByteAddr, endurance);
        p.Roughness.Initialize(halfByteAddr, roughnness);

        int passAcc, aggression;
        halfByteAddr = iter.GetROMOffset();
        iter.LoadHalfByteNumbers(&passAcc, &aggression);
        p.BasePassAccuracy.Initialize(halfByteAddr, passAcc);
        p.BaseAggression.Initialize(halfByteAddr, aggression);

        result.Players.push_back(p);
    }

    result.TeamCity.Initialize(iter.GetROMOffset(), iter.LoadROMString());
    result.Acronym.Initialize(iter.GetROMOffset(), iter.LoadROMString());
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
            unsigned char b0 = s_romData.Get(fileOffset + 0);
            unsigned char b1 = s_romData.Get(fileOffset + 1);
            unsigned char b2 = s_romData.Get(fileOffset + 2);
            unsigned char b3 = s_romData.Get(fileOffset + 3);

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

System::Void nhl94e::Form1::exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	this->Close();
}

System::String^ IntToCliString(int n)
{
    std::ostringstream strm;
    strm << n;
    return gcnew System::String(strm.str().c_str());
}

void nhl94e::Form1::AddTeamGridUI(TeamData const& team)
{
    // Declare variables
    DoubleBufferedDataGridView^ dataGridView1;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column0;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column1;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column2;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column3;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column4;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column5;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column6;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column7;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column8;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column9;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column10;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column11;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column12;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column13;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column14;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column15;
    System::Windows::Forms::DataGridViewTextBoxColumn^ Column16;

    // Create objects
    dataGridView1 = (gcnew DoubleBufferedDataGridView());
    Column0 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column4 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column5 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column6 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column7 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column8 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column9 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column10 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column11 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column12 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column13 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column14 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column15 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    Column16 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());

    dataGridView1->CellValidating += gcnew System::Windows::Forms::DataGridViewCellValidatingEventHandler(this, &nhl94e::Form1::OnCellValidating);
    dataGridView1->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &nhl94e::Form1::OnCellValueChanged);

    System::Windows::Forms::TabPage^ tabPage1;

    tabPage1 = (gcnew System::Windows::Forms::TabPage());

    tabPage1->SuspendLayout();

    this->tabControl1->Controls->Add(tabPage1);

    Column0->HeaderText = L"Idx";
    Column0->Name = L"Column0";
    Column0->Width = 25;
    Column0->ReadOnly = true;

    Column1->HeaderText = L"Player Name";
    Column1->Name = L"Column1";

    Column2->HeaderText = L"#";
    Column2->Name = L"Column2";
    Column2->Width = 25;

    Column3->HeaderText = L"Weight Class";
    Column3->Name = L"Column3";
    Column3->Width = 50;

    Column4->HeaderText = L"Agility";
    Column4->Name = L"Column4";
    Column4->Width = 40;

    Column5->HeaderText = L"Speed";
    Column5->Name = L"Column5";
    Column5->Width = 40;

    Column6->HeaderText = L"Off Aware";
    Column6->Name = L"Column6";
    Column6->Width = 40;

    Column7->HeaderText = L"Def Aware";
    Column7->Name = L"Column7";
    Column7->Width = 40;

    Column8->HeaderText = L"Shot Power";
    Column8->Name = L"Column8";
    Column8->Width = 40;

    Column9->HeaderText = L"Checking";
    Column9->Name = L"Column9";
    Column9->Width = 60;

    Column10->HeaderText = L"Hand";
    Column10->Name = L"Column10";
    Column10->Width = 40;

    Column11->HeaderText = L"Stick Handling";
    Column11->Name = L"Column11";
    Column11->Width = 60;

    Column12->HeaderText = L"Shot Acc";
    Column12->Name = L"Column12";
    Column12->Width = 40;

    Column13->HeaderText = L"Endurance";
    Column13->Name = L"Column13";
    Column13->Width = 70;

    Column14->HeaderText = L"Roughness";
    Column14->Name = L"Column14";
    Column14->Width = 70;

    Column15->HeaderText = L"Pass Acc";
    Column15->Name = L"Column15";
    Column15->Width = 40;

    Column16->HeaderText = L"Aggression";
    Column16->Name = L"Column16";
    Column16->Width = 70;

    tabPage1->Controls->Add(dataGridView1);
    tabPage1->Location = System::Drawing::Point(4, 22);
    tabPage1->Name = L"tabPage1";
    tabPage1->Padding = System::Windows::Forms::Padding(3);
    tabPage1->Size = System::Drawing::Size(401, 496);
    tabPage1->TabIndex = 0;
    System::String^ teamAcronym = gcnew System::String(team.Acronym.Get().c_str());
    tabPage1->Text = teamAcronym;
    tabPage1->UseVisualStyleBackColor = true;

    dataGridView1->AllowUserToAddRows = false;
    dataGridView1->AllowUserToDeleteRows = false;
    dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column0 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column1 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column2 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column3 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column4 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column5 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column6 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column7 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column8 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column9 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column10 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column11 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column12 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column13 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column14 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column15 });
    dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(1) { Column16 });
    dataGridView1->Dock = System::Windows::Forms::DockStyle::Fill;
    dataGridView1->Location = System::Drawing::Point(3, 3);
    dataGridView1->Name = L"dataGridView1";
    dataGridView1->Size = System::Drawing::Size(395, 490);
    dataGridView1->TabIndex = 0;

    dataGridView1->Rows->Clear();

    for (size_t i = 0; i < team.Players.size(); ++i)
    {
        PlayerData const& player = team.Players[i];

        System::Object^ playerIndex = i;
        System::String^ playerNameString = gcnew System::String(player.Name.Get().c_str());
        System::Object^ playerNumber = player.PlayerNumber.Get();
        System::Object^ weightClass = player.WeightFactor.Get();
        System::Object^ agility = player.BaseAgility.Get();
        System::Object^ speed = player.BaseSpeed.Get();
        System::Object^ offAware = player.BaseOffAware.Get();
        System::Object^ defAware = player.BaseDefAware.Get();
        System::Object^ shotPower = player.BaseShotPower.Get();
        System::Object^ checking = player.BaseChecking.Get();
        System::String^ handednessString = player.WhichHandedness == Handedness::Left ? L"L" : L"R";
        System::Object^ stickHandling = player.BaseStickHandling.Get();
        System::Object^ shotAcc = player.BaseShotAccuracy.Get();
        System::Object^ endurance = player.BaseEndurance.Get();
        System::Object^ roughness = player.Roughness.Get();
        System::Object^ passAcc = player.BasePassAccuracy.Get();
        System::Object^ aggression = player.BaseAggression.Get();

        dataGridView1->Rows->Add(gcnew cli::array<System::Object^>(17) 
        { 
            playerIndex,
            playerNameString, 
            playerNumber, 
            weightClass, 
            agility,
            speed,
            offAware,
            defAware,
            shotPower,
            checking,
            handednessString,
            stickHandling,
            shotAcc,
            endurance,
            roughness,
            passAcc,
            aggression
        });
    }

    tabPage1->ResumeLayout(false);
}

std::wstring ManagedToWideString(System::String^ s)
{
    array<wchar_t>^ arr = s->ToCharArray();

    std::wstring result;
    for (int i = 0; i < arr->Length; ++i)
    {
        result.push_back(arr[i]);
    }
    return result;
}

std::string ManagedToNarrowASCIIString(System::String^ s)
{
    array<wchar_t>^ arr = s->ToCharArray();

    std::string result;
    for (int i = 0; i < arr->Length; ++i)
    {
        wchar_t wide = arr[i];
        assert(wide < 256);
        char narrow = static_cast<char>(wide);
        result.push_back(narrow);
    }
    return result;
}

System::String^ NarrowASCIIStringToManaged(std::string const& s)
{
    return gcnew System::String(s.c_str());
}

void nhl94e::Form1::OpenROM(std::wstring romFilename)
{
    s_romData.LoadBytesFromFile(romFilename.c_str());

    if (!s_romData.EnsureExpandedSize())
        return;

    s_romData.InitializeWriteabilityMask();

    s_allTeams = LoadPlayerNamesAndStats();

    for (int teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& team = s_allTeams[teamIndex];
        AddTeamGridUI(team);
    }

    tabControl1->SelectedIndex = (int)Team::Montreal;

    this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &nhl94e::Form1::OnSelectedIndexChanged);
    OnSelectedIndexChanged(nullptr, nullptr);
}

System::Void nhl94e::Form1::openROMToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
    s_allTeams.clear();
    this->tabControl1->Controls->Clear();

    OpenFileDialog^ dialog = gcnew OpenFileDialog();
#if _DEBUG
    dialog->FileName = L"E:\\Emulation\\SNES\\Images\\nhl94e.sfc";
#endif

    System::Windows::Forms::DialogResult result = dialog->ShowDialog();
    if (result != System::Windows::Forms::DialogResult::OK)
        return;

    std::wstring romFilename = ManagedToWideString(dialog->FileName);

    OpenROM(romFilename);
}

System::Void nhl94e::Form1::Form1_Load(System::Object^ sender, System::EventArgs^ e)
{
#if _DEBUG
    OpenROM(L"E:\\Emulation\\SNES\\Images\\nhl94.sfc");
#endif
}

unsigned char ChrToHex(wchar_t ch)
{
    if (ch >= L'0' && ch <= L'9')
    {
        return ch - L'0';
    }
    else if (ch >= L'A' && ch <= L'F')
    {
        return ch - L'A' + 10;
    }
    else
    {
        assert(false); //Unexpected value
        return 0;
    }
}

unsigned char StrToHex(std::wstring s)
{
    assert(s.length() == 2);
    unsigned char d0 = ChrToHex(s[0]);
    unsigned char d1 = ChrToHex(s[1]);
    return (d0 * 16) + d1;
}

std::vector<unsigned char> LoadAsmFromDebuggerText(std::wstring fileName)
{
    std::vector<unsigned char> code;

    std::wifstream f(fileName);
    std::vector<std::wstring> lines;
    while (f.good())
    {
        std::wstring line;
        std::getline(f, line);

        size_t commentIndex = line.find(L"//");
        std::wstring uncomment = line.substr(0, commentIndex);
        if (uncomment.length() == 0)
            continue;

        std::wstring removeAddress;
        if (uncomment[0] == L'$')
        {
            removeAddress = uncomment.substr(9);
        }
        else
        {
            removeAddress = uncomment;
        }

        if (removeAddress[0] == L'\t' || removeAddress[0] == L' ')
            continue;

        std::wistringstream strm(removeAddress);
        std::wstring tokens[4];

        strm >> tokens[0];
        strm >> tokens[1];
        strm >> tokens[2];
        strm >> tokens[3];

        for (int i = 0; i < 4; ++i)
        {
            if (tokens[i].length() > 2)
                break;

            unsigned char b = StrToHex(tokens[i]);

            code.push_back(b);
        }
    }


    return code;
}

bool InsertDetour(
    wchar_t const* codeToInsert,
    int detourSrcStartROMAddress,
    int detourSrcEndROMAddress,
    int detourDestROMAddress)
{
    std::vector<unsigned char> decompressProfileMain = LoadAsmFromDebuggerText(codeToInsert);

    if (decompressProfileMain.size() == 0)
        return false;

    // Quick parameter checking
    int detourLength = detourSrcEndROMAddress - detourSrcStartROMAddress + 1; // Addresses are inclusive
    assert(detourLength >= 4); // Need enough room to insert a long jump

    int fileOffsetSrcStart = ROMAddressToFileOffset(detourSrcStartROMAddress);
    int fileOffsetSrcEnd = ROMAddressToFileOffset(detourSrcEndROMAddress); // Inclusive

    int fileOffsetDst = ROMAddressToFileOffset(detourDestROMAddress);

    for (size_t i = 0; i < decompressProfileMain.size(); ++i)
    {
        s_romData.SetROMData(fileOffsetDst + i, decompressProfileMain[i]);
    }

    for (int i = fileOffsetSrcStart; i <= fileOffsetSrcEnd; ++i) // Good hygiene
    {
        s_romData.SetROMData(i, 0xEA); // NOP
    }

    // Insert JMP to detour payload
    // Detour payload author is responsible for JMPing back or they can RTL.
    int detourB2 = detourDestROMAddress & 0xFF;
    detourDestROMAddress >>= 8;
    int detourB1 = detourDestROMAddress & 0xFF;
    detourDestROMAddress >>= 8;
    int detourB0 = detourDestROMAddress & 0xFF;
    detourDestROMAddress >>= 8;
    assert(detourDestROMAddress == 0);

    s_romData.SetROMData(fileOffsetSrcStart, 0x5C); // JMP
    s_romData.SetROMData(fileOffsetSrcStart + 1, detourB2);
    s_romData.SetROMData(fileOffsetSrcStart + 2, detourB1);
    s_romData.SetROMData(fileOffsetSrcStart + 3, detourB0);

    return true;
}

bool InsertTeamLocationText()
{
    struct TeamRename
    {
        Team WhichTeam;
        std::string OriginalName;
        std::string NewName;
    };

    std::vector<TeamRename> renames;
    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        if (teamData.TeamCity.IsChanged())
        {
            TeamRename r;
            r.WhichTeam = (Team)teamIndex;
            r.NewName = teamData.TeamCity.Get();
            renames.push_back(r);
        }
    }

    if (renames.size() == 0)
        return true; // Nothing to do

    // Need to do code patching
    std::vector<int> stringAddresses;
    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        stringAddresses.push_back(teamData.TeamCity.SourceROMAddress);
    }

    int stringPtrTableStart = 0xA08200;
    int stringAddressTableSize = stringAddresses.size() * 4;
    int freeSpaceForStrings = stringPtrTableStart + stringAddressTableSize;

    // Put string after the table
    {
        RomDataIterator datastreamIter(ROMAddressToFileOffset(freeSpaceForStrings));

        for (int i = 0; i < renames.size(); ++i)
        {
            stringAddresses[(int)renames[i].WhichTeam] = datastreamIter.GetROMOffset();
            datastreamIter.SaveROMString(renames[i].NewName);
        }
    }

    // Put table at 0xA08200
    {
        assert(stringPtrTableStart == 0xA08200);
        RomDataIterator datastreamIter(ROMAddressToFileOffset(0xA08200));

        int dstROMAddress = stringPtrTableStart;
        int dstFileOffset = ROMAddressToFileOffset(dstROMAddress);

        unsigned char* stringAddressData = reinterpret_cast<unsigned char*>(stringAddresses.data());
        for (int i = 0; i < stringAddressTableSize; ++i)
        {
            s_romData.Set(dstFileOffset + i, stringAddressData[i]);
        }
    }

    // Insert code overtop 9B/C5AB -> 9B/C5E6 with noops in the extra space
    {
        std::vector<unsigned char> decompressProfileMain = LoadAsmFromDebuggerText(L"LookupTeamLocationStringAddress.asm");

        int dstStartROMAddress = 0x9BC5AB;
        int dstEndROMAddress = 0x9BC5E6;
        int dstFileOffsetStart = ROMAddressToFileOffset(dstStartROMAddress);
        int dstFileOffsetEnd = ROMAddressToFileOffset(dstEndROMAddress);

        int patchIndex = 0;

        for (int fileOffset = dstFileOffsetStart; fileOffset <= dstFileOffsetEnd; ++fileOffset)
        {
            if (patchIndex < decompressProfileMain.size())
            {
                s_romData.SetROMData(fileOffset, decompressProfileMain[patchIndex]);
                patchIndex++;
            }
            else
            {
                s_romData.SetROMData(fileOffset, 0xEA); // NOP
            }
        }
    }

    return true;
}


bool InsertTeamAcronymText()
{
    struct TeamRename
    {
        Team WhichTeam;
        std::string OriginalAcronym;
        std::string NewAcronym;
    };

    std::vector<TeamRename> renames;
    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        if (teamData.Acronym.IsChanged())
        {
            TeamRename r;
            r.WhichTeam = (Team)teamIndex;
            r.NewAcronym = teamData.Acronym.Get();
            renames.push_back(r);
        }
    }

    if (renames.size() == 0)
        return true; // Nothing to do

    // Need to do code patching
    std::vector<int> stringAddresses;
    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        stringAddresses.push_back(teamData.Acronym.SourceROMAddress);
    }

    int stringPtrTableStart = 0xA08200;
    int stringAddressTableSize = stringAddresses.size() * 4;
    int freeSpaceForStrings = stringPtrTableStart + stringAddressTableSize;

    // Put string after the table
    {
        RomDataIterator datastreamIter(ROMAddressToFileOffset(freeSpaceForStrings));

        for (int i = 0; i < renames.size(); ++i)
        {
            stringAddresses[(int)renames[i].WhichTeam] = datastreamIter.GetROMOffset();
            datastreamIter.SaveROMString(renames[i].NewAcronym);
        }
    }

    // Put table at 0xA08200
    {
        assert(stringPtrTableStart == 0xA08200);
        RomDataIterator datastreamIter(ROMAddressToFileOffset(0xA08200));

        int dstROMAddress = stringPtrTableStart;
        int dstFileOffset = ROMAddressToFileOffset(dstROMAddress);

        unsigned char* stringAddressData = reinterpret_cast<unsigned char*>(stringAddresses.data());
        for (int i = 0; i < stringAddressTableSize; ++i)
        {
            s_romData.Set(dstFileOffset + i, stringAddressData[i]);
        }
    }

    // Insert code overtop 9B/C5AB -> 9B/C5E6 with noops in the extra space
    {
        std::vector<unsigned char> decompressProfileMain = LoadAsmFromDebuggerText(L"LookupTeamLocationStringAddress.asm");

        int dstStartROMAddress = 0x9BC5AB;
        int dstEndROMAddress = 0x9BC5E6;
        int dstFileOffsetStart = ROMAddressToFileOffset(dstStartROMAddress);
        int dstFileOffsetEnd = ROMAddressToFileOffset(dstEndROMAddress);

        int patchIndex = 0;

        for (int fileOffset = dstFileOffsetStart; fileOffset <= dstFileOffsetEnd; ++fileOffset)
        {
            if (patchIndex < decompressProfileMain.size())
            {
                s_romData.SetROMData(fileOffset, decompressProfileMain[patchIndex]);
                patchIndex++;
            }
            else
            {
                s_romData.SetROMData(fileOffset, 0xEA); // NOP
            }
        }
    }

    return true;
}

struct PlayerRename
{
    Team WhichTeam;
    int PlayerIndex;
    ModifiableStat<std::string> Name;
    ModifiableStat<int> PlayerNumber;
};
void AddLookupPlayerNamePointerTables(std::vector<PlayerRename> const& renames)
{
    std::vector<TeamData> allTeams = LoadPlayerNamesAndStats();

    // There are two lookup tables involved.

    // Table 1) the "alternate main table"
    // Key: Team index
    // Key range: [0x0-0x1B]. See the enum, "Team"
    // Value : A long pointer.
    // Value range: Pointers of the form "0xA8xxxx"
    // Element size: 4 bytes
    // Purpose of table:
    //     Alternate main table.
    //     Maps from "team index" to "team data table", described as Table 2) below. There is a table for each team. This includes the two 
    //     All Stars teams.
    //
    // Table 2)
    // Key: Player index
    // Key range: [0-0x19] The most players on a given team is 25 = 0x19.
    // Value: A long pointer
    // Value range: By default, pointers of the form "0x9Cxxxx". But you can change this to any long pointer you want, including those in expanded ROM space.
    // Element size: 4 bytes
    // Purpose of table:
    //     Maps from "player index" of a given team to that player's name and number.
    //     Change elements of this table to freely give characters new names, even longer ones.
    //     Stats metadata don't need to come after the name and number. They aren't used by the function which dereferences this
    //     For stats I prefer modifying them without all this relocation business. It's fixed length non string data so who cares. Also I would need to
    //     look up all the places that touch them to feel good about it and I can't be bothered.
    //
    // At the end of the tables is a data stream of non-fixed length.
    //
    // In general this approach is memory-optimized for the situation where you plan to modify some players but not all of them.
    // In the datastream you store just the players whose names need to be patched on top.

    // First, plan out where each of the tables will be so that things are nice and compact.

    const int firstTableLocation = 0xA8D000;
    int firstPointerTableSize = static_cast<int>(allTeams.size()) * 4;

    const int secondTableLocation = 0xA8D070;
    assert(secondTableLocation == firstTableLocation + firstPointerTableSize);
    const int totalPlayerCount = 653;
    const int secondTableTotalSizes = totalPlayerCount * 4;

    const int datastreamLocation = 0xa8daa4;
    assert(datastreamLocation == secondTableLocation + secondTableTotalSizes);

    // Write the datastream
    {
        RomDataIterator datastreamIter(ROMAddressToFileOffset(datastreamLocation));
        for (int i = 0; i < renames.size(); ++i)
        {
            // Look up the person being renamed
            TeamData* team = &allTeams[(int)renames[i].WhichTeam];
            PlayerData* playerData = &team->Players[renames[i].PlayerIndex];

            playerData->ReplacedROMAddressForRename = datastreamIter.GetROMOffset();

            datastreamIter.SaveROMString(renames[i].Name.Get());
            datastreamIter.SaveDecimalNumber(renames[i].PlayerNumber.Get(), false);
        }
    }

    // Write the first and second tables
    RomDataIterator firstTableIter(ROMAddressToFileOffset(firstTableLocation));
    RomDataIterator secondTableIter(ROMAddressToFileOffset(secondTableLocation));
    for (int teamIndex = 0; teamIndex < allTeams.size(); ++teamIndex)
    {
        const TeamData& team = allTeams[teamIndex];

        int teamPointerTableEntry = secondTableIter.GetROMOffset();
        firstTableIter.SaveLongAddress4Bytes(teamPointerTableEntry);

        for (int playerIndex = 0; playerIndex < team.Players.size(); ++playerIndex)
        {
            const PlayerData& player = team.Players[playerIndex];

            if (player.ReplacedROMAddressForRename != 0)
            {
                secondTableIter.SaveLongAddress4Bytes(player.ReplacedROMAddressForRename);
            }
            else
            {
                secondTableIter.SaveLongAddress4Bytes(player.OriginalROMAddress);
            }
        }
    }
}

bool InsertPlayerNameText()
{
    std::vector<PlayerRename> renames;

    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        for (size_t playerIndex = 0; playerIndex < teamData.Players.size(); ++playerIndex)
        {
            PlayerData const& player = teamData.Players[playerIndex];
            if (player.Name.IsChanged())
            {
                PlayerRename r{};
                r.Name = player.Name;
                r.PlayerIndex = playerIndex;
                r.PlayerNumber = player.PlayerNumber;
                r.WhichTeam = (Team)teamIndex;
                renames.push_back(r);
            }
        }
    }

    if (renames.size() == 0)
        return true; // Nothing to do

    bool detourPatched = InsertDetour(L"LookupPlayerNameDet.asm", 0x9FC732, 0x9FC756, 0xA08100);
    if (!detourPatched)
        return detourPatched;

    AddLookupPlayerNamePointerTables(renames); // The above code depends on these tables.

    return true;
}

System::Void nhl94e::Form1::saveROMToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
    SaveFileDialog^ dialog = gcnew SaveFileDialog();
#if _DEBUG
    dialog->FileName = L"E:\\Emulation\\SNES\\Images\\Test\\nhl94em.sfc";
#endif
    System::Windows::Forms::DialogResult result = dialog->ShowDialog();
    if (result != System::Windows::Forms::DialogResult::OK)
        return;

    std::wstring outputFilename = ManagedToWideString(dialog->FileName);
    
    // Write stats
    for (int teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData& team = s_allTeams[teamIndex];

        for (int playerIndex = 0; playerIndex < team.Players.size(); ++playerIndex)
        {
            PlayerData& player = team.Players[playerIndex];
            RomDataIterator iter(ROMAddressToFileOffset(player.OriginalROMAddress));

            iter.SkipROMString(); // player name

            iter.SaveDecimalNumber(player.PlayerNumber.Get(), true);
            iter.SaveHalfByteNumbers(player.WeightFactor.Get(), player.BaseAgility.Get(), true);
            iter.SaveHalfByteNumbers(player.BaseSpeed.Get(), player.BaseOffAware.Get(), true);
            iter.SaveHalfByteNumbers(player.BaseDefAware.Get(), player.BaseShotPower.Get(), true);
            iter.SaveHalfByteNumbers(player.BaseChecking.Get(), player.HandednessValue.Get(), true);
            iter.SaveHalfByteNumbers(player.BaseStickHandling.Get(), player.BaseShotAccuracy.Get(), true);
            iter.SaveHalfByteNumbers(player.BaseEndurance.Get(), player.Roughness.Get(), true);
            iter.SaveHalfByteNumbers(player.BasePassAccuracy.Get(), player.BaseAggression.Get(), true);
        }
    }

    PlayerRenamer playerRenamer;
    TeamLocationRenamer teamLocationRenamer;
    TeamAcronymRenamer teamAcronymRenamer;

    playerRenamer.AllocateTable(); // Figures out whether it should detour code and add a table. 
    teamLocationRenamer.AllocateTable();
    teamAcronymRenamer.AllocateTable();

    playerRenamer.WriteDynamicallySizedData();
    teamLocationRenamer.WriteDynamicallySizedData();
    teamAcronymRenamer.WriteDynamicallySizedData();

    // Write renamed location strings
    if (!InsertTeamLocationText())
    {
        System::String^ dialogString = gcnew System::String(L"Encountered an error loading the contents of the file LookupTeamLocationStringAddress.asm.");
        MessageBox::Show(dialogString);
        return;
    }

    // Write renamed players' names
    if (!InsertPlayerNameText())
    {
        System::String^ dialogString = gcnew System::String(L"Encountered an error loading the contents of the file LookupPlayerNameDet.asm.");
        MessageBox::Show(dialogString);
        return;
    }

    s_romData.SaveBytesToFile(outputFilename.c_str());

    MessageBox::Show(L"Output file saved.", L"Info");
}

bool IsValidPlayerName(System::String^ name)
{
    // Name must consist of alphanumeric letters, space and period
    for (int i = 0; i < name->Length; ++i)
    {
        wchar_t ch = name[i];
        bool valid =
            (ch >= L'a' && ch <= L'z') ||
            (ch >= L'A' && ch <= L'Z') ||
            ch == L' ' ||
            ch == L'.';
        if (!valid)
            return false;
    }

    return true;
}

void nhl94e::Form1::OnCellValidating(System::Object^ sender, System::Windows::Forms::DataGridViewCellValidatingEventArgs^ e)
{
    WhichStat stat = (WhichStat)e->ColumnIndex;
    int r = 0;

    switch (stat)
    {
    case WhichStat::PlayerName:
        {
            if (!IsValidPlayerName((System::String^)e->FormattedValue))
            {
                MessageBox::Show(L"Encountered an invalid character in player name. Valid characters are alphabetic, space and period.", L"Info");
                e->Cancel = true;
            }
            break;
        }
    case WhichStat::PlayerNumber:
        {
            if (!int::TryParse((System::String^)e->FormattedValue, r) || r < 1 || r > 99)
            {
                MessageBox::Show(L"Please enter a number between 1 and 99, inclusive.", L"Info");
                e->Cancel = true;
            }
            break;
        }
    case WhichStat::WeightClass:
        {
            if (!int::TryParse((System::String^)e->FormattedValue, r) || r < 0 || r > 16)
            {
                MessageBox::Show(L"Please enter a number between 0 and 16, inclusive.", L"Info");
                e->Cancel = true;
            }
            break;
        }
    case WhichStat::Handedness:
        {
            // L or R
            String^ valueString = (String^)e->FormattedValue;
            if (valueString != "L" && valueString != "R")
            {
                MessageBox::Show(L"Please enter either L or R.", L"Info");
                e->Cancel = true;
            }
            break;
        }
    case WhichStat::Agility:
    case WhichStat::Speed:
    case WhichStat::OffAware:
    case WhichStat::DefAware:
    case WhichStat::ShotPower:
    case WhichStat::Checking:
    case WhichStat::StickHandling:
    case WhichStat::ShotAccuracy:
    case WhichStat::Endurance:
    case WhichStat::Roughness:
    case WhichStat::PassAccuracy:
    case WhichStat::Aggression:
        {
            if (!int::TryParse((System::String^)e->FormattedValue, r) || r < 0 || r > 6)
            {
                MessageBox::Show(L"Please enter a number between 0 and 6, inclusive.", L"Info");
                e->Cancel = true;
            }
            break;
        }
    case WhichStat::PlayerIndex:
        // Can't change this
    default:
        ;
    }
}


void TryCommitStatChange(
    System::Windows::Forms::DataGridView^ view,
    int teamIndex,
    int rowIndex,
    int whichStatIndex,
    int minAllowedValue,
    int maxAllowedValue)
{
    System::Object^ value = view->Rows[rowIndex]->Cells[whichStatIndex]->Value;

    int n = 0;
    if (!int::TryParse((System::String^)value, n))
        return;

    if (n < minAllowedValue || n > maxAllowedValue)
        return;

    // Commit new value to s_allTeams
    unsigned __int64 playerIndex = (unsigned __int64)(view->Rows[rowIndex]->Cells[(int)WhichStat::PlayerIndex]->Value);

    s_allTeams[teamIndex].Players[playerIndex].SetNumericalStat((WhichStat)whichStatIndex, n);

    if (s_allTeams[teamIndex].Players[playerIndex].IsNumericalStatChanged((WhichStat)whichStatIndex))
    {
        view->Rows[rowIndex]->Cells[whichStatIndex]->Style->BackColor = System::Drawing::Color::LightBlue;
    }
    else
    {
        view->Rows[rowIndex]->Cells[whichStatIndex]->Style->BackColor = System::Drawing::Color::White;
    }
}

void UpdateGridCellBackgroundColor(
    System::Windows::Forms::DataGridViewCell^ gridCell,
    PlayerData* player,
    WhichStat whichStat)
{
    if (player->IsNumericalStatChanged(whichStat))
    {
        gridCell->Style->BackColor = System::Drawing::Color::LightBlue;
    }
    else
    {
        gridCell->Style->BackColor = System::Drawing::Color::White;
    }
}

void TryCommitHandednessStatChange(
    System::Windows::Forms::DataGridView^ view,
    int teamIndex,
    int rowIndex)
{
    System::Object^ value = view->Rows[rowIndex]->Cells[(int)WhichStat::Handedness]->Value;
    System::String^ stringValue = (System::String^)value;

    if (stringValue != "L" && stringValue != "R")
        return;

    // Commit new value to s_allTeams
    unsigned __int64 playerIndex = (unsigned __int64)(view->Rows[rowIndex]->Cells[(int)WhichStat::PlayerIndex]->Value);

    int n = value == "L" ? 0 : 2;

    s_allTeams[teamIndex].Players[playerIndex].SetNumericalStat(WhichStat::Handedness, n);

    UpdateGridCellBackgroundColor(
        view->Rows[rowIndex]->Cells[(int)WhichStat::Handedness],
        &s_allTeams[teamIndex].Players[playerIndex],
        WhichStat::Handedness);
}

void TryCommitPlayerNameChange(
    System::Windows::Forms::DataGridView^ view,
    int teamIndex,
    int rowIndex)
{
    System::Windows::Forms::DataGridViewCell^ gridCell = view->Rows[rowIndex]->Cells[(int)WhichStat::PlayerName];
    System::Object^ value = gridCell->Value;
    System::String^ stringValue = (System::String^)value;

    if (!IsValidPlayerName(stringValue))
        return;

    unsigned __int64 playerIndex = (unsigned __int64)(view->Rows[rowIndex]->Cells[(int)WhichStat::PlayerIndex]->Value);

    PlayerData* player = &s_allTeams[teamIndex].Players[playerIndex];

    std::string s = ManagedToNarrowASCIIString(stringValue);
    s_allTeams[teamIndex].Players[playerIndex].Name.Set(s);

    if (player->Name.IsChanged())
    {
        gridCell->Style->BackColor = System::Drawing::Color::LightBlue;
    }
    else
    {
        gridCell->Style->BackColor = System::Drawing::Color::White;
    }
}

void nhl94e::Form1::OnCellValueChanged(System::Object^ sender, System::Windows::Forms::DataGridViewCellEventArgs^ e)
{
    int teamIndex = tabControl1->SelectedIndex;
    int rowIndex = e->RowIndex;
    int whichStatIndex = e->ColumnIndex;

    DataGridView^ view = (DataGridView^)sender;    
    
    if (whichStatIndex == (int)WhichStat::WeightClass)
    {
        TryCommitStatChange(view, teamIndex, rowIndex, whichStatIndex, 0, 16);
    }
    else if (whichStatIndex == (int)WhichStat::PlayerNumber)
    {
        TryCommitStatChange(view, teamIndex, rowIndex, whichStatIndex, 1, 99);
    }
    else if (whichStatIndex == (int)WhichStat::Handedness)
    {
        TryCommitHandednessStatChange(view, teamIndex, rowIndex);
    }
    else if (whichStatIndex >= (int)WhichStat::Agility && whichStatIndex <= (int)WhichStat::Aggression)
    {
        TryCommitStatChange(view, teamIndex, rowIndex, whichStatIndex, 0, 6);
    }
    else if (whichStatIndex == (int)WhichStat::PlayerName)
    {
        TryCommitPlayerNameChange(view, teamIndex, rowIndex);
    }
}

void nhl94e::Form1::OnSelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;    

    // Refresh what's in the team data pane
    locationTextBox->Text = NarrowASCIIStringToManaged(s_allTeams[teamIndex].TeamCity.Get());
    acronymTextBox->Text = NarrowASCIIStringToManaged(s_allTeams[teamIndex].Acronym.Get());
    teamNameTextBox->Text = NarrowASCIIStringToManaged(s_allTeams[teamIndex].TeamName);
    teamVenueTextBox->Text = NarrowASCIIStringToManaged(s_allTeams[teamIndex].Venue);
}

void nhl94e::Form1::locationTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;
    std::string newName = ManagedToNarrowASCIIString(locationTextBox->Text);
    s_allTeams[teamIndex].TeamCity.Set(newName);
}