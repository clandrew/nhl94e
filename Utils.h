#pragma once

unsigned int GetFileOffsetFromROMAddress(unsigned int romAddress);

long GetFileSize(FILE* file);

std::vector<unsigned char> LoadBytesFromFile(const wchar_t* fileName);

void SaveBytesToFile(const wchar_t* fileName, std::vector<unsigned char>& romData);
