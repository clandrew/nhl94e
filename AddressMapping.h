#pragma once

int ROMAddressToFileOffset(int romAddress);
int FileOffsetToROMAddress(int fileOffset);
bool ROMAddressRangeCrossesPageBoundary(int beginAddr, int endAddr);