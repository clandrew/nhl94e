#pragma once

unsigned int GetFileOffsetFromROMAddress(unsigned int romAddress);

long GetFileSize(FILE* file);

std::vector<unsigned char> LoadBytesFromFile(const wchar_t* fileName);

void SaveBytesToFile(const wchar_t* fileName, std::vector<unsigned char>& romData);

int SnesB5G5R5ToR8B8G8(unsigned short snesFmt);
unsigned short R8B8G8ToSnesB5G5R5(int rgbFmt);