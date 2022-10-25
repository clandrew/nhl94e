#include "Decompress2.h"

int main()
{
    Decompress_Slow_Init();

    for (int teamIndex = (int)Team::Anaheim; teamIndex <= (int)Team::AllStarsWest; ++teamIndex)
    {
        for (int playerIndex = 0; playerIndex < 6; ++playerIndex)
        {
            Decompress_Slow_Run(teamIndex, playerIndex);
        }
    }

    return 0;
}