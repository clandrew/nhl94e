#pragma once

enum class Handedness
{
    Left, Right
};

template<typename T>
struct ModifiableStat
{
    T OriginalValue;
    T NewValue;
};

struct PlayerData
{
    int OriginalROMAddress;
    int ReplacedROMAddressForRename;
    std::string Name;

    ModifiableStat<int> PlayerNumber;

    int WeightFactor;
    int WeightInPounds;

    ModifiableStat<int> BaseAgility;
    ModifiableStat<int> BaseSpeed;

    ModifiableStat<int> BaseOffAware;
    ModifiableStat<int> BaseDefAware;

    ModifiableStat<int> BaseShotPower;

    ModifiableStat<int> BaseChecking;

    int HandednessValue;
    Handedness WhichHandedness;

    ModifiableStat<int> BaseStickHandling;
    ModifiableStat<int> BaseShotAccuracy;
    ModifiableStat<int> BaseEndurance;
    ModifiableStat<int> Roughness; // Not reported in the game card
    ModifiableStat<int> BasePassAccuracy;
    ModifiableStat<int> BaseAggression;
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