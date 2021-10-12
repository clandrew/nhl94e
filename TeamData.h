#pragma once

enum class Handedness
{
    Left, Right
};

enum class WhichStat
{
    PlayerIndex = 0,
    PlayerName = 1,
    ProfileImage = 2,
    PlayerNumber = 3,
    WeightClass = 4,
    Agility = 5,
    Speed = 6,
    OffAware = 7,
    DefAware = 8,
    ShotPower = 9,
    Checking = 10,
    Handedness = 11,
    StickHandling = 12,
    ShotAccuracy = 13,
    Endurance = 14,
    Roughness = 15,
    PassAccuracy = 16,
    Aggression = 17
};

template<typename T>
struct ModifiableStat
{
    int SourceROMAddress;

public:
    void Initialize(int srcAddr, T n)
    {
        OriginalValue = n;
        NewValue = n;
        SourceROMAddress = srcAddr;
    }

    bool IsChanged() const
    {
        return OriginalValue != NewValue;
    }

    void Set(T const& v)
    {
        NewValue = v;
    }

    T const& GetOriginal() const
    {
        return OriginalValue;
    }

    T const& Get() const
    {
        return NewValue;
    }

private:
    T OriginalValue;
    T NewValue;
};

struct PlayerData
{
    int OriginalROMAddress;
    int ReplacedROMAddressForRename;
    ModifiableStat<std::string> Name;

    ModifiableStat<int> PlayerNumber;

    ModifiableStat<int> WeightFactor;
    int WeightInPounds;

    ModifiableStat<int> BaseAgility;
    ModifiableStat<int> BaseSpeed;

    ModifiableStat<int> BaseOffAware;
    ModifiableStat<int> BaseDefAware;

    ModifiableStat<int> BaseShotPower;

    ModifiableStat<int> BaseChecking;

    ModifiableStat<int> HandednessValue;
    Handedness WhichHandedness;

    ModifiableStat<int> BaseStickHandling;
    ModifiableStat<int> BaseShotAccuracy;
    ModifiableStat<int> BaseEndurance;
    ModifiableStat<int> Roughness; // Not reported in the game card
    ModifiableStat<int> BasePassAccuracy;
    ModifiableStat<int> BaseAggression;

    bool HasProfileImage;

    void SetNumericalStat(WhichStat s, int v)
    {
        switch (s)
        {
            case WhichStat::PlayerNumber: PlayerNumber.Set(v); break;
            case WhichStat::WeightClass: WeightFactor.Set(v); break;
            case WhichStat::Agility: BaseAgility.Set(v); break;
            case WhichStat::Speed: BaseSpeed.Set(v); break;
            case WhichStat::OffAware: BaseOffAware.Set(v); break;
            case WhichStat::DefAware: BaseDefAware.Set(v); break;
            case WhichStat::ShotPower: BaseShotPower.Set(v); break;
            case WhichStat::Checking: BaseChecking.Set(v); break;
            case WhichStat::Handedness: HandednessValue.Set(v); break;
            case WhichStat::StickHandling: BaseStickHandling.Set(v); break;
            case WhichStat::ShotAccuracy: BaseShotAccuracy.Set(v); break;
            case WhichStat::Endurance: BaseEndurance.Set(v); break;
            case WhichStat::Roughness: Roughness.Set(v); break;
            case WhichStat::PassAccuracy: BasePassAccuracy.Set(v); break;
            case WhichStat::Aggression: BaseAggression.Set(v); break;

            case WhichStat::PlayerIndex: 
            case WhichStat::PlayerName:
                System::Diagnostics::Debug::Assert(false); // Unexpected stat
                break;
        }
    }

    bool IsNumericalStatChanged(WhichStat s)
    {
        switch (s)
        {
        case WhichStat::PlayerNumber: return PlayerNumber.IsChanged(); break;
        case WhichStat::WeightClass: return WeightFactor.IsChanged(); break;
        case WhichStat::Agility: return BaseAgility.IsChanged(); break;
        case WhichStat::Speed: return BaseSpeed.IsChanged(); break;
        case WhichStat::OffAware: return BaseOffAware.IsChanged(); break;
        case WhichStat::DefAware: return BaseDefAware.IsChanged(); break;
        case WhichStat::ShotPower: return BaseShotPower.IsChanged(); break;
        case WhichStat::Checking: return BaseChecking.IsChanged(); break;
        case WhichStat::Handedness: return HandednessValue.IsChanged(); break;
        case WhichStat::StickHandling: return BaseStickHandling.IsChanged(); break;
        case WhichStat::ShotAccuracy: return BaseShotAccuracy.IsChanged(); break;
        case WhichStat::Endurance: return BaseEndurance.IsChanged(); break;
        case WhichStat::Roughness: return Roughness.IsChanged(); break;
        case WhichStat::PassAccuracy: return BasePassAccuracy.IsChanged(); break;
        case WhichStat::Aggression: return BaseAggression.IsChanged(); break;

        case WhichStat::PlayerIndex:
        case WhichStat::PlayerName:
            System::Diagnostics::Debug::Assert(false); // Unexpected stat
            break;
        }

        return false;
    }
};

struct StringWithSourceROMAddress
{
    std::string Str;
    int SourceROMAddress;
};

struct TeamData
{
    int SourceDataROMAddress;
    int DestDataROMAddress;
    std::vector<unsigned char> Header;
    ModifiableStat<std::string> TeamCity;
    ModifiableStat<std::string> Acronym;
    ModifiableStat<std::string> TeamName;
    ModifiableStat<std::string> Venue;
    std::vector<PlayerData> Players;
    int HeaderColorIndex;
    int HomeColorIndex;
    int AwayColorIndex;
    int SkinColorOverrideIndex; // 0 if unused

    PlayerData* GetPlayerByOriginalName(std::string name)
    {
        for (int i = 0; i < Players.size(); ++i)
        {
            if (Players[i].Name.GetOriginal().find(name) != -1)
            {
                return &Players[i];
            }
        }
        return nullptr;
    }
};

struct PlayerPallette
{
    struct ColorSet
    {
        int SourceDataROMAddress;
        int SourceDataFileOffset;

        union
        {
            struct
            {
                unsigned short Reserved;
                unsigned short Skateblade;
                unsigned short Skate;
                unsigned short Gear0;
                unsigned short Gear1;
                unsigned short Gear2;
                unsigned short Gear3;
                unsigned short Skin;

                unsigned short Gear4;
                unsigned short Gear5;
                unsigned short Gear6;
                unsigned short Gear7;
                unsigned short Gear8;
                unsigned short Gear9;
                unsigned short Wood;
                unsigned short IceShadow;
            } Named;
            unsigned char Bytes[0x20];
        };
    };
    ColorSet Home;
    ColorSet Away;
};

struct ProfilePalletteData
{
    int PalletteFileOffset;
    int PalletteROMAddress;
    std::vector<unsigned char> PalletteBytes;
};

struct ProfileImageData
{
    int ImageDataROMAddress;
    std::wstring Path;
    std::vector<unsigned char> ImageBytes;
};