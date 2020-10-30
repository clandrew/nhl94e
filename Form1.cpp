#include "pch.h"
#include "Form1.h"
#include "AddressMapping.h"
#include "TeamData.h"
#include "Utils.h"

static std::vector<TeamData> s_allTeams;
static std::vector<unsigned char> s_romData;

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
        unsigned char headerLength0 = s_romData[m_fileOffset + 0];
        unsigned char headerLength1 = s_romData[m_fileOffset + 1];
        m_fileOffset += 2;

        assert(headerLength0 > 0x2 && headerLength1 == 0x0);

        int headerLengthPlusLengthWord = (headerLength1 << 8) | (headerLength0);
        assert(headerLengthPlusLengthWord > 2);
        int headerLength = headerLengthPlusLengthWord - 2;

        m_fileOffset += headerLength;
    }

    std::vector<unsigned char> LoadHeader()
    {
        unsigned char headerLength0 = s_romData[m_fileOffset + 0];
        unsigned char headerLength1 = s_romData[m_fileOffset + 1];
        m_fileOffset += 2;

        assert(headerLength0 > 0x2 && headerLength1 == 0x0);

        int headerLengthPlusLengthWord = (headerLength1 << 8) | (headerLength0);
        assert(headerLengthPlusLengthWord > 2);
        int headerLength = headerLengthPlusLengthWord - 2;

        std::vector<unsigned char> result;
        for (int i = 0; i < headerLength; ++i)
        {
            result.push_back(s_romData[m_fileOffset + i]);
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

        s_romData[m_fileOffset + 0] = headerLengthPlusLengthWord0;
        s_romData[m_fileOffset + 1] = headerLengthPlusLengthWord1;
        m_fileOffset += 2;

        for (int i = 0; i < headerLength; ++i)
        {
            s_romData[m_fileOffset + i] = header[i];
        }
        m_fileOffset += headerLength;
    }

    std::string LoadROMString()
    {
        unsigned char stringLength0 = s_romData[m_fileOffset + 0];
        unsigned char stringLength1 = s_romData[m_fileOffset + 1];
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
            result.push_back(s_romData[m_fileOffset + i]);
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

        s_romData[m_fileOffset + 0] = strLengthPlusLengthWord0;
        s_romData[m_fileOffset + 1] = strLengthPlusLengthWord1;
        m_fileOffset += 2;

        for (int i = 0; i < strLength; ++i)
        {
            s_romData[m_fileOffset + i] = str[i];
        }
        m_fileOffset += strLength;
    }

    void SkipROMString()
    {
        unsigned char stringLength0 = s_romData[m_fileOffset + 0];
        unsigned char stringLength1 = s_romData[m_fileOffset + 1];
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
        unsigned char numberByte = s_romData[m_fileOffset];
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
        s_romData[m_fileOffset] = resultByte;
        ++m_fileOffset;
    }

    void SkipDecimalNumber()
    {
        ++m_fileOffset;
    }

    void LoadHalfByteNumbers(int* a, int* b)
    {
        unsigned char stat = s_romData[m_fileOffset];
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
        s_romData[m_fileOffset] = resultByte;
        ++m_fileOffset;
    }

    void SkipHalfByteNumbers()
    {
        ++m_fileOffset;
    }

    void SaveDelimiter()
    {
        s_romData[m_fileOffset] = 0x02;
        ++m_fileOffset;
        s_romData[m_fileOffset] = 0x00;
        ++m_fileOffset;
    }

    void SaveLongAddress4Bytes(int addr) // big endian-to-little-endian
    {
        s_romData[m_fileOffset] = addr & 0xFF;
        addr >>= 8;
        ++m_fileOffset;

        s_romData[m_fileOffset] = addr & 0xFF;
        addr >>= 8;
        ++m_fileOffset;

        s_romData[m_fileOffset] = addr & 0xFF;
        addr >>= 8;
        ++m_fileOffset;

        s_romData[m_fileOffset] = 0;
        ++m_fileOffset;

        assert(addr == 0); // 24-byte address expected
    }

    void SaveByte(int b)
    {
        assert(b > 0 && b < 256);
        s_romData[m_fileOffset] = b;
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

        p.PlayerNumber.Initialize(iter.LoadDecimalNumber());

        int weightFactor, agility;
        iter.LoadHalfByteNumbers(&weightFactor, &agility);
        p.WeightFactor.Initialize(weightFactor);
        p.BaseAgility.Initialize(agility);

        p.WeightInPounds = 140 + (weightFactor * 8);

        int speed, offAware;
        iter.LoadHalfByteNumbers(&speed, &offAware);
        p.BaseSpeed.Initialize(speed);
        p.BaseOffAware.Initialize(offAware);

        int defAware, shotPower;
        iter.LoadHalfByteNumbers(&defAware, &shotPower);
        p.BaseDefAware.Initialize(defAware);
        p.BaseShotPower.Initialize(shotPower);

        int checking, handednessValue;
        iter.LoadHalfByteNumbers(&checking, &handednessValue);
        p.BaseChecking.Initialize(checking);
        p.HandednessValue.Initialize(handednessValue);
        p.WhichHandedness = handednessValue % 2 == 0 ? Handedness::Left : Handedness::Right;

        int stickHandling, shotAccuracy;
        iter.LoadHalfByteNumbers(&stickHandling, &shotAccuracy);
        p.BaseStickHandling.Initialize(stickHandling);
        p.BaseShotAccuracy.Initialize(shotAccuracy);

        int endurance, roughnness;
        iter.LoadHalfByteNumbers(&endurance, &roughnness);
        p.BaseEndurance.Initialize(endurance);
        p.Roughness.Initialize(roughnness);

        int passAcc, aggression;
        iter.LoadHalfByteNumbers(&passAcc, &aggression);
        p.BasePassAccuracy.Initialize(passAcc);
        p.BaseAggression.Initialize(aggression);

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
            unsigned char b0 = s_romData[fileOffset + 0];
            unsigned char b1 = s_romData[fileOffset + 1];
            unsigned char b2 = s_romData[fileOffset + 2];
            unsigned char b3 = s_romData[fileOffset + 3];

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

System::String^ IntToCliString(int n)
{
    std::ostringstream strm;
    strm << n;
    return gcnew System::String(strm.str().c_str());
}

void CppCLRWinformsProjekt::Form1::AddTeamGridUI(TeamData const& team)
{
    // Declare variables
    System::Windows::Forms::DataGridView^ dataGridView1;
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
    dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
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

    dataGridView1->CellValidating += gcnew System::Windows::Forms::DataGridViewCellValidatingEventHandler(this, &CppCLRWinformsProjekt::Form1::OnCellValidating);
    dataGridView1->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &CppCLRWinformsProjekt::Form1::OnCellValueChanged);

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
    Column1->ReadOnly = true;

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
    Column10->ReadOnly = true;

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
    System::String^ teamAcronym = gcnew System::String(team.Acronym.c_str());
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
        System::String^ playerNameString = gcnew System::String(player.Name.c_str());
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

System::Void CppCLRWinformsProjekt::Form1::openROMToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
    s_allTeams.clear();
    s_romData.clear();

    OpenFileDialog^ dialog = gcnew OpenFileDialog();
    System::Windows::Forms::DialogResult result = dialog->ShowDialog();
    if (result != System::Windows::Forms::DialogResult::OK)
        return;

    //std::wstring romFilename = L"E:\\Emulation\\SNES\\Images\\nhl94e.sfc";

    std::wstring romFilename = ManagedToWideString(dialog->FileName);

    s_romData = LoadBytesFromFile(romFilename.c_str());

    // Check size
    size_t expectedSize = 0x400000;
    if (s_romData.size() != expectedSize)
    {
        std::wostringstream sstream;
        sstream << "This program is designed to work on a Super Nintendo NHL '94 ROM which has been expanded to 32 Mbit (4MB).\n";
        sstream << "Use Lunar Expand or other similar tools to perform expansion.\n";
        sstream << "Unexpected size detected: found " << s_romData.size() << " bytes, expected " << expectedSize << " bytes.\n";

        System::String^ dialogString = gcnew System::String(sstream.str().c_str());
        MessageBox::Show(dialogString);
        return;
    }

    s_allTeams = LoadPlayerNamesAndStats();

    for (int teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& team = s_allTeams[teamIndex];
        AddTeamGridUI(team);
    }

    tabControl1->SelectedIndex = (int)Team::Montreal;
}

System::Void CppCLRWinformsProjekt::Form1::Form1_Load(System::Object^ sender, System::EventArgs^ e)
{
}

System::Void CppCLRWinformsProjekt::Form1::saveROMToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) 
{
    std::wstring outputFilename = L"E:\\Emulation\\SNES\\Images\\nhl94em.sfc";
    
    for (int teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData& team = s_allTeams[teamIndex];

        for (int playerIndex = 0; playerIndex < team.Players.size(); ++playerIndex)
        {
            PlayerData& player = team.Players[playerIndex];
            RomDataIterator iter(ROMAddressToFileOffset(player.OriginalROMAddress));

            iter.SkipROMString(); // player name

            iter.SaveDecimalNumber(player.PlayerNumber.Get());
            iter.SaveHalfByteNumbers(player.WeightFactor.Get(), player.BaseAgility.Get());
            iter.SaveHalfByteNumbers(player.BaseSpeed.Get(), player.BaseOffAware.Get());
            iter.SaveHalfByteNumbers(player.BaseDefAware.Get(), player.BaseShotPower.Get());
            iter.SaveHalfByteNumbers(player.BaseChecking.Get(), player.HandednessValue.Get());
            iter.SaveHalfByteNumbers(player.BaseStickHandling.Get(), player.BaseShotAccuracy.Get());
            iter.SaveHalfByteNumbers(player.BaseEndurance.Get(), player.Roughness.Get());
            iter.SaveHalfByteNumbers(player.BasePassAccuracy.Get(), player.BaseAggression.Get());
        }
    }

    SaveBytesToFile(outputFilename.c_str(), s_romData);

    MessageBox::Show(L"Output file saved.", L"Info");
}

void CppCLRWinformsProjekt::Form1::OnCellValidating(System::Object^ sender, System::Windows::Forms::DataGridViewCellValidatingEventArgs^ e)
{
    WhichStat stat = (WhichStat)e->ColumnIndex;
    int r = 0;

    switch (stat)
    {
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
            if (e->FormattedValue != "L" && e->FormattedValue != "R")
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
    case WhichStat::PlayerName:
        // Can't change this (for now)
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
    if (int::TryParse((System::String^)value, n))
    {
        if (n >= minAllowedValue && n <= maxAllowedValue)
        {
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
    }
}

void CppCLRWinformsProjekt::Form1::OnCellValueChanged(System::Object^ sender, System::Windows::Forms::DataGridViewCellEventArgs^ e)
{
    int teamIndex = tabControl1->SelectedIndex;
    int rowIndex = e->RowIndex;
    int whichStatIndex = e->ColumnIndex;

    DataGridView^ view = (DataGridView^)sender;
    
    if (whichStatIndex >= (int)WhichStat::Agility && whichStatIndex <= (int)WhichStat::Aggression)
    {
        TryCommitStatChange(view, teamIndex, rowIndex, whichStatIndex, 0, 6);
    }
    else if (whichStatIndex == (int)WhichStat::WeightClass)
    {
        TryCommitStatChange(view, teamIndex, rowIndex, whichStatIndex, 0, 16);
    }
    else if (whichStatIndex == (int)WhichStat::PlayerNumber)
    {
        TryCommitStatChange(view, teamIndex, rowIndex, whichStatIndex, 1, 99);
    }
}