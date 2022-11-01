#include "Decompress2.h"
#include <iostream>

enum class DecompressMode
{
    Slow,
    Fast
} g_mode;

int main()
{
    g_mode = DecompressMode::Fast;

    if (g_mode == DecompressMode::Slow)
    {
        Decompress_Slow_Init();
    }
    else if (g_mode == DecompressMode::Fast)
    {
        if (!Decompress_Fast_Init())
        {
            std::cout << "Error opening validation file.";
            return -1;
        }
    }
    else
    {
        __debugbreak();
    }

    for (int teamIndex = (int)Team::Anaheim; teamIndex <= (int)Team::AllStarsWest; ++teamIndex)
    {
        for (int playerIndex = 0; playerIndex < 6; ++playerIndex)
        {

            if (g_mode == DecompressMode::Slow)
            {
                Decompress_Slow_Run(teamIndex, playerIndex);
            }
            else if (g_mode == DecompressMode::Fast)
            {
                Decompress_Fast_Run(teamIndex, playerIndex);
            }
            else
            {
                __debugbreak();
            }
        }
    }

    return 0;
}