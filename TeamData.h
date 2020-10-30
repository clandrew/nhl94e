#pragma once

enum class Handedness
{
    Left, Right
};

enum class WhichStat
{
    PlayerIndex = 0,
    PlayerName = 1,
    PlayerNumber = 2,
    WeightClass = 3,
    Agility = 4,
    Speed = 5,
    OffAware = 6,
    DefAware = 7,
    ShotPower = 8,
    Checking = 9,
    Handedness = 10,
    StickHandling = 11,
    ShotAccuracy = 12,
    Endurance = 13,
    Roughness = 14,
    PassAccuracy = 15,
    Aggression = 16
};

template<typename T>
struct ModifiableStat
{
    int SourceROMAddress;

public:
    void Initialize(T n)
    {
        OriginalValue = n;
        NewValue = n;
    }

    bool IsChanged() const
    {
        return OriginalValue != NewValue;
    }

    void Set(T const& v)
    {
        NewValue = v;
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
    std::string Name;

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
    }
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