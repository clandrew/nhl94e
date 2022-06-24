// Decompress2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>

std::vector<unsigned char> expected;
std::vector<unsigned char> actual;
int validationLength = 1;

int main()
{
	// Expected values are in Expected.bin.
    FILE* file{};
    fopen_s(&file, "Expected.bin", "rb");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    expected.resize(length);
    fseek(file, 0, SEEK_SET);
    fread(expected.data(), 1, length, file);
    fclose(file);

    // First value is at 7F5100.
    // What writes 00 to 0x7F5100?
    // It's $9B/864B 91 10       STA ($10),y[$7F:5100]   A:0000 X:0008 Y:0000 P:envmxdiZC
    actual.push_back(0);

    if (actual.size() < validationLength)
    {
        __debugbreak();
        return -1;
    }

    for (int i = 0; i < validationLength; ++i)
    {
        if (expected[i] != actual[i])
        {
            __debugbreak();
            return -1;
        }
    }

    return 0;
}