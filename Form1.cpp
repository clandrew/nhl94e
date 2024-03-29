﻿#include "pch.h"
#include "Form1.h"
#include "Form2.h"
#include "AddressMapping.h"
#include "TeamData.h"
#include "Utils.h"
#include "Optional.h"
#include "ProfileImageImporter.h"

static std::vector<TeamData> s_allTeams;

static std::vector<PlayerPallette> s_playerPallettes;

static std::vector<ProfilePalletteData> s_profilePalletteData;

std::vector<ProfileImageData> s_profileImageData;

std::vector<unsigned char> LoadRomBytesFromFile(std::wstring sourcePath)
{
    FILE* file = {};
    _wfopen_s(&file, sourcePath.c_str(), L"rb");
    long retrievedFileSize = GetFileSize(file);

    std::vector<unsigned char> result;
    result.resize(retrievedFileSize);
    fread(&result[0], 1, retrievedFileSize, file);
    fclose(file);

    return result;
}

void LoadRomBytesFromFile(std::wstring sourcePath, std::vector<unsigned char>* dest)
{
    FILE* file = {};
    _wfopen_s(&file, sourcePath.c_str(), L"rb");
    long retrievedFileSize = GetFileSize(file);

    fread(dest->data(), 1, retrievedFileSize, file);
    fclose(file);
}

class RomData
{
    std::vector<unsigned char> m_data;

public:
    unsigned char Get(int index) const
    {
        return m_data[index];
    }

    void LoadRomBytesFromFile(const wchar_t* fileName)
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

    void SetROMData(int index, unsigned char data)
    {
        assert(index < 0xA08000);
        m_data[index] = data;
    }

    void Set(int index, unsigned char data)
    {
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

    void ReadBytes(int fileOffset, int byteCount, unsigned char* dest)
    {
        memcpy(dest, &(m_data[fileOffset]), byteCount);
    }

    void SaveBytes(int fileOffset, int byteCount, unsigned char* bytes)
    {
        memcpy(&(m_data[fileOffset]), bytes, byteCount);
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
    AllStarsWest = 0x1B,
    Count
};

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

static struct SkinColorOverride
{
    std::string Name;
    int A8R8G8B8;
    unsigned short B5G5R5;
    int UITextColorARGB;
};
SkinColorOverride s_skinColorOverrides[]
{
    { "<not overridden>",   0x0,            0x0,        0xFF000000 },
    { "Light Beige",        0xFFE2BfB4,     0x5AFC,     0xFF000000},
    //{ "Beige", 0xC09080, 0x4258 }, used in game if not overridden
    { "Tan",                0xFF846054,     0x2990,     0xFFFFFFFF },
    { "Brown",              0xFF5B4437,     0x190B,     0xFFFFFFFF },
    { "Dark Brown",         0xFF2F231D,     0x0C85,     0xFFFFFFFF },
};

static std::vector<unsigned char> asm_LoadLongAddress_ArrayElement_Into_8D_txt;

struct ObjectCode
{
    std::vector<unsigned char> m_code;

    void PatchLoadLongAddressIn8D_Code(int expectedAddress)
    {
        // Operates on this kind of sequence
        /*
        A9 A0 00    LDA #$00A0  ; Upper short
        85 8F       STA $8F
        A9 00 82    LDA #$8200  ; Lower short
        85 8D       STA $8D
        */

        int sequenceFound = 0;
        for (int i = 0; i < m_code.size() - 9; ++i)
        {
            if (m_code[i + 0] == 0xA9 &&
                m_code[i + 3] == 0x85 && m_code[i + 4] == 0x8F &&
                m_code[i + 5] == 0xA9 &&
                m_code[i + 8] == 0x85 &&
                m_code[i + 9] == 0x8D)
            {
                int address =
                    m_code[i + 2] << 24 |
                    m_code[i + 1] << 16 |
                    m_code[i + 7] << 8 |
                    m_code[i + 6] << 0;
                if (address != expectedAddress)
                {
                    int temp = expectedAddress;
                    m_code[i + 6] = temp & 0xFF;
                    temp >>= 8;
                    m_code[i + 7] = temp & 0xFF;
                    temp >>= 8;
                    m_code[i + 1] = temp & 0xFF;
                    temp >>= 8;
                    m_code[i + 2] = temp & 0xFF;

                    int newAddress =
                        m_code[i + 2] << 24 |
                        m_code[i + 1] << 16 |
                        m_code[i + 7] << 8 |
                        m_code[i + 6] << 0;
                    assert(newAddress == expectedAddress);
                }

                ++sequenceFound;
            }
        }

        assert(sequenceFound == 1);
    }

    void PatchLoadFromLongAddress_LookupPlayerNameDet(int expectedAddress)
    {
        // Operates on this kind of sequence
        /*
        AA                   TAX
        BF 00 D0 A8          LDA 0xA8D000,x
        85 89                STA $89
        E8                   INX
        E8                   INX
        BF 00 D0 A8          LDA 0xA8D000,x
        85 8B                STA $8B
        */

        int sequenceFound = 0;
        for (int i = 0; i < m_code.size() - 15; ++i)
        {
            if (m_code[i + 0] == 0xAA &&
                m_code[i + 1] == 0xBF &&
                m_code[i + 5] == 0x85 && m_code[i + 6] == 0x89 &&
                m_code[i + 7] == 0xE8 &&
                m_code[i + 8] == 0xE8 &&
                m_code[i + 9] == 0xBF &&
                m_code[i + 13] == 0x85 && m_code[i + 14] == 0x8B)
            {
                int address0 =
                    m_code[i + 4] << 16 |
                    m_code[i + 3] << 8 |
                    m_code[i + 2] << 0;

                if (address0 != expectedAddress)
                {
                    int temp = expectedAddress;
                    m_code[i + 2] = temp & 0xFF;
                    temp >>= 8;
                    m_code[i + 3] = temp & 0xFF;
                    temp >>= 8;
                    m_code[i + 4] = temp & 0xFF;
                    temp >>= 8;

                    int newAddress =
                        m_code[i + 4] << 16 |
                        m_code[i + 3] << 8 |
                        m_code[i + 2] << 0;
                    assert(newAddress == expectedAddress);
                }

                int address1 =
                    m_code[i + 12] << 16 |
                    m_code[i + 11] << 8 |
                    m_code[i + 10] << 0;

                if (address1 != expectedAddress)
                {
                    int temp = expectedAddress;
                    m_code[i + 10] = temp & 0xFF;
                    temp >>= 8;
                    m_code[i + 11] = temp & 0xFF;
                    temp >>= 8;
                    m_code[i + 12] = temp & 0xFF;
                    temp >>= 8;

                    int newAddress =
                        m_code[i + 12] << 16 |
                        m_code[i + 11] << 8 |
                        m_code[i + 10] << 0;
                    assert(newAddress == expectedAddress);
                }
            }
        }
    }

    void AppendReturnLong_6B()
    {
        m_code.push_back(0x6B);
    }

    void AppendLongAddress3Bytes(int addr)
    {
        unsigned char c;
        c = addr & 0xFF;
        m_code.push_back(c);
        addr >>= 8;

        c = addr & 0xFF;
        m_code.push_back(c);
        addr >>= 8;

        c = addr & 0xFF;
        m_code.push_back(c);
        addr >>= 8;

        assert(addr == 0); // Needs to be 24 bits

    }

    void AppendLoadAccImmediate_A9_16bit(int imm)
    {
        m_code.push_back(0xA9);
        AppendShortImmediate(imm);
    }

    void AppendStoreLong_8F(int addr)
    {
        m_code.push_back(0x8F);
        AppendLongAddress3Bytes(addr);
    }

    void AppendLoadDirect_A5(unsigned char c)
    {
        m_code.push_back(0xA5);
        m_code.push_back(c);
    }

    void AppendIncAcc_1A()
    {
        m_code.push_back(0x1A);
    }

    void AppendIncY_C8()
    {
        m_code.push_back(0xC8);
    }

    void AppendLoadYImmediate_A0(int imm)
    {
        m_code.push_back(0xA0);
        AppendShortImmediate(imm);
    }

    void AppendLoadYDirect_A4(unsigned char c)
    {
        m_code.push_back(0xA4);
        m_code.push_back(c);
    }

    void AppendLoadAccumulatorAbsoluteY_B9(int address)
    {
        m_code.push_back(0xB9);
        AppendShortImmediate(address);
    }

    void AppendLoadLong_AF(int addr)
    {
        m_code.push_back(0xAF);
        AppendLongAddress3Bytes(addr);
    }

    void AppendLoadDirectFromLongPointer_A7(unsigned char c)
    {
        m_code.push_back(0xA7);
        m_code.push_back(c);
    }

    void AppendLoadDirectFromLongPointer_YIndexed_B7(unsigned char c)
    {
        m_code.push_back(0xB7);
        m_code.push_back(c);
    }

    void AppendStoreDirectFromLongPointer_87(unsigned char c)
    {
        m_code.push_back(0x87);
        m_code.push_back(c);
    }

    void AppendPushAcc_48()
    {
        m_code.push_back(0x48);
    }

    void AppendPullAcc_68()
    {
        m_code.push_back(0x68);
    }

    void AppendStoreDirect_85(unsigned char c)
    {
        m_code.push_back(0x85);
        m_code.push_back(c);
    }

    void LoadAccDirect_A5(unsigned char c)
    {
        m_code.push_back(0xA5);
        m_code.push_back(c);
    }

    void AppendAddWithCarryDirect_65(unsigned char c)
    {
        m_code.push_back(0x65);
        m_code.push_back(c);
    }

    void AppendArithmaticShiftAccLeft_0A()
    {
        m_code.push_back(0x0A);
    }

    void AppendShortImmediate(int imm)
    {
        unsigned char c;
        c = imm & 0xFF;
        m_code.push_back(c);
        imm >>= 8;

        c = imm & 0xFF;
        m_code.push_back(c);
        imm >>= 8;

        assert(imm == 0); // Needs to be 16 bits
    }

    void AppendAndImmediate_29(int imm)
    {
        m_code.push_back(0x29);
        AppendShortImmediate(imm);
    }

    void AppendLongJump_5C(int addr)
    {
        m_code.push_back(0x5C);
        AppendLongAddress3Bytes(addr);
    }

    void AppendStoreZeroA5()
    {
        // 64 A5                STZ $A5
        m_code.push_back(0x64);
        m_code.push_back(0xA5);
    }

    void AppendJumpSubroutineLong_22(int addr)
    {
        m_code.push_back(0x22);
        AppendLongAddress3Bytes(addr);
    }

    void PrependCode(unsigned char code[], int codeSize)
    {
        for (int i = codeSize - 1; i >= 0; --i)
        {
            m_code.insert(m_code.begin(), code[i]);
        }
    }

    void LoadAsm_LoadLongAddress_ArrayElement_Into_8D_txt()
    {
        if (asm_LoadLongAddress_ArrayElement_Into_8D_txt.size() == 0)
        {
            asm_LoadLongAddress_ArrayElement_Into_8D_txt = LoadAsmFromDebuggerTextImpl(L"LoadLongAddress_ArrayElement_Into_8D.asm");
        }

        m_code = asm_LoadLongAddress_ArrayElement_Into_8D_txt;
    }

    static std::vector<unsigned char> LoadAsmFromDebuggerTextImpl(std::wstring fileName)
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

    void LoadAsmFromDebuggerText(std::wstring fileName)
    {
        m_code = LoadAsmFromDebuggerTextImpl(fileName);
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

    int GetFileOffset() const
    {
        return m_fileOffset;
    }

    int GetROMOffset() const
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

    void SaveLengthDelimitedROMString_EnsureSpaceInBank(std::string const& str)
    {
        int strLength = static_cast<int>(str.size());
        int strLengthPlusLengthWord = strLength + 2;

        EnsureSpaceInBank(strLengthPlusLengthWord);

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

    void SaveNullDelimitedROMString_EnsureSpaceInBank(std::string const& str)
    {
        int strLength = static_cast<int>(str.size());
        int strLengthPlusNull = strLength + 1;

        EnsureSpaceInBank(strLengthPlusNull);

        for (int i = 0; i < strLength; ++i)
        {
            s_romData.Set(m_fileOffset + i, str[i]);
        }
        m_fileOffset += strLength;

        s_romData.Set(m_fileOffset, 0);
        ++m_fileOffset;
    }

    void SaveBytes(unsigned char* bytes, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            s_romData.Set(m_fileOffset, bytes[i]);
            ++m_fileOffset;
        }
    }

    void SaveBytes_EnsureSpaceInBank(unsigned char* bytes, int count)
    {
        EnsureSpaceInBank(count);
        SaveBytes(bytes, count);
    }

    void SaveObjectCode(ObjectCode* code)
    {
        for (int i = 0; i < code->m_code.size(); ++i)
        {
            s_romData.Set(m_fileOffset, code->m_code[i]);
            ++m_fileOffset;
        }

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

    void SkipBytes(int count)
    {
        m_fileOffset += count;
    }

    void SaveDelimiter()
    {
        s_romData.Set(m_fileOffset, 0x02);
        ++m_fileOffset;
        s_romData.Set(m_fileOffset, 0x00);
        ++m_fileOffset;
    }

    void SaveLongAddress3Bytes(int addr) // big endian-to-little-endian
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

        assert(addr == 0); // 24-byte address expected
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

    int LoadLongAddress4Bytes()
    {
        unsigned char b0 = s_romData.Get(m_fileOffset);
        ++m_fileOffset;

        unsigned char b1 = s_romData.Get(m_fileOffset);
        ++m_fileOffset;

        unsigned char b2 = s_romData.Get(m_fileOffset);
        ++m_fileOffset;

        unsigned char b3 = s_romData.Get(m_fileOffset);
        assert(b3 == 0); // 24-byte address expected
        ++m_fileOffset;

        int addr = 0;
        addr |= b3;
        addr <<= 8;
        addr |= b2;
        addr <<= 8;
        addr |= b1;
        addr <<= 8;
        addr |= b0;

        return addr;
    }

    void SaveByte(int b)
    {
        assert(b >= 0 && b < 256);
        s_romData.Set(m_fileOffset, b);
        ++m_fileOffset;
    }

    unsigned char LoadByte()
    {
        unsigned char b = s_romData.Get(m_fileOffset);
        ++m_fileOffset;
        return b;
    }

    void EnsureSpaceInBank(int bytes)
    {
        // This checks bank boundaries not page boundaries.
        // I haven't hit any bugs related to crossing of page boundaries seems this class of bugs existed on 6502 and were fixed in 65816.
        // There can be a performance penalty crossing page boundaries. This patcher doesn't engage with anything super 
        // down-to-the-clock-cycle performance critical.
        assert(bytes <= 0xFFFF); // Sanity check

        int thisBank = FileOffsetToROMAddress(m_fileOffset) >> 16;
        int thatBank = FileOffsetToROMAddress(m_fileOffset + bytes) >> 16;
        if (thisBank != thatBank)
        {
            int nextAddr = thatBank << 16;
            m_fileOffset = ROMAddressToFileOffset(nextAddr);
        }
    }
};

TeamData GetTeamData(int teamIndex, int playerDataAddress)
{
    TeamData result{};

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

        p.ProfileImageIndex = -1;

        result.Players.push_back(p);
    }

    {
        int addr = iter.GetROMOffset();
        result.TeamCity.Initialize(addr, iter.LoadROMString());
    }
    {
        int addr = iter.GetROMOffset();
        result.Acronym.Initialize(addr, iter.LoadROMString());
    }
    {
        int addr = iter.GetROMOffset();
        result.TeamName.Initialize(addr, iter.LoadROMString());
    }
    {
        int addr = iter.GetROMOffset();
        result.Venue.Initialize(addr, iter.LoadROMString());
    }

    result.HeaderColorIndex = teamIndex;
    result.HomeColorIndex = teamIndex;
    result.AwayColorIndex = teamIndex;
    result.SkinColorOverrideIndex = 0;

    return result;
}

// This loads pallettes for the gameplay sprites that display during games.
// These aren't the pallettes for profile images.
std::vector<PlayerPallette> LoadPlayerPallettes()
{
    std::vector<PlayerPallette> result;

    for (int teamIndex = 0; teamIndex < 28; ++teamIndex)
    {
        PlayerPallette p;
        p.Away.SourceDataROMAddress = 0x96C9CE + (0x20 * teamIndex);
        p.Away.SourceDataFileOffset = ROMAddressToFileOffset(p.Away.SourceDataROMAddress);
        s_romData.ReadBytes(p.Away.SourceDataFileOffset, 0x20, p.Away.Bytes);

        p.Home.SourceDataROMAddress = 0x96CD4E + (0x20 * teamIndex);
        p.Home.SourceDataFileOffset = ROMAddressToFileOffset(p.Home.SourceDataROMAddress);
        s_romData.ReadBytes(p.Home.SourceDataFileOffset, 0x20, p.Home.Bytes);

        result.push_back(p);
    }

    return result;
}

std::vector<ProfileImageData> LoadProfileImageData()
{
    std::wstring imageFilenames[] = {
        L"anaheim.bin",
        L"boston.bin",
        L"buffalo.bin",
        L"calgary.bin",
        L"chicago.bin",
        L"dallas.bin",
        L"detroit.bin",
        L"edmonton.bin",
        L"florida.bin",
        L"hartford.bin",
        L"LA.bin",
        L"Montreal.bin",
        L"NJ.bin",
        L"NYIslanders.bin",
        L"NYRangers.bin",
        L"Ottawa.bin",
        L"philly.bin",
        L"pittsburgh.bin",
        L"quebec.bin",
        L"sanjose.bin",
        L"stlouis.bin",
        L"tampabay.bin",
        L"toronto.bin",
        L"vancouver.bin",
        L"washington.bin",
        L"winnepeg.bin",
        L"ase.bin",
        L"asw.bin",
    };

    std::wstring prefix = L"ImageData\\";

    std::vector<ProfileImageData> result;

    for (int i = 0; i < _countof(imageFilenames); ++i)
    {
        ProfileImageData p;
        p.Path = prefix;
        p.Path.append(imageFilenames[i]);

        // The byte count in the binary file is slightly less than 2400. 
        // Reason: there is some non-viable data at the end of each profile image. 
        // We don't bother storing the last profile image's non-viable data on disk.
        // For profile images other than the last one, the non-viable data is stored in the file as zeroes.
        // Anyway, we pad out the full allocation here.
        p.ImageBytes.resize(0x2400);
        std::fill(p.ImageBytes.begin(), p.ImageBytes.end(), 0);
        LoadRomBytesFromFile(p.Path, &p.ImageBytes);
        result.push_back(p);
    }

    return result;
}

std::vector<ProfilePalletteData> LoadProfileImagePallettes()
{

    int profileImagePalletteLocationsInFile[] =
    {
        0xd747c,    // Anaheim confirm
        0x2ffdf,    // Boston confirm
        0xd6a5c,    // Buffalo confirm
        0xd6a7c,    // Calgary confirm
        0xd6a9c,    // Chicago confirm
        0xd6abc,    // Dallas confirm
        0xd6adc,    // Detroit confirm        
        0xd6afc,    // Edmonton confirm
        0xd6b1c,    // Florida confirm
        0xd6b5c,    // Hartford confirm
        0xd6b7c,    // LA confirm
        0xd6b9c,    // Montreal confirm
        0xd6bbc,    // NJ
        0xd6bdc,    // NY Islanders
        0xd6bfc,    // NY Rangers
        0xd6c1c,    // Ottawa
        0xd6c3c,    // Philly
        0xd6c5c,    // Pittsburgh
        0xd6c7c,    // Quebec
        0xd6c9c,    // SJ
        0xd6cbc,    // St Louis
        0xd6cdc,    // Tampa
        0xd6cfc,    // Toronto
        0xd6d1c,    // Vancouver
        0xd6d3c,    // Washington
        0xd6d5c,    // Winnepeg
        0xd6b3c,    // ASW/ASE confirm
        0xd6b3c,    // ASW/ASE confirm
    };

    std::vector<ProfilePalletteData> result;

    for (int i = 0; i < _countof(profileImagePalletteLocationsInFile); ++i)
    {
        ProfilePalletteData p;

        p.PalletteFileOffset = profileImagePalletteLocationsInFile[i];
        p.PalletteROMAddress = FileOffsetToROMAddress(p.PalletteFileOffset);
        p.PalletteBytes.resize(0x20);
        s_romData.ReadBytes(p.PalletteFileOffset, 0x20, p.PalletteBytes.data());

        result.push_back(p);
    }

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
        result.push_back(GetTeamData(teamIndex, playerDataPointers[teamIndex]));
    }

    // Hardcode who has profile images for now.
    result[(int)Team::Montreal].Players[2].ProfileImageIndex = 0;
    result[(int)Team::Montreal].Players[6].ProfileImageIndex = 1;
    result[(int)Team::Montreal].Players[11].ProfileImageIndex = 2;
    result[(int)Team::Montreal].Players[17].ProfileImageIndex = 3;
    result[(int)Team::Montreal].Players[16].ProfileImageIndex = 4;
    result[(int)Team::Montreal].Players[0].ProfileImageIndex = 5;

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
    System::Windows::Forms::DataGridViewButtonColumn^ Column1_5;
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

#if _DEBUG
    if (team.TeamCity.Get() == "Montreal")
    {
        m_montrealDataGridView = dataGridView1;
    }
#endif
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

void nhl94e::Form1::OpenROM(std::wstring romFilename)
{
    s_romData.LoadRomBytesFromFile(romFilename.c_str());

    if (!s_romData.EnsureExpandedSize())
        return;

    s_allTeams = LoadPlayerNamesAndStats();

    s_playerPallettes = LoadPlayerPallettes();

    s_profileImageData = LoadProfileImageData();

    s_profilePalletteData = LoadProfileImagePallettes();

    LoadProfileImagePallettes();

    for (int teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& team = s_allTeams[teamIndex];
        AddTeamGridUI(team);
    }

    const char* colorSchemeNames[] =
    {
        "Anaheim_WhiteGreen",
        "Boston_BlackYellow",
        "Buffalo_YellowBlue",
        "Calgary_YellowOrange",
        "Chicago_BlackRed",
        "Dallas_BlackGreen",
        "Detroit_WhiteRed",
        "Edmonton_BlueOrange",
        "Florida_BlueOrange",
        "Hartford_GreenBlue",
        "LosAngeles_BlackGray",
        "Montreal_RedBlue",
        "NewJersey_BlackRed",
        "NyIslanders_BlueOrange",
        "NyRangers_RedBlue",
        "Ottawa_BlackRed",
        "Philly_BlackOrange",
        "Pittsburgh_YellowBrown",
        "Quebec_LightBlueWhite",
        "SanJose_BlackTurquoise",
        "StLouis_YellowBlue",
        "TampaBay_BlackBlue",
        "Toronto_WhiteBlue",
        "Vancouver_YellowOrange",
        "Washington_BlueRed",
        "Winnipeg_RedPurple",
        "ASE_OrangeWhite",
        "ASW_BlackOrange",
    };

    for (int i = 0; i < 28; ++i)
    {
        this->headerColorComboBox->Items->Add(gcnew System::String(colorSchemeNames[i]));
        this->homeColorComboBox->Items->Add(gcnew System::String(colorSchemeNames[i]));
        this->awayColorComboBox->Items->Add(gcnew System::String(colorSchemeNames[i]));
    }

    for (int i = 0; i < _countof(s_skinColorOverrides); ++i)
    {
        this->skinColorOverrideComboBox->Items->Add(gcnew System::String(s_skinColorOverrides[i].Name.c_str()));
    }

    tabControl1->SelectedIndex = (int)Team::Montreal;

    this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &nhl94e::Form1::OnSelectedIndexChanged);
    OnSelectedIndexChanged(nullptr, nullptr);

#if _DEBUG
    
    {   // Goalie
        int playerIndex = 0;
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerName]->Value = "Amanda Leveille";
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerNumber]->Value = "29";
    }
    {
        int playerIndex = 1;
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerName]->Value = "Allie Morse";
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerNumber]->Value = "31";
    }
    {
        int playerIndex = 2;
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerName]->Value = "Ashleigh Brykaliuk";
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerNumber]->Value = "13";
    }
    {
        int playerIndex = 6;
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerName]->Value = "Jonna Curtis";
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerNumber]->Value = "3";
    }
    {
        int playerIndex = 11;
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerName]->Value = "Allie Thunstrom";
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerNumber]->Value = "9";
    }
    {
        int playerIndex = 16;
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerName]->Value = "Mak Langei";
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerNumber]->Value = "2";
    }
    {
        int playerIndex = 17;
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerName]->Value = "Patti Marshall";
        m_montrealDataGridView->Rows[playerIndex]->Cells[(int)WhichStat::PlayerNumber]->Value = "27";
    }
    {
        locationTextBox->Text = "Minnesota";
        acronymTextBox->Text = "MN";
        teamNameTextBox->Text = "Whitecaps"; 
        teamVenueTextBox->Text = "TRIA Rink";
        headerColorComboBox->SelectedIndex = (int)Team::TampaBay;
        homeColorComboBox->SelectedIndex = (int)Team::TampaBay;
        awayColorComboBox->SelectedIndex = (int)Team::TampaBay;
    }
    {
        ProfileImageImporter importer;
        importer.Import(L"reference.png");
        assert(importer.ImportedSomethingValid());

        MultiFormatPallette* importedPallette = importer.GetImportedPallette();
        std::vector<unsigned char>* importedSnesImageData = importer.GetImportedSnesImageData();
        OverwriteProfileImage((int)Team::Montreal, importedPallette, importedSnesImageData);
    }

#endif
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

struct InitializeDetourResult
{
    int DetourDestROMAddress;
    int FileOffsetSrcStart;
};

InitializeDetourResult InitializeDetour(
    std::vector<unsigned char> const& detourCode,
    int detourSrcStartROMAddress,
    int detourSrcEndROMAddress,
    RomDataIterator* freeSpaceIter)
{
    InitializeDetourResult r{};

    // Quick parameter checking
    int detourLength = detourSrcEndROMAddress - detourSrcStartROMAddress + 1; // Addresses are inclusive
    assert(detourLength >= 4); // Need enough room to insert a long jump

    r.FileOffsetSrcStart = ROMAddressToFileOffset(detourSrcStartROMAddress);
    int fileOffsetSrcEnd = ROMAddressToFileOffset(detourSrcEndROMAddress); // Exclusive

    // Save the start of free space so we know where to jump
    r.DetourDestROMAddress = freeSpaceIter->GetROMOffset();

    // Put detour payload in free space
    for (size_t i = 0; i < detourCode.size(); ++i)
    {
        freeSpaceIter->SaveByte(detourCode[i]);
    }

    for (int i = r.FileOffsetSrcStart; i < fileOffsetSrcEnd; ++i) // Good hygiene
    {
        s_romData.SetROMData(i, 0xEA); // NOP
    }

    return r;
}

bool InsertCallLongDetour(
    std::vector<unsigned char> const& detourCode,
    int detourSrcStartROMAddress,
    int detourSrcEndROMAddress,
    RomDataIterator* freeSpaceIter)
{
    InitializeDetourResult r = InitializeDetour(detourCode, detourSrcStartROMAddress, detourSrcEndROMAddress, freeSpaceIter);

    RomDataIterator iter(r.FileOffsetSrcStart);
    iter.SaveByte(0x22);
    iter.SaveLongAddress3Bytes(r.DetourDestROMAddress); // Detour payload author is responsible for RTLing.

    return true;
}

bool InsertJumpOutDetour(
    std::vector<unsigned char> const& detourCode,
    int detourSrcStartROMAddress,
    int detourSrcEndROMAddress,
    RomDataIterator* freeSpaceIter)
{
    if (detourCode.size() == 0)
        return false;

    InitializeDetourResult r = InitializeDetour(detourCode, detourSrcStartROMAddress, detourSrcEndROMAddress, freeSpaceIter);

    RomDataIterator iter(r.FileOffsetSrcStart);
    iter.SaveByte(0x5C);// JMP
    // Detour payload author is responsible for JMPing back or they can RTL in certain cases.
    iter.SaveLongAddress3Bytes(r.DetourDestROMAddress); 

    return true;
}

bool InsertTeamLocationText(RomDataIterator* freeSpaceIter)
{
    struct TeamRename
    {
        Team WhichTeam;
        std::string NewName;
    };

    std::vector<TeamRename> renames;
    std::vector<int> lengthDelimitedStringAddresses;
    std::vector<int> nullDelimitedStringAddresses;
    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        int lengthDelimitedStringAddress = teamData.TeamCity.SourceROMAddress;

        if (teamData.TeamCity.IsChanged())
        {
            TeamRename r;
            r.WhichTeam = (Team)teamIndex;
            r.NewName = teamData.TeamCity.Get();
            renames.push_back(r);

            lengthDelimitedStringAddress = 0; // Going to be changed
        }

        lengthDelimitedStringAddresses.push_back(lengthDelimitedStringAddress);
    }

    if (renames.size() == 0)
        return true; // Nothing to do

    {
        int nullDelimitedTeamLocationStringAddresses[] = {
            /* Anaheim */ 0x9C9681,
            /* Boston */ 0x9C9689,
            /* Buffalo */ 0x9C9690,
            /* Calgary */ 0x9C9698,
            /* Chicago */ 0x9C96A0,
            /* Dallas */ 0x9C96A8,
            /* Detroit */ 0x9C96AF,
            /* Edmonton */ 0x9C96B7,
            /* Florida */ 0x9C96C0,
            /* Hartford */ 0x9C96C8,
            /* LAKings */ 0x9C96D1,
            /* Montreal */ 0x9C96DD,
            /* NewJersey */ 0x9C96E6,
            /* NYIslanders */ 0x9C96F1,
            /* NYRangers */ 0x9C96FA,
            /* Ottawa */ 0x9C9703,
            /* Philly */ 0x9C970A,
            /* Pittsburgh */ 0x9C9717,
            /* Quebec */ 0x9C9722,
            /* SanJose */ 0x9C9729,
            /* StLouis */ 0x9C9732,
            /* TampaBay */ 0x9C973C,
            /* Toronto */ 0x9C9746,
            /* Vancouver */ 0x9C974E,
            /* Washington */ 0x9C9758,
            /* Winnepeg */ 0x9C9763,
            /* AllStarsEast */ 0x9C976C,
            /* AllStarsWest */ 0x9C9779
        };

        for (int i = 0; i < _countof(nullDelimitedTeamLocationStringAddresses); ++i)
        {
            nullDelimitedStringAddresses.push_back(nullDelimitedTeamLocationStringAddresses[i]);
        }

    }

    int lengthDelimitedStringTableStartFileAddress = 0;
    int nullDelimitedStringTableStartFileAddress = 0;
    int stringAddressTableSize = (int)Team::Count * 4;

    {
        // Write the length-delimited renamed strings and update the table
        for (int i = 0; i < renames.size(); ++i)
        {
            lengthDelimitedStringAddresses[(int)renames[i].WhichTeam] = freeSpaceIter->GetROMOffset();
            freeSpaceIter->SaveLengthDelimitedROMString_EnsureSpaceInBank(renames[i].NewName);
        }

        lengthDelimitedStringTableStartFileAddress = freeSpaceIter->GetFileOffset();

        // Write the length-delimited string table
        unsigned char* stringAddressData = reinterpret_cast<unsigned char*>(lengthDelimitedStringAddresses.data());
        freeSpaceIter->SaveBytes_EnsureSpaceInBank(stringAddressData, stringAddressTableSize);
    }
    {
        // Write the null-delimited renamed strings and update the table
        for (int i = 0; i < renames.size(); ++i)
        {
            nullDelimitedStringAddresses[(int)renames[i].WhichTeam] = freeSpaceIter->GetROMOffset();
            freeSpaceIter->SaveNullDelimitedROMString_EnsureSpaceInBank(renames[i].NewName);
        }

        nullDelimitedStringTableStartFileAddress = freeSpaceIter->GetFileOffset();


        // Write the length-delimited string table
        unsigned char* stringAddressData = reinterpret_cast<unsigned char*>(nullDelimitedStringAddresses.data());
        freeSpaceIter->SaveBytes_EnsureSpaceInBank(stringAddressData, stringAddressTableSize);
    }

    // Code patching
    // There are 2 pieces of code patching :( two things try to load team location strings
    // A) the game menu
    // B) the 'Ron Barr commentary'
    // They use completely different loader code. Terrific

    // This part is for the game menu
    // Insert code overtop 9B/C5AB -> 9B/C5E6 with noops in the extra space
    {
        ObjectCode code;
        code.LoadAsm_LoadLongAddress_ArrayElement_Into_8D_txt();
        code.PatchLoadLongAddressIn8D_Code(FileOffsetToROMAddress(lengthDelimitedStringTableStartFileAddress));
        code.AppendReturnLong_6B();

        int dstStartROMAddress = 0x9BC5AB;
        int dstEndROMAddress = 0x9BC5E6;
        int dstFileOffsetStart = ROMAddressToFileOffset(dstStartROMAddress);
        int dstFileOffsetEnd = ROMAddressToFileOffset(dstEndROMAddress);

        int patchIndex = 0;

        for (int fileOffset = dstFileOffsetStart; fileOffset <= dstFileOffsetEnd; ++fileOffset)
        {
            if (patchIndex < code.m_code.size())
            {
                s_romData.SetROMData(fileOffset, code.m_code[patchIndex]);
                patchIndex++;
            }
            else
            {
                s_romData.SetROMData(fileOffset, 0xEA); // NOP
            }
        }
    }

    // This part is for the 'Ron Barr commentary'.
    //
    // The detoured payload is too big to fit on top, so we need a jump out.
    // This is operating on $9E/CCFE - $9E/CD17
    {

        int dstStartROMAddress = 0x9ECCFE;
        int dstEndROMAddress = 0x9ECD17 + 2;
        int dstFileOffsetStart = ROMAddressToFileOffset(dstStartROMAddress);
        int dstFileOffsetEnd = ROMAddressToFileOffset(dstEndROMAddress);

        for (int fileOffset = dstFileOffsetStart; fileOffset < dstFileOffsetEnd; ++fileOffset)
        {
            s_romData.SetROMData(fileOffset, 0xEA); // NOP for safety
        }
        {
            // At AwayTeamWildCardImpl we change
            // $9E/CCFE AD DE 1C    LDA $1CDE  [$9F:1CDE]   A:0068 X:000A Y:0400 P:envmxdiZC	; E.g., load BDB2, the low short of the team's player data.
			//                                                                                  ; Observation: 9F1CE0-2 contain the home and away team indices, respectively.
            // $9E/CD01 85 8D       STA $8D    [$00:008D]   A:0068 X:000A Y:0400 P:envmxdiZC
            // $9E/CD03 80 05       BRA $05    [$CD0A]      A:0068 X:000A Y:0400 P:envmxdiZC	; goto DoneHomeOrAwaySelection
            
            // into

            // $9E/CCFE AF E2 1C 9F LDA $9F1CE2     ; This is the away team index
            // $9E/CD02 80 06       BRA $06    [$CD0A]
            int i = ROMAddressToFileOffset(0x9ECCFE);
            s_romData.SetROMData(i + 0, 0xAF);
            s_romData.SetROMData(i + 1, 0xE2);
            s_romData.SetROMData(i + 2, 0x1C);
            s_romData.SetROMData(i + 3, 0x9F);
            s_romData.SetROMData(i + 4, 0x80);
            s_romData.SetROMData(i + 5, 0x06);
        }
        {
            // At HomeTeamImpl we need to change

            // $9E/CD05 AD DC 1C    LDA $1CDC  [$9F:1CDC]   A:0000 X:000A Y:0004 P:envmxdiZc	; weird journey how we actually get here through a jump
            // 							                                                        ; E.g., load C2DB, the low short of the home team's player data.
            // 							                                                        ; 9ECCFE - 9ECD0D disasm. Need a branch 80 06
            // $9E/CD08 85 8D       STA $8D    [$00:008D]   A:0000 X:000A Y:0004 P:envmxdiZc

            // into

            // $9E/CD05 AF E0 1C 9F LDA $9F1CE0 ; this is the home team index
            int i = ROMAddressToFileOffset(0x9ECD05);
            s_romData.SetROMData(i + 0, 0xAF);
            s_romData.SetROMData(i + 1, 0xE0);
            s_romData.SetROMData(i + 2, 0x1C);
            s_romData.SetROMData(i + 3, 0x9F);
        }
        {
            ObjectCode code;
            code.LoadAsm_LoadLongAddress_ArrayElement_Into_8D_txt();
            code.PatchLoadLongAddressIn8D_Code(FileOffsetToROMAddress(lengthDelimitedStringTableStartFileAddress));
            code.AppendLongJump_5C(0x9ECD19);

            freeSpaceIter->EnsureSpaceInBank(code.m_code.size());
            InsertJumpOutDetour(code.m_code, 0x9ECD0A, 0x9ECD17 + 2, freeSpaceIter);
        }
    }

    // This part is for the game menu strings with the colored background. 
    // Those are actually two copies of strings like "Montreal" and "Anaheim". Different string, different format too.
    // Strings with colored background- all null delimited, preceeded by a short pointer table, 1 entry per team
    // Strings beside "home" and "visitor"- stored with length, not null delimited, together with the team data beside players' names etc

    // The awkward thing here is we can't really bake in a new string table right at the string load site because multiple 
    // string-loading entrypoints funnel in to a common path that loads from a short address.
    // And yes, one of those entrypoints uses a pointer table, but it's a pointer table of shorts.
    // The approach here is to
    //  1) Change the common path to load from a long instead of a short
    //  2) Change the entrypoints to write the bank of the long. In most cases, the bank is DBR

    // Entrypoint1
    // For developer credits, UI flavor text
    {
        ObjectCode code_LoadGameMenuString;
        code_LoadGameMenuString.AppendStoreDirect_85(0x16);
        code_LoadGameMenuString.AppendLoadDirect_A5(0x8D);
        code_LoadGameMenuString.AppendStoreDirect_85(0xA9);

        // Precondition of Entrypoint1 is that the requested data bank is in $8F.
        code_LoadGameMenuString.AppendLoadDirect_A5(0x8F);
        code_LoadGameMenuString.AppendStoreDirect_85(0xAB);

        code_LoadGameMenuString.AppendLongJump_5C(0x9C9429);

        freeSpaceIter->EnsureSpaceInBank(code_LoadGameMenuString.m_code.size());
        InsertJumpOutDetour(code_LoadGameMenuString.m_code, 0x9C9423, 0x9C9427 + 2, freeSpaceIter);
    }
    // Entrypoint2_BranchA
    // For loading strings like "Montreal" etc
    {
        int tableROMAddress = FileOffsetToROMAddress(nullDelimitedStringTableStartFileAddress);
        int tableLow = tableROMAddress & 0xFFFF;
        tableROMAddress >>= 16;
        int tableHigh = tableROMAddress & 0xFFFF;

        ObjectCode code_LoadGameMenuString;
        code_LoadGameMenuString.AppendArithmaticShiftAccLeft_0A();
        code_LoadGameMenuString.AppendArithmaticShiftAccLeft_0A(); // Multiply team index by 4 to turn into an offset
        code_LoadGameMenuString.m_code.push_back(0xA8); // TAY. Y == offset

        code_LoadGameMenuString.AppendLoadAccImmediate_A9_16bit(tableHigh);
        code_LoadGameMenuString.AppendStoreDirect_85(0xAB);
        code_LoadGameMenuString.AppendLoadAccImmediate_A9_16bit(tableLow);
        code_LoadGameMenuString.AppendStoreDirect_85(0xA9);

        // Load pointer table address. Subsequent loader code is set up to understand a long
        code_LoadGameMenuString.AppendLoadDirectFromLongPointer_YIndexed_B7(0xA9);
        code_LoadGameMenuString.AppendPushAcc_48(); 
        code_LoadGameMenuString.AppendIncY_C8();
        code_LoadGameMenuString.AppendIncY_C8();
        code_LoadGameMenuString.AppendLoadDirectFromLongPointer_YIndexed_B7(0xA9);
        code_LoadGameMenuString.AppendStoreDirect_85(0xAB);
        code_LoadGameMenuString.AppendPullAcc_68();
        code_LoadGameMenuString.AppendStoreDirect_85(0xA9);

        code_LoadGameMenuString.AppendLoadYImmediate_A0(0x0000);

        code_LoadGameMenuString.AppendLongJump_5C(0x9C945A);

        freeSpaceIter->EnsureSpaceInBank(code_LoadGameMenuString.m_code.size());
        InsertJumpOutDetour(code_LoadGameMenuString.m_code, 0x9C9450, 0x9C9457 + 3, freeSpaceIter);
    }
    // Entrypoint2_BranchB
    // For loading buffered player name strings like "Kirk Muller"
    {
        ObjectCode code_LoadGameMenuString;
        code_LoadGameMenuString.AppendLoadDirect_A5(0x8D);
        code_LoadGameMenuString.AppendIncAcc_1A();
        code_LoadGameMenuString.AppendIncAcc_1A();
        code_LoadGameMenuString.AppendStoreDirect_85(0xA9); // store result

        // Push DBR, pull acc, mask
        code_LoadGameMenuString.m_code.push_back(0x8B);
        code_LoadGameMenuString.m_code.push_back(0x8B);
        code_LoadGameMenuString.AppendPullAcc_68();
        code_LoadGameMenuString.AppendAndImmediate_29(0xFF);
        code_LoadGameMenuString.AppendStoreDirect_85(0xAB);

        code_LoadGameMenuString.AppendLongJump_5C(0x9C9471);

        freeSpaceIter->EnsureSpaceInBank(code_LoadGameMenuString.m_code.size());
        InsertJumpOutDetour(code_LoadGameMenuString.m_code, 0x9C946B, 0x9C946F + 2, freeSpaceIter);
    }
    {
        // 1st pass over the string for layout. That variable width font though
        ObjectCode code_LoadGameMenuString_CommonPath_FirstLoad;
        code_LoadGameMenuString_CommonPath_FirstLoad.AppendLoadDirectFromLongPointer_YIndexed_B7(0xA9); // This is the value we need to return
        code_LoadGameMenuString_CommonPath_FirstLoad.AppendAndImmediate_29(0xFF); // Remember mask
        code_LoadGameMenuString_CommonPath_FirstLoad.AppendLongJump_5C(0x9C947E);

        freeSpaceIter->EnsureSpaceInBank(code_LoadGameMenuString_CommonPath_FirstLoad.m_code.size());
        InsertJumpOutDetour(code_LoadGameMenuString_CommonPath_FirstLoad.m_code, 0x9C9479, 0x9C947B + 3, freeSpaceIter);
    }
    {
        // 2nd pass over the string to actually copy it
        ObjectCode code_LoadGameMenuString_CommonPath_SecondLoad;
        code_LoadGameMenuString_CommonPath_SecondLoad.AppendLoadDirectFromLongPointer_YIndexed_B7(0xA9); // This is the value we need to return
        code_LoadGameMenuString_CommonPath_SecondLoad.AppendAndImmediate_29(0xFF); // Remember mask
        code_LoadGameMenuString_CommonPath_SecondLoad.AppendLongJump_5C(0x9C94FD);

        freeSpaceIter->EnsureSpaceInBank(code_LoadGameMenuString_CommonPath_SecondLoad.m_code.size());
        InsertJumpOutDetour(code_LoadGameMenuString_CommonPath_SecondLoad.m_code, 0x9C94F8, 0x9C94FA + 3, freeSpaceIter);
    }
    
    // This is for the TEAM SETUP screen, which has different string loader code from the GAME SETUP screen
    {

        int tableROMAddress = FileOffsetToROMAddress(nullDelimitedStringTableStartFileAddress);
        int tableLow = tableROMAddress & 0xFFFF;
        tableROMAddress >>= 16;
        int tableHigh = tableROMAddress & 0xFFFF;

        ObjectCode code;

        // Objective: load e.g., A0807B into $0C-$0E.
        // Precondition: team index * 2 is stored in X

        code.m_code.push_back(0x5A); // PHY
        code.m_code.push_back(0x8A); // TXA
        code.AppendArithmaticShiftAccLeft_0A();
        code.m_code.push_back(0xA8); // TAY

        code.AppendLoadAccImmediate_A9_16bit(tableHigh);
        code.AppendStoreDirect_85(0x0E);
        code.AppendLoadAccImmediate_A9_16bit(tableLow);
        code.AppendStoreDirect_85(0x0C);

        code.AppendLoadDirectFromLongPointer_YIndexed_B7(0x0C);
        code.AppendPushAcc_48();
        code.AppendIncY_C8();
        code.AppendIncY_C8();
        code.AppendLoadDirectFromLongPointer_YIndexed_B7(0x0C);
        code.AppendStoreDirect_85(0x0E);
        code.AppendPullAcc_68();
        code.AppendStoreDirect_85(0x0C);

        code.m_code.push_back(0x7A); // PLY

        code.AppendLongJump_5C(0x9DAF38);

        freeSpaceIter->EnsureSpaceInBank(code.m_code.size());
        InsertJumpOutDetour(code.m_code, 0x9DAF2D, 0x9DAF36 + 2, freeSpaceIter);
    }

    return true;
}

bool InsertTeamAcronymText(RomDataIterator* freeSpaceIter)
{
    struct TeamRename
    {
        Team WhichTeam;
        std::string NewAcronym;
    };

    std::vector<TeamRename> renames;
    std::vector<int> stringAddresses;
    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        int stringAddress = teamData.Acronym.SourceROMAddress;

        if (teamData.Acronym.IsChanged())
        {
            TeamRename r;
            r.WhichTeam = (Team)teamIndex;
            r.NewAcronym = teamData.Acronym.Get();
            renames.push_back(r);

            stringAddress = 0; // Going to be changed
        }

        stringAddresses.push_back(stringAddress);
    }

    if (renames.size() == 0)
        return true; // Nothing to do

    // Reserve string table. Can't write the whole thing because we don't know the addresses of renamed strings yet.
    int stringTableStartFileAddress = freeSpaceIter->GetFileOffset();
    int stringAddressTableSize = (int)Team::Count * 4;
    freeSpaceIter->EnsureSpaceInBank(stringAddressTableSize);
    freeSpaceIter->SkipBytes(stringAddressTableSize);

    // Write the renamed strings
    for (int i = 0; i < renames.size(); ++i)
    {
        stringAddresses[(int)renames[i].WhichTeam] = freeSpaceIter->GetROMOffset();
        freeSpaceIter->SaveLengthDelimitedROMString_EnsureSpaceInBank(renames[i].NewAcronym);
    }

    // Write the string table
    {
        int dstFileOffset = stringTableStartFileAddress;

        unsigned char* stringAddressData = reinterpret_cast<unsigned char*>(stringAddresses.data());
        for (int i = 0; i < stringAddressTableSize; ++i)
        {
            s_romData.Set(dstFileOffset + i, stringAddressData[i]);
        }
    }

    // Code patching
    // Insert code overtop 9B/C5AB -> 9B/C5E6 with noops in the extra space
    {
        ObjectCode code;
        code.LoadAsm_LoadLongAddress_ArrayElement_Into_8D_txt();

        // AD 0F 0D    LDA $0D0F; Original code does this, we preserve it.
        // 48          PHA
        //
        // Load the team index, which has been stored at 9F1C98/9F1C9A for home/away.
        // A4 91                LDY $91
        // B9 98 1C             LDA $1C98, y[$9F:1C98]
        unsigned char prefix[] = { 0xAD, 0x0F, 0x0D, 0x48, 0xA4, 0x91, 0xB9, 0x98, 0x1C };
        code.PrependCode(prefix, _countof(prefix));

        code.PatchLoadLongAddressIn8D_Code(FileOffsetToROMAddress(stringTableStartFileAddress));

        code.AppendJumpSubroutineLong_22(0x9EC7BE);

        int dstStartROMAddress = 0x9FBD66;
        int dstEndROMAddress = 0x9FBD8D;
        int dstFileOffsetStart = ROMAddressToFileOffset(dstStartROMAddress);
        int dstFileOffsetEnd = ROMAddressToFileOffset(dstEndROMAddress);

        int patchIndex = 0;

        for (int fileOffset = dstFileOffsetStart; fileOffset <= dstFileOffsetEnd; ++fileOffset)
        {
            if (patchIndex < code.m_code.size())
            {
                s_romData.SetROMData(fileOffset, code.m_code[patchIndex]);
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


bool InsertTeamNameOrVenueText(RomDataIterator* freeSpaceIter)
{
    struct TeamRename
    {
        Team WhichTeam;
        std::string NewName;
        std::string NewVenue;
    };

    std::vector<TeamRename> renames;
    std::vector<int> teamNameStringAddresses;
    std::vector<int> venueStringAddresses;
    for (size_t teamIndex = 0; teamIndex < s_allTeams.size(); ++teamIndex)
    {
        TeamData const& teamData = s_allTeams[teamIndex];
        int teamNameStringAddress = teamData.TeamName.SourceROMAddress;
        int venueStringAddress = teamData.Venue.SourceROMAddress;

        if (teamData.TeamName.IsChanged() || teamData.Venue.IsChanged())
        {
            TeamRename r;
            r.WhichTeam = (Team)teamIndex;
            r.NewName = teamData.TeamName.Get();
            r.NewVenue = teamData.Venue.Get();
            renames.push_back(r);

            teamNameStringAddress = 0; // Going to be changed
        }

        teamNameStringAddresses.push_back(teamNameStringAddress);
        venueStringAddresses.push_back(venueStringAddress);
    }

    if (renames.size() == 0)
        return true; // Nothing to do

    // Write the renamed strings
    for (int i = 0; i < renames.size(); ++i)
    {
        teamNameStringAddresses[(int)renames[i].WhichTeam] = freeSpaceIter->GetROMOffset();
        freeSpaceIter->SaveLengthDelimitedROMString_EnsureSpaceInBank(renames[i].NewName);

        venueStringAddresses[(int)renames[i].WhichTeam] = freeSpaceIter->GetROMOffset();
        freeSpaceIter->SaveLengthDelimitedROMString_EnsureSpaceInBank(renames[i].NewVenue);
    }
    // Write the string tables
    int teamNameStringTableStartFileAddress = 0;
    {
        teamNameStringTableStartFileAddress = freeSpaceIter->GetFileOffset();
        int teamNameStringAddressTableSize = (int)Team::Count * 4;
        unsigned char* stringAddressData = reinterpret_cast<unsigned char*>(teamNameStringAddresses.data());
        freeSpaceIter->SaveBytes_EnsureSpaceInBank(stringAddressData, teamNameStringAddressTableSize);
    }

    int venueStringTableStartFileAddress = 0;
    {
        venueStringTableStartFileAddress = freeSpaceIter->GetFileOffset();
        int venueStringAddressTableSize = (int)Team::Count * 4;
        unsigned char* stringAddressData = reinterpret_cast<unsigned char*>(venueStringAddresses.data());
        freeSpaceIter->SaveBytes_EnsureSpaceInBank(stringAddressData, venueStringAddressTableSize);
    }

    // Code for replacement of team name and venue in player card
    {
        ObjectCode code_teamNameInPlayerCard;
        code_teamNameInPlayerCard.LoadAsm_LoadLongAddress_ArrayElement_Into_8D_txt();

        // Load the team index, which has been stored at 9F1C98/9F1C9A for home/away.
        // A4 91                LDY $91
        // B9 98 1C             LDA $1C98, y[$9F:1C98]
        unsigned char prefix[] = { 0xA4, 0x91, 0xB9, 0x98, 0x1C };
        code_teamNameInPlayerCard.PrependCode(prefix, _countof(prefix));
        code_teamNameInPlayerCard.AppendLongJump_5C(0x9DC149);
        code_teamNameInPlayerCard.PatchLoadLongAddressIn8D_Code(FileOffsetToROMAddress(teamNameStringTableStartFileAddress));

        bool detourPatched = InsertJumpOutDetour(code_teamNameInPlayerCard.m_code, 0x9DC12B, 0x9DC147 + 2, freeSpaceIter);
        if (!detourPatched) return false;
    }

    // Code for replacement of team name as used by Ron Barr
    {
        ObjectCode code_teamNameForRonBarr;
        code_teamNameForRonBarr.LoadAsm_LoadLongAddress_ArrayElement_Into_8D_txt();

        int dstStartROMAddress = 0x9ECC1E;
        int dstEndROMAddress = 0x9ECC48 + 3;
        int dstFileOffsetStart = ROMAddressToFileOffset(dstStartROMAddress);
        int dstFileOffsetEnd = ROMAddressToFileOffset(dstEndROMAddress);

        for (int fileOffset = dstFileOffsetStart; fileOffset < dstFileOffsetEnd; ++fileOffset)
        {
            s_romData.SetROMData(fileOffset, 0xEA); // NOP for safety
        }

        {
            // For the away team, we need to change

            // $9E/CC1E AD DE 1C    LDA $1CDE  [$9F:1CDE]   A:0002 X:000A Y:0004 P:envmxdizc
            // $9E/CC21 85 8D       STA $8D    [$00:008D]   A:0002 X:000A Y:0004 P:envmxdizc
            // $9E/CC23 80 05       BRA $05    [$CC2A]      A:009C X:000A Y:0004 P:envmxdizC	

            // into

            // $9E/CC1E AF E2 1C 9F LDA $9F1CE2     ; This is the away team index
            // $9E/CC22 80 06       BRA $06    [$CC2A]      A:009C X:000A Y:0004 P:envmxdizC	
            int i = ROMAddressToFileOffset(0x9ECC1E);
            s_romData.SetROMData(i + 0, 0xAF);
            s_romData.SetROMData(i + 1, 0xE2);
            s_romData.SetROMData(i + 2, 0x1C);
            s_romData.SetROMData(i + 3, 0x9F);
            s_romData.SetROMData(i + 4, 0x80);
            s_romData.SetROMData(i + 5, 0x06);
        }
        {
            // At HomeTeamImpl we need to change
            
            // $9E/CC25 AD DC 1C    LDA $1CDC  [$9F:1CDC]   A:009C X:000A Y:0004 P:envmxdizC	; Load main table entry for home team
            // $9E/CC28 85 8D       STA $8D    [$00:008D]   A:C2DB X:000A Y:0400 P:eNvmxdizC

            // into

            // $9E/CC25 AF E0 1C 9F LDA $9F1CE0 ; this is the home team index
            int i = ROMAddressToFileOffset(0x9ECC25);
            s_romData.SetROMData(i + 0, 0xAF);
            s_romData.SetROMData(i + 1, 0xE0);
            s_romData.SetROMData(i + 2, 0x1C);
            s_romData.SetROMData(i + 3, 0x9F);
        }
        // By here, the team index is loaded into A.
        code_teamNameForRonBarr.AppendStoreZeroA5();
        code_teamNameForRonBarr.AppendLongJump_5C(0x9ECD19);
        code_teamNameForRonBarr.PatchLoadLongAddressIn8D_Code(FileOffsetToROMAddress(teamNameStringTableStartFileAddress));

        bool detourPatched = InsertJumpOutDetour(code_teamNameForRonBarr.m_code, 0x9ECC2A, 0x9ECC48 + 2, freeSpaceIter);
        if (!detourPatched) return false;
    }

    // Code for replacement of team venue as used by Ron Barr
    {
        ObjectCode code_teamVenueForRonBarr;
        code_teamVenueForRonBarr.LoadAsm_LoadLongAddress_ArrayElement_Into_8D_txt();

        // AF E0 1C 9F LDA $9F1CE0
        unsigned char prefix[] = { 0xAF, 0xE0, 0x1C, 0x9F };
        code_teamVenueForRonBarr.PrependCode(prefix, _countof(prefix));
        code_teamVenueForRonBarr.AppendLongJump_5C(0x9ECD19);

        code_teamVenueForRonBarr.PatchLoadLongAddressIn8D_Code(FileOffsetToROMAddress(venueStringTableStartFileAddress));

        bool detourPatched = InsertJumpOutDetour(code_teamVenueForRonBarr.m_code, 0x9ECC4B, 0x9ECC6C + 2, freeSpaceIter);
        if (!detourPatched) return false;
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
bool AddLookupPlayerNamePointerTables(std::vector<PlayerRename> const& renames, RomDataIterator* freeSpaceIter)
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
    // At the end of the tables is a data stream of variable length.
    //
    // In general this approach is memory-optimized for the situation where you plan to modify some players but not all of them.
    // In the datastream you store just the players whose names need to be patched on top.

    // First, plan out where each of the tables will be so that things are nice and compact.

    ObjectCode code_loadNamesDuringGameSetup;
    code_loadNamesDuringGameSetup.LoadAsmFromDebuggerText(L"LookupPlayerNameDet.asm");
    if (code_loadNamesDuringGameSetup.m_code.size() == 0)
        return false;

    int codeSize = code_loadNamesDuringGameSetup.m_code.size();
    freeSpaceIter->EnsureSpaceInBank(codeSize);
    const int codeROMLocation = freeSpaceIter->GetROMOffset();
    freeSpaceIter->SkipBytes(codeSize);

    int firstPointerTableSize = static_cast<int>(allTeams.size()) * 4;
    freeSpaceIter->EnsureSpaceInBank(firstPointerTableSize);
    const int firstTableLocation = freeSpaceIter->GetROMOffset();
    freeSpaceIter->SkipBytes(firstPointerTableSize);

    const int totalPlayerCount = 653;
    const int secondTableTotalSizes = totalPlayerCount * 4;
    freeSpaceIter->EnsureSpaceInBank(secondTableTotalSizes);
    const int secondTableLocation = freeSpaceIter->GetROMOffset();
    freeSpaceIter->SkipBytes(secondTableTotalSizes);


    // Write the datastream
    {
        for (int i = 0; i < renames.size(); ++i)
        {
            // Look up the person being renamed
            TeamData* team = &allTeams[(int)renames[i].WhichTeam];
            PlayerData* playerData = &team->Players[renames[i].PlayerIndex];

            // The string itself, its length prefix, and decimal player-number
            freeSpaceIter->EnsureSpaceInBank(renames[i].Name.Get().size() + 2 + 1);

            playerData->ReplacedROMAddressForRename = freeSpaceIter->GetROMOffset();
            freeSpaceIter->SaveLengthDelimitedROMString_EnsureSpaceInBank(renames[i].Name.Get());
            freeSpaceIter->SaveDecimalNumber(renames[i].PlayerNumber.Get(), false);
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

    // Patch code that looks up into the tables in the GAME SETUP menu
    {
        RomDataIterator codeIter(ROMAddressToFileOffset(codeROMLocation));

        code_loadNamesDuringGameSetup.PatchLoadFromLongAddress_LookupPlayerNameDet(firstTableLocation);

        // Insert the detour code in free space, and add the jmp
        bool detourPatched = InsertJumpOutDetour(code_loadNamesDuringGameSetup.m_code, 0x9FC732, 0x9FC756 + 1, &codeIter);
        if (!detourPatched)
            return false;
    }

    // Patch code for the goalie selection menu if you are re-naming goalies
    {
        ObjectCode code_goalieSelection;

        /*
        Back up 8D, 8F 
        */
        code_goalieSelection.LoadAccDirect_A5(0x8D);
        code_goalieSelection.AppendPushAcc_48();
        code_goalieSelection.LoadAccDirect_A5(0x8F);
        code_goalieSelection.AppendPushAcc_48();
         
        /*
            LDY $91                         ; Load home-versus-away
            LDA 9F1C98/9F1C9A based on Y    ; Load team index
            0A ASL                          ; Multiply by 4 to turn into a byte index
            0A ASL
            A8 TAY                          ; Put byte index in Y
        */
        code_goalieSelection.AppendLoadYDirect_A4(0x91);
        code_goalieSelection.AppendLoadAccumulatorAbsoluteY_B9(0x1C98);
        code_goalieSelection.AppendArithmaticShiftAccLeft_0A();
        code_goalieSelection.AppendArithmaticShiftAccLeft_0A();
        code_goalieSelection.m_code.push_back(0xA8);

        /*        
            // Put 0xA08200 (or whatever the alternate main table is) into $99
            A9 A0 00    LDA #$00A0
            85 8F       STA $9B
            A9 00 82    LDA #$8200
            85 8D       STA $99
        */

        int tableROMAddress = firstTableLocation;
        int tableLow = tableROMAddress & 0xFFFF;
        tableROMAddress >>= 16;
        int tableHigh = tableROMAddress & 0xFFFF;

        code_goalieSelection.AppendLoadAccImmediate_A9_16bit(tableHigh);
        code_goalieSelection.AppendStoreDirect_85(0x9B);
        code_goalieSelection.AppendLoadAccImmediate_A9_16bit(tableLow);
        code_goalieSelection.AppendStoreDirect_85(0x99);

        /*
            // Load AlternateMainTable[teamIndex], a long pointer, into $8D-$8F

            B7 99       LDA [$99],y         ; Seek to the right team. load the low short
            85 8D       STA $8D             ; Save the low short
            C8          INY                 ; Y+=2
            C8          INY
            B7 99       LDA [$99],y
            85 8F       STA $8F
        */
        code_goalieSelection.AppendLoadDirectFromLongPointer_YIndexed_B7(0x99);
        code_goalieSelection.AppendStoreDirect_85(0x8D);
        code_goalieSelection.AppendIncY_C8();
        code_goalieSelection.AppendIncY_C8();
        code_goalieSelection.AppendLoadDirectFromLongPointer_YIndexed_B7(0x99);
        code_goalieSelection.AppendStoreDirect_85(0x8F);

        /*
            // Taken by itself, the long pointer we just stored points to the first
            // person in the team.
            // Instead, we want it to point to the Nth person on the team, where N = goalie index.
            // So, increment the long pointer by goalie index * 4.

                        LDA $C5             ; Load the goalie index
            0A          ASL                 ; Multiply by 4 to turn into a byte index
            0A          ASL
                        ADC $8D
                        STA $8D        
        */
        code_goalieSelection.LoadAccDirect_A5(0xC5);
        code_goalieSelection.AppendArithmaticShiftAccLeft_0A();
        code_goalieSelection.AppendArithmaticShiftAccLeft_0A();
        code_goalieSelection.AppendAddWithCarryDirect_65(0x8D);
        code_goalieSelection.AppendStoreDirect_85(0x8D);

        /*
            // Ok so now the long pointer points to the address of string for the right player.
            // It'll be 0xA0..... for replaced names, 0x9C.... for default names.
            // Dereference this to find the value we ultimately need to return.

            A7 8D       LDA [$8D]         ; Low short
            85 99       STA $99           ; Save the low short

                        LDA $8D
                        INC
                        INC
                        STA $8D

                        LDA [$8D]
                        STA $9B
        */
        code_goalieSelection.AppendLoadDirectFromLongPointer_A7(0x8D);
        code_goalieSelection.AppendStoreDirect_85(0x99);

        code_goalieSelection.LoadAccDirect_A5(0x8D);
        code_goalieSelection.AppendIncAcc_1A();
        code_goalieSelection.AppendIncAcc_1A();
        code_goalieSelection.AppendStoreDirect_85(0x8D);

        code_goalieSelection.AppendLoadDirectFromLongPointer_A7(0x8D);
        code_goalieSelection.AppendStoreDirect_85(0x9B);


        /*
        Restore 8D, 8F
        */
        code_goalieSelection.AppendPullAcc_68();
        code_goalieSelection.AppendStoreDirect_85(0x8F);
        code_goalieSelection.AppendPullAcc_68();
        code_goalieSelection.AppendStoreDirect_85(0x8D);

        // Jump back
        code_goalieSelection.AppendLongJump_5C(0x9FEB80 + 2);

        bool detourPatched = InsertJumpOutDetour(code_goalieSelection.m_code, 0x9FEB5E, 0x9FEB80 + 2, freeSpaceIter);
        if (!detourPatched) return false;
    }

    return true;
}

bool InsertPlayerNameText(RomDataIterator* freeSpaceIter)
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

    AddLookupPlayerNamePointerTables(renames, freeSpaceIter); // The above code depends on these tables.

    return true;
}

bool InsertLogo(RomDataIterator* freeSpaceIter)
{
    int montrealRoutineROMAddress = 0;
    {
        ObjectCode code;        

        code.m_code.push_back(0xC2); // REP 10
        code.m_code.push_back(0x10);

        // Write all 0s to 0x7FA675 thru 0x7FA675 + 0x380
        // LDX 0380
        code.m_code.push_back(0xA2);
        code.m_code.push_back(0x80);
        code.m_code.push_back(0x03);

        code.AppendLoadAccImmediate_A9_16bit(0);
        
        // STA long,X
        code.m_code.push_back(0x9F);
        code.AppendLongAddress3Bytes(0x7FA675);

        code.m_code.push_back(0xCA);// DEX

        code.m_code.push_back(0xD0);// BNE loop
        code.m_code.push_back(0xF9);

        code.m_code.push_back(0xE2); // SEP 10
        code.m_code.push_back(0x10);
        code.AppendReturnLong_6B();

        freeSpaceIter->EnsureSpaceInBank(code.m_code.size());
        montrealRoutineROMAddress = freeSpaceIter->GetROMOffset();
        freeSpaceIter->SaveObjectCode(&code);

        // Need to investigate why this change is not sufficient.

        // Logo still shows in Ron Barr screen. In Ron Barr screen, it's stored at CC00.
        // For Ron Barr, though, the logo is still stored at 0x7FA675.
        /*        
            Full list of decompress args for Ron Barr:
            64 B6 81
            75 B6 81
            58 CB 81
            46 FE 81

            16 C3 9A

            26 E2 94

            F4 C1 9A
            33 88 9A

            25 8B 99
            E9 E1 99
            0A CD 98
            CD D7 97
            B5 FA 99
            7B E6 97
            7A CB 97
            0B AB 97
            42 DA 97
            77 D8 98
            9C E1 97
        */
    }
    {
        ObjectCode code;

        // If 0C-0E contains 9AE99E, that's the montreal logo.
        code.AppendLoadDirect_A5(0x0D);
        code.m_code.push_back(0xC9);    // CMP
        code.m_code.push_back(0xE9);
        code.m_code.push_back(0x9A);

        code.m_code.push_back(0xD0); // BNE
        code.m_code.push_back(0x04); 
        code.AppendJumpSubroutineLong_22(montrealRoutineROMAddress);

        // Required payload. Keep this last.
        //              LDA 6C
        // 30 04        BMI 04
        // __ __ __ __  JMP 80C199
        // __ __ __ __  JMP 80C1AF
        code.AppendLoadDirect_A5(0x6C);
        code.m_code.push_back(0x30);
        code.m_code.push_back(0x04);
        code.AppendLongJump_5C(0x80C199);
        code.AppendLongJump_5C(0x80C1AF);

        freeSpaceIter->EnsureSpaceInBank(code.m_code.size());
        InsertJumpOutDetour(code.m_code, 0x80C195, 0x80C197 + 2, freeSpaceIter);
    }

    return true;
}

struct TeamSubheaderPaletteDescription
{
    int DataAddress;
    unsigned char* Data;
};

void SaveHeaderColors()
{
    unsigned char anaheimWhiteGreen[] = { 0x00, 0x00, 0x7B, 0x6F, 0xEF, 0x3D, 0x10, 0xC2, 0x52, 0x4A, 0x94, 0x52, 0xD6, 0xDA, 0x18, 0xE3, 0x60, 0x42, 0x20, 0x3A, 0xE0, 0x31, 0xA0, 0xA9, 0x60, 0xA1, 0x20, 0x19, 0x00, 0x11, 0x1A, 0x03 };
    unsigned char bostonBlackYellow[] = { 0x00, 0x00, 0x7B, 0x6F, 0x21, 0x04, 0x42, 0x08, 0x84, 0x10, 0xA5, 0x94, 0xE7, 0x9C, 0x29, 0xA5, 0x9F, 0x2F, 0x3C, 0xA7, 0xDA, 0x1E, 0x97, 0x96, 0x35, 0x8E, 0xF2, 0x05, 0x90, 0x81, 0x1A, 0x83 };
    unsigned char buffaloYellowBlue[] = { 0x00, 0x00, 0x7B, 0x6F, 0x8F, 0x01, 0xF2, 0x81, 0x75, 0x82, 0xD8, 0x02, 0x5B, 0x83, 0xDE, 0x03, 0x00, 0xFC, 0x00, 0x6C, 0x00, 0x5C, 0x00, 0x50, 0x00, 0x40, 0x00, 0x34, 0x00, 0x24, 0x1A, 0x03 };
    unsigned char calgaryYellowOrange[] = { 0x00, 0x00, 0x7B, 0x6F, 0xF2, 0x01, 0x54, 0x82, 0xB7, 0x02, 0x3A, 0x83, 0x9C, 0x03, 0xFF, 0x83, 0x5D, 0x81, 0x3A, 0x01, 0x18, 0x01, 0xF5, 0x00, 0xD2, 0x80, 0xAF, 0x80, 0x8D, 0x80, 0x1A, 0x83 };
    unsigned char chicagoBlackRed[] = { 0x00, 0x00, 0x7B, 0x6F, 0x21, 0x04, 0x42, 0x08, 0x84, 0x10, 0xA5, 0x94, 0xE7, 0x9C, 0x29, 0xA5, 0x1B, 0x00, 0x18, 0x00, 0x16, 0x00, 0x13, 0x00, 0x10, 0x00, 0x0E, 0x00, 0x0B, 0x00, 0x1A, 0x03 };
    unsigned char dallasBlackGreen[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x00, 0x21, 0x04, 0x63, 0x0C, 0xA5, 0x14, 0xE7, 0x9C, 0x29, 0xA5, 0xA0, 0x02, 0x60, 0x82, 0x00, 0x02, 0xC0, 0x01, 0x80, 0x81, 0x20, 0x81, 0xE0, 0x00, 0x1A, 0x83 };
    unsigned char detroitWhiteRed[] = { 0x00, 0x00, 0x7B, 0x6F, 0xAD, 0x35, 0xEF, 0xBD, 0x31, 0xC6, 0x52, 0x4A, 0x94, 0x52, 0xD6, 0xDA, 0x1F, 0x80, 0x1C, 0x00, 0x19, 0x00, 0x16, 0x00, 0x13, 0x00, 0x10, 0x00, 0x0D, 0x00, 0x1A, 0x03 };
    unsigned char edmontonBlueOrange[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x24, 0x00, 0x30, 0x00, 0x3C, 0x00, 0x4C, 0x00, 0x58, 0x00, 0x64, 0x7B, 0x01, 0x59, 0x01, 0x36, 0x01, 0x14, 0x01, 0xF1, 0x00, 0xCF, 0x80, 0xAD, 0x80, 0x1A, 0x83 };
    unsigned char floridaBlueOrange[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x18, 0x00, 0x24, 0x00, 0x34, 0x00, 0x40, 0x00, 0x50, 0x00, 0x60, 0xDF, 0x00, 0xBB, 0x80, 0x98, 0x80, 0x94, 0x80, 0x71, 0x80, 0x4D, 0x00, 0x4A, 0x00, 0x1A, 0x03 };
    unsigned char hartfordGreenBlue[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x01, 0x40, 0x01, 0x80, 0x01, 0xE0, 0x81, 0x20, 0x82, 0x60, 0x02, 0x00, 0x65, 0xE0, 0x58, 0xC0, 0xCC, 0xA0, 0xC0, 0x80, 0xB8, 0x60, 0xAC, 0x60, 0x20, 0x1A, 0x03 };
    unsigned char losAngelesBlackGray[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x00, 0x21, 0x04, 0x63, 0x0C, 0xA5, 0x14, 0xE7, 0x9C, 0x29, 0xA5, 0x52, 0x4A, 0x10, 0x42, 0xCE, 0x39, 0x8C, 0xB1, 0x6B, 0xAD, 0x29, 0x25, 0xE7, 0x1C, 0x1A, 0x83 };
    unsigned char montrealRedBlue[] = { 0x00, 0x00, 0x7B, 0x6F, 0x0A, 0x00, 0x0D, 0x00, 0x11, 0x00, 0x14, 0x00, 0x17, 0x00, 0x1B, 0x00, 0x00, 0x6C, 0x00, 0x60, 0x00, 0x54, 0x00, 0x48, 0x00, 0x3C, 0x00, 0x30, 0x00, 0x28, 0x1A, 0x03 };
    unsigned char newJerseyBlackRed[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x00, 0x21, 0x04, 0x63, 0x0C, 0xA5, 0x14, 0xE7, 0x9C, 0x29, 0xA5, 0x1D, 0x00, 0x1A, 0x00, 0x17, 0x00, 0x13, 0x00, 0x10, 0x00, 0x0D, 0x00, 0x0A, 0x00, 0x1A, 0x03 };
    unsigned char nyIslandersBlueOrange[] = { 0x00, 0x00, 0x7B, 0x6F, 0x40, 0x20, 0x60, 0x2C, 0xA0, 0x3C, 0xC0, 0xC8, 0xE0, 0xD8, 0x20, 0xE5, 0x7E, 0x01, 0x5C, 0x01, 0x39, 0x01, 0x17, 0x01, 0xF5, 0x00, 0xD2, 0x80, 0xD0, 0x80, 0x1A, 0x83 };
    unsigned char nyRangersRedBlue[] = { 0x00, 0x00, 0x7B, 0x6F, 0x0A, 0x00, 0x0D, 0x00, 0x11, 0x00, 0x15, 0x00, 0x19, 0x00, 0x1D, 0x00, 0x00, 0x70, 0x00, 0x64, 0x00, 0x58, 0x00, 0x4C, 0x00, 0x40, 0x00, 0x34, 0x00, 0x28, 0x1A, 0x03 };
    unsigned char ottawaBlackRed[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x00, 0x21, 0x04, 0x63, 0x0C, 0xA5, 0x14, 0xE7, 0x9C, 0x29, 0xA5, 0x18, 0x00, 0x15, 0x00, 0x13, 0x00, 0x10, 0x00, 0x0D, 0x00, 0x0B, 0x00, 0x08, 0x00, 0x1A, 0x03 };
    unsigned char phillyBlackOrange[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x00, 0x21, 0x04, 0x63, 0x0C, 0xA5, 0x14, 0xE7, 0x9C, 0x29, 0xA5, 0x5E, 0x01, 0x3B, 0x01, 0xF8, 0x00, 0xD5, 0x80, 0x92, 0x80, 0x6F, 0x80, 0x2C, 0x00, 0x1A, 0x03 };
    unsigned char pittsburghYellowBrown[] = { 0x00, 0x00, 0x7B, 0x6F, 0xB0, 0x01, 0x13, 0x82, 0x96, 0x02, 0x19, 0x83, 0x7C, 0x03, 0xFF, 0x03, 0x29, 0xA5, 0xE7, 0x1C, 0xC6, 0x98, 0x84, 0x90, 0x63, 0x8C, 0x21, 0x04, 0x00, 0x00, 0x1A, 0x03 };
    unsigned char quebecLightBlueWhite[] = { 0x00, 0x00, 0x7B, 0x6F, 0xC0, 0x34, 0xE0, 0xBC, 0x20, 0xC9, 0x40, 0x55, 0x80, 0x61, 0xA0, 0xED, 0xD6, 0xDA, 0x94, 0xD2, 0x73, 0xCE, 0x31, 0x46, 0x10, 0x42, 0xCE, 0x39, 0xAD, 0xB5, 0x1A, 0x83 };
    unsigned char sanJoseBlackTurquoise[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x00, 0x21, 0x04, 0x63, 0x0C, 0xA5, 0x14, 0xE7, 0x9C, 0x29, 0xA5, 0x60, 0x56, 0x20, 0x4E, 0xE0, 0x45, 0xC0, 0xBD, 0x80, 0xB5, 0x40, 0xAD, 0x00, 0x25, 0x1A, 0x03 };
    unsigned char stLouisYellowBlue[] = { 0x00, 0x00, 0x7B, 0x6F, 0xD1, 0x09, 0x34, 0x86, 0xB7, 0x06, 0x19, 0x83, 0x9C, 0x03, 0xFF, 0x83, 0x00, 0xF4, 0x00, 0x68, 0x00, 0x5C, 0x00, 0x54, 0x00, 0x48, 0x00, 0x3C, 0x00, 0x30, 0x1A, 0x03 };
    unsigned char tampaBayBlackBlue[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x00, 0x21, 0x04, 0x63, 0x0C, 0xA5, 0x14, 0xE7, 0x9C, 0x29, 0xA5, 0x00, 0x7C, 0x00, 0x6C, 0x00, 0x60, 0x00, 0x54, 0x00, 0x48, 0x00, 0x38, 0x00, 0x2C, 0x1A, 0x03 };
    unsigned char torontoWhiteBlue[] = { 0x00, 0x00, 0x7B, 0x6F, 0x8C, 0x31, 0xCE, 0xB9, 0x10, 0xC2, 0x52, 0x4A, 0x94, 0x52, 0xD6, 0xDA, 0x00, 0xF8, 0x00, 0x6C, 0x00, 0x5C, 0x00, 0x50, 0x00, 0x40, 0x00, 0x34, 0x00, 0x28, 0x1A, 0x03 };
    unsigned char vancouverYellowOrange[] = { 0x00, 0x00, 0x7B, 0x6F, 0xAF, 0x01, 0xF1, 0x81, 0x54, 0x82, 0xB7, 0x02, 0x19, 0x83, 0x7C, 0x03, 0x1C, 0x00, 0x19, 0x00, 0x15, 0x00, 0x12, 0x00, 0x0F, 0x00, 0x0B, 0x00, 0x08, 0x00, 0x1A, 0x03 };
    unsigned char washingtonBlueRed[] = { 0x00, 0x00, 0x7B, 0x6F, 0x00, 0x28, 0x00, 0x38, 0x00, 0x48, 0x00, 0x58, 0x00, 0x6C, 0x00, 0x7C, 0x1F, 0x00, 0x1B, 0x00, 0x18, 0x00, 0x14, 0x00, 0x11, 0x00, 0x0D, 0x00, 0x0A, 0x00, 0x1A, 0x03 };
    unsigned char winnipegRedPurple[] = { 0x00, 0x00, 0x7B, 0x6F, 0x0A, 0x00, 0x0E, 0x00, 0x12, 0x00, 0x17, 0x00, 0x1B, 0x00, 0x1F, 0x00, 0x0C, 0x64, 0x0B, 0x58, 0x09, 0x50, 0x08, 0x44, 0x06, 0x3C, 0x05, 0x30, 0x03, 0x28, 0x1A, 0x03 };
    unsigned char aseOrangeWhite[] = { 0x00, 0x00, 0x7B, 0x6F, 0x8B, 0x00, 0xAF, 0x80, 0xF3, 0x80, 0x17, 0x81, 0x3B, 0x01, 0x7F, 0x01, 0xF7, 0x5E, 0xB5, 0xD6, 0x73, 0xCE, 0x31, 0x46, 0xEF, 0x3D, 0xAD, 0xB5, 0x6B, 0xAD, 0x1A, 0x03 };
    unsigned char aswBlackOrange[] = { 0x00, 0x00, 0x7B, 0x6F, 0x21, 0x04, 0x42, 0x08, 0x84, 0x10, 0xA5, 0x94, 0xE7, 0x9C, 0x29, 0xA5, 0x9F, 0x01, 0x7C, 0x81, 0x5A, 0x01, 0x37, 0x01, 0x14, 0x01, 0xF2, 0x00, 0xCF, 0x80, 0x1A, 0x83 };

    // Set up defaults
    TeamSubheaderPaletteDescription paletteROMAddresses[(int)Team::Count] =
    {
        { 0x9AF17C, anaheimWhiteGreen },   // Anaheim
        { 0x9AEF5C, bostonBlackYellow },   // Boston
        { 0x9AF19C, buffaloYellowBlue },   // Buffalo
        { 0x9AF57C, calgaryYellowOrange },   // Calgary
        { 0x9AF53C, chicagoBlackRed },   // Chicago
        { 0x9af4FC, dallasBlackGreen },   // Dallas
        { 0x9AF55C, detroitWhiteRed },   // Detroit
        { 0x9af4Dc, edmontonBlueOrange },   // Edmonton
        { 0x9af4Bc, floridaBlueOrange },   // Florida
        { 0x9af49c, hartfordGreenBlue },   // Hartford
        { 0x9af45c, losAngelesBlackGray },   // LA
        { 0x9af43c, montrealRedBlue },   // Montreal
        { 0x9af41c, newJerseyBlackRed },   // New Jersey
        { 0x9AF3FC, nyIslandersBlueOrange },   // NY Islanders
        { 0x9AF3DC, nyRangersRedBlue },   // NY Rangers
        { 0x9AF3BC, ottawaBlackRed },   // Ottawa
        { 0x9AF39C, phillyBlackOrange },   // Philly
        { 0x9AF37C, pittsburghYellowBrown },   // Pittsburgh
        { 0x9AF35C, quebecLightBlueWhite },   // Quebec
        { 0x9AF31C, sanJoseBlackTurquoise },   // San Jose
        { 0x9AF2DC, stLouisYellowBlue },   // St Louis
        { 0x9AF2BC, tampaBayBlackBlue },   // Tampa Bay
        { 0x9AF29C, torontoWhiteBlue },   // Toronto
        { 0x9AF25C, vancouverYellowOrange },   // Vancouver
        { 0x9AF21C, washingtonBlueRed },   // Washington
        { 0x9AF1FC, winnipegRedPurple },   // Winnipeg
        { 0x9AF23C, aseOrangeWhite },   // All stars east
        { 0x9AF51C, aswBlackOrange },   // All stars west
    };

    for (int i = 0; i < (int)Team::Count; ++i)
    {
        int subheaderPaletteROMAddress = ROMAddressToFileOffset(paletteROMAddresses[i].DataAddress);

        int colorIndex = s_allTeams[i].HeaderColorIndex;
        unsigned char* colorData = paletteROMAddresses[colorIndex].Data;

        RomDataIterator iter(subheaderPaletteROMAddress);
        iter.SaveBytes_EnsureSpaceInBank(colorData, _countof(montrealRedBlue));
    }
}

void SavePlayerPallettes()
{
    for (int i = 0; i < (int)Team::Count; ++i)
    {
        PlayerPallette::ColorSet homeColors = s_playerPallettes[s_allTeams[i].HomeColorIndex].Home;
        PlayerPallette::ColorSet awayColors = s_playerPallettes[s_allTeams[i].AwayColorIndex].Away;

        if (s_allTeams[i].SkinColorOverrideIndex != 0)
        {
            homeColors.Named.Skin = s_skinColorOverrides[s_allTeams[i].SkinColorOverrideIndex].B5G5R5;
            awayColors.Named.Skin = s_skinColorOverrides[s_allTeams[i].SkinColorOverrideIndex].B5G5R5;
        }

        s_romData.SaveBytes(s_playerPallettes[i].Home.SourceDataFileOffset, 0x20, homeColors.Bytes);
        s_romData.SaveBytes(s_playerPallettes[i].Away.SourceDataFileOffset, 0x20, awayColors.Bytes);
    }
}

bool InsertPlayerGraphics(RomDataIterator* freeSpaceIter)
{
    // The profile images are all stored compressed.
    // I believe in work smarter not harder. So this approach does not actualy involve reverse engineering of
    // the whole compression or working out of compressed images. Instead, keep a decompressed copy with this 
    // program and re-write the game's decompress with a dead simple load. I noticed the decompression itself
    // doesn't have external side effects so it is safe to totally replace it.

    // This over-writes the code in function "LoadTeamProfileImagesForGameSetup() / 9DCC42" to do a stupid copy
    // instead of what it normally does, which is
    // - Do a bunch of setup in preparation for DecompressActual1()
    // - Call DecompressActual1()
    // - Call DecompressActual2()

    // There are other code paths (e.g., EDIT LINES) which also call DecompressActual1() and DecompressActual2().

    assert(s_profileImageData.size() == (size_t)Team::Count);

    for (int i = 0; i < (int)Team::Count; ++i)
    {
        ProfileImageData& p = s_profileImageData[i];

        freeSpaceIter->EnsureSpaceInBank(p.ImageBytes.size());
        p.ImageDataROMAddress = freeSpaceIter->GetROMOffset();
        freeSpaceIter->SaveBytes(p.ImageBytes.data(), p.ImageBytes.size());
    }

    // Write the addresses of all the profile data images to a table.
    int tableSize = 4 * (int)Team::Count;
    freeSpaceIter->EnsureSpaceInBank(tableSize);
    int profileDataAddressTableROMAddress = freeSpaceIter->GetROMOffset();
    for (int i = 0; i < (int)Team::Count; ++i)
    {
        freeSpaceIter->SaveLongAddress4Bytes(s_profileImageData[i].ImageDataROMAddress);
    }

    {
        /*
         A0 00 24			LDY #$2400  // initialize a counter
         88					DEY

         // Here: need to seek to the right element of the pointer array based on the team index.
        A9 0B 00            LDA 000B
        0A                  ASL
        0A                  ASL
        AA                  TAX
        BF  __ __ __        LDA ______,X
        85 0C				STA $0C
        E8                  INX
        BF  __ __ __        LDA ______,X
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
        */

        // Reference: CopyProfileImages.asm
        ObjectCode copy;
        copy.AppendLoadYImmediate_A0(0x2400);
        copy.m_code.push_back(0x88); // DEY

        copy.m_code.push_back(0xA6);    // LDX $91    [$00:0091] Load the choice of HomeOrAway. 0 == home, 2 == away	
        copy.m_code.push_back(0x91);
        copy.m_code.push_back(0xBD);
        copy.m_code.push_back(0x98);
        copy.m_code.push_back(0x1C);    // LDA $1C98,x[$9F:1C98]   // Load the index of CurrentTeam. 

        copy.AppendArithmaticShiftAccLeft_0A();
        copy.AppendArithmaticShiftAccLeft_0A();
        copy.m_code.push_back(0xAA); //TAX

        copy.m_code.push_back(0xBF);    // LDA ______,X
        copy.AppendLongAddress3Bytes(profileDataAddressTableROMAddress);
        copy.AppendStoreDirect_85(0x0C);
        copy.m_code.push_back(0xE8); // INX
        copy.m_code.push_back(0xE8); // INX
        copy.m_code.push_back(0xBF);    // LDA ______,X
        copy.AppendLongAddress3Bytes(profileDataAddressTableROMAddress);
        copy.AppendStoreDirect_85(0x0E);

        copy.m_code.push_back(0xA6); // LDX $91
        copy.m_code.push_back(0x91);
        copy.m_code.push_back(0xD0);
        copy.m_code.push_back(0x07);

        copy.AppendLoadAccImmediate_A9_16bit(0x5100);
        copy.AppendStoreDirect_85(0x10);
        copy.m_code.push_back(0x80); // BRA after
        copy.m_code.push_back(0x05);

        copy.AppendLoadAccImmediate_A9_16bit(0x2D00);
        copy.AppendStoreDirect_85(0x10);

        copy.AppendLoadAccImmediate_A9_16bit(0x007F);
        copy.AppendStoreDirect_85(0x12);

        copy.m_code.push_back(0xB7);
        copy.m_code.push_back(0x0C); // LDA [$0C],y
        copy.m_code.push_back(0x97);
        copy.m_code.push_back(0x10); // STA [$10],y
        copy.m_code.push_back(0x88); // DEY
        copy.m_code.push_back(0x10);
        copy.m_code.push_back(0xF9);

        copy.AppendReturnLong_6B();

        int decompressMainStartFileOffset = ROMAddressToFileOffset(0x9DCC42);
        int decompressMainEndFileOffset = ROMAddressToFileOffset(0x9DCCAD);

        // Pad with no-ops for hygiene
        for (int i = decompressMainStartFileOffset; i <= decompressMainEndFileOffset; ++i)
        {
            s_romData.Set(i, 0xEA); // NOP
        }

        RomDataIterator iter(decompressMainStartFileOffset);
        iter.SaveObjectCode(&copy);
    }
    {
        // Turns out this is called not just for edit lines :(
        // How to distinguish between them?
        ObjectCode editLinesPayload;

        // Call the thing that we replaced, so there's no loss of behavior
        editLinesPayload.AppendJumpSubroutineLong_22(0x9DDDB3);

        // Here-- write a black pixel to sysmem

        // Done, return
        editLinesPayload.AppendLongJump_5C(0x9DDDAF);

        freeSpaceIter->EnsureSpaceInBank(editLinesPayload.m_code.size());

        // Detour here: $9D/DDAB 22 B3 DD 9D JSL $9DDDB3[$9D:DDB3]   A:3568 X:0000 Y:FFFE P:envmxdizc
        InsertJumpOutDetour(editLinesPayload.m_code, 0x9DDDAB, 0x9DDDAB + 4, freeSpaceIter);
    }

    return true;
}

System::Void nhl94e::Form1::saveROMToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
    SaveFileDialog^ dialog = gcnew SaveFileDialog();
#if _DEBUG
    dialog->FileName = L"E:\\Emulation\\SNES\\Images\\Test\\nhl94em.sfc";
#endif
    dialog->Filter = "SFC files (*.sfc)|*.sfc|All files (*.*)|*.*";

    System::Windows::Forms::DialogResult result = dialog->ShowDialog();
    if (result != System::Windows::Forms::DialogResult::OK)
        return;

    std::wstring outputFilename = ManagedToWideString(dialog->FileName);
    
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

    RomDataIterator freeSpaceIter(ROMAddressToFileOffset(0xA08000));

    SaveHeaderColors();

    SavePlayerPallettes();

    if (!InsertTeamLocationText(&freeSpaceIter))
    {
        System::String^ dialogString = gcnew System::String(L"Encountered an error loading the contents of the file LoadLongAddress_ArrayElement_Into_8D.asm.");
        MessageBox::Show(dialogString);
        return;
    }

    if (!InsertTeamAcronymText(&freeSpaceIter))
    {
        System::String^ dialogString = gcnew System::String(L"Encountered an error loading the contents of the file LoadLongAddress_ArrayElement_Into_8D.asm.");
        MessageBox::Show(dialogString);
        return;
    }

    if (!InsertTeamNameOrVenueText(&freeSpaceIter))
    {
        System::String^ dialogString = gcnew System::String(L"Encountered an error loading the contents of the file LoadLongAddress_ArrayElement_Into_8D.asm.");
        MessageBox::Show(dialogString);
        return;
    }

    if (!InsertPlayerNameText(&freeSpaceIter))
    {
        System::String^ dialogString = gcnew System::String(L"Encountered an error loading the contents of the file LookupPlayerNameDet.asm.");
        MessageBox::Show(dialogString);
        return;
    }

    if (!InsertPlayerGraphics(&freeSpaceIter))
    {
        System::String^ dialogString = gcnew System::String(L"Encountered an error inserting player graphics.");
        MessageBox::Show(dialogString);
        return;
    }

    s_romData.SaveBytesToFile(outputFilename.c_str());

    MessageBox::Show(L"Output file saved.", L"Info");
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
    teamNameTextBox->Text = NarrowASCIIStringToManaged(s_allTeams[teamIndex].TeamName.Get());
    teamVenueTextBox->Text = NarrowASCIIStringToManaged(s_allTeams[teamIndex].Venue.Get());

    // Update combo boxes to reflect the current team selection
    this->headerColorComboBox->SelectedIndex = s_allTeams[teamIndex].HeaderColorIndex;
    this->homeColorComboBox->SelectedIndex = s_allTeams[teamIndex].HomeColorIndex;
    this->awayColorComboBox->SelectedIndex = s_allTeams[teamIndex].AwayColorIndex;

    this->skinColorOverrideComboBox->SelectedIndex = s_allTeams[teamIndex].SkinColorOverrideIndex;
}

void nhl94e::Form1::locationTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;
    std::string newName = ManagedToNarrowASCIIString(locationTextBox->Text);
    s_allTeams[teamIndex].TeamCity.Set(newName);
}

void nhl94e::Form1::acronymTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;
    std::string newName = ManagedToNarrowASCIIString(acronymTextBox->Text);
    s_allTeams[teamIndex].Acronym.Set(newName);
}


void nhl94e::Form1::teamNameTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;
    std::string newName = ManagedToNarrowASCIIString(teamNameTextBox->Text);
    s_allTeams[teamIndex].TeamName.Set(newName);
}

void nhl94e::Form1::teamVenueTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;
    std::string newName = ManagedToNarrowASCIIString(teamVenueTextBox->Text);
    s_allTeams[teamIndex].Venue.Set(newName);
}

void nhl94e::Form1::headerColorComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;
    s_allTeams[teamIndex].HeaderColorIndex = headerColorComboBox->SelectedIndex;
}

void nhl94e::Form1::homeColorComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
    int teamIndex = this->tabControl1->SelectedIndex;
    s_allTeams[teamIndex].HomeColorIndex = homeColorComboBox->SelectedIndex;
}

void nhl94e::Form1::awayColorComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
    int teamIndex = this->tabControl1->SelectedIndex;
    s_allTeams[teamIndex].AwayColorIndex = awayColorComboBox->SelectedIndex;
}

void nhl94e::Form1::skinColorOverrideComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
    int teamIndex = this->tabControl1->SelectedIndex;
    s_allTeams[teamIndex].SkinColorOverrideIndex = skinColorOverrideComboBox->SelectedIndex;

    if (skinColorOverrideComboBox->SelectedIndex == 0)
    {
        skinColorOverrideComboBox->BackColor = System::Drawing::Color::White;
        skinColorOverrideComboBox->ForeColor = System::Drawing::Color::Black;
    }
    else
    {
        int backArgb = s_skinColorOverrides[skinColorOverrideComboBox->SelectedIndex].A8R8G8B8;
        int textArgb = s_skinColorOverrides[skinColorOverrideComboBox->SelectedIndex].UITextColorARGB;

        skinColorOverrideComboBox->BackColor = System::Drawing::Color::FromArgb(backArgb);
        skinColorOverrideComboBox->ForeColor = System::Drawing::Color::FromArgb(textArgb);
    }
}

void nhl94e::Form1::OverwriteProfileImage(
    int teamIndex,
    MultiFormatPallette* importedPallette,
    std::vector<unsigned char>* importedSnesImageData)
{
    // Put pallette back into ROM
    {
        int addr = s_profilePalletteData[teamIndex].PalletteROMAddress;
        RomDataIterator iter(ROMAddressToFileOffset(addr));
        for (int i = 0; i < 16; ++i)
        {
            int color = importedPallette->SnesB5G5R5[i];
            unsigned char low = color & 0xFF;
            color >>= 8;
            unsigned char high = color & 0xFF;
            color >>= 8;
            assert(color == 0);

            iter.SaveByte(low);
            iter.SaveByte(high);

            // Update the cached copy too
            s_profilePalletteData[teamIndex].PalletteBytes[i * 2 + 0] = low;
            s_profilePalletteData[teamIndex].PalletteBytes[i * 2 + 1] = high;
        }
    }

    // Put image data back into ROM
    assert(importedSnesImageData->size() == 0x2400 && importedSnesImageData->size() == s_profileImageData[teamIndex].ImageBytes.size());
    std::copy(importedSnesImageData->begin(), importedSnesImageData->end(), s_profileImageData[teamIndex].ImageBytes.begin());
}

void nhl94e::Form1::profileImagesButton_Click(System::Object^ sender, System::EventArgs^ e) 
{
    int teamIndex = this->tabControl1->SelectedIndex;

    Form2^ dialog = gcnew Form2();
    dialog->SetProfileData(&(s_profileImageData[teamIndex]), &(s_profilePalletteData[teamIndex]));
    dialog->ShowDialog();

    if (dialog->DialogResult != System::Windows::Forms::DialogResult::OK)
    {
        return;
    }

    if (!dialog->ImportedSomethingValid())
    {
        return;
    }

    MultiFormatPallette* importedPallette = dialog->GetImportedPallette();
    std::vector<unsigned char>* importedSnesImageData = dialog->GetImportedSnesImageData();

    OverwriteProfileImage(teamIndex, importedPallette, importedSnesImageData);
}