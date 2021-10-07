// PreDecompress.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <assert.h>

int token = 0;
int counter = 8;
int pointer = 0;
unsigned char compressedData[] = { 
	0x30, 0xFB, 0x00, 0x01, 0x00, 0xA1, 0x93, 0xE2, 0x90, 0x21, 0x88, 0xF8, 0x40, 0x04, 0x4B, 0xD6, 0xAF, 0xFF, 0xFD, 0xAB, 0x7F, 0x82, 0x20, 0x07, 0x0E, 0xFD, 0xAE, 0xB7, 0xD9, 0x65, 0x61, 0x5C,
	0xD8, 0xD5, 0x51, 0x4D, 0x48, 0xC1, 0x44, 0x38, 0x38, 0x83, 0xC8, 0x09, 0x00 };

unsigned char array80C2B6[] = { 0xA2, 0x10, 0x80, 0xD0, 0x00, 0x00, 0x04, 0x00, 0x0C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x7C, 0x00 };

int var14 = 0x14;
int var6C = 0;
int var6F = 0;
int result2Accum = 0;
int var77 = 0;
int var79 = 0;

int MultiplyBy2WithOverflowCheck(int n, bool* overflow)
{
	int result = n * 2;
	if (result < 0x10000)
	{
		*overflow = false;
		return result;
	}
	else
	{
		*overflow = true;
		return result - 0x10000;
	}
}

void DecrementCounter_LoadNextTokenIfZero()
{
	counter--;
	if (counter == 0)
	{
		// 8bit mode
		token &= 0xFF00;
		token |= compressedData[pointer];
		pointer++;
		counter = 8;
	}
}

int RotateLeft(int n, bool* carry)
{
	n <<= 1;
	if (*carry)
	{
		n |= 1;
	}

	*carry = n > 0xFFFF;
	n &= 0xFFFF;
	return n;
}

int PreDecompress()
{
	var6F = 0;
	bool overflow = false;
	token = var6C;

	token = MultiplyBy2WithOverflowCheck(token, &overflow);
	DecrementCounter_LoadNextTokenIfZero();

	if (!overflow)
	{
		int index = 2;
		do
		{
			token = MultiplyBy2WithOverflowCheck(token, &overflow);
			DecrementCounter_LoadNextTokenIfZero();
			index++;
		} while (!overflow);

		unsigned char baseElement = array80C2B6[index * 2]; // Array of shorts i guess

		do
		{
			token = MultiplyBy2WithOverflowCheck(token, &overflow);
			var6F = RotateLeft(var6F, &overflow);
			DecrementCounter_LoadNextTokenIfZero();
			index--;
		} while (index > 0);

		var6C = token;
		int result = baseElement + var6F;
		return result;
	}
	else
	{
		token = MultiplyBy2WithOverflowCheck(token, &overflow);
		var6F = RotateLeft(var6F, &overflow); 
		DecrementCounter_LoadNextTokenIfZero();

		token = MultiplyBy2WithOverflowCheck(token, &overflow);
		var6F = RotateLeft(var6F, &overflow);
		DecrementCounter_LoadNextTokenIfZero();
		if (counter == 8)
		{
			var6C = token;
			return token;
		}
		else
		{
			// Store result into 6C
			// Load A out of 6F
			var6C = token;
			int result = var6F;
			return result;
		}
	}
}

int main()
{
	int index = 0;
	int initialToken = compressedData[6];
	initialToken <<= 8;
	initialToken |= compressedData[7];
	var6C = initialToken;
	pointer = 8;

	std::vector<int> results;
	std::vector<int> results2;

	for (int j = 0; j < 5; j++)
	{
		int preDecompressResult = PreDecompress();
		results.push_back(preDecompressResult);

		var77 += preDecompressResult;

		result2Accum += var6F;

		if (var6F == 0)
		{
			results2.push_back(0);
			// PreDecompress again
		}
		else
		{
			if (var14 == 0)
			{
				assert(false);
			}
			else
			{
				int result2 = result2Accum; // result2 stored as var00
				bool overflow = false;
				for (int i = 0; i < var14; ++i)
				{
					result2 = MultiplyBy2WithOverflowCheck(result2, &overflow);
				}
				results2.push_back(result2);
				assert(!overflow); // Assuming no overflow here

				var79 = index / 2; // sizeInElements?
				index = 0x3E; 
			}
		}
	}

	assert(results[0] == 0);
	assert(results[1] == 0);
	assert(results[2] == 3);
	assert(results[3] == 2);
	assert(results[4] == 0x10);
}
