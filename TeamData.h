#pragma once

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
    int Roughness; // Not reported in the game card
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