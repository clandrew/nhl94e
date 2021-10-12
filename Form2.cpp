#include "pch.h"
#include "Form2.h"
#include "Utils.h"

void nhl94e::Form2::OnLoad(System::Object^ sender, System::EventArgs^ e)
{
	demoBitmap = gcnew System::Drawing::Bitmap(L"0B_0.png");
	demoBitmap->SetResolution(96, 96);
}

void nhl94e::Form2::SetProfileData(ProfileImageData* img, ProfilePalletteData* pal)
{
	profileImageData = img;
	profilePalletteData = pal;

	demoBitmap2 = gcnew System::Drawing::Bitmap(48, 48);
	demoBitmap2->SetResolution(96, 96);

	int snesToRgb[16]; 
	for (int i = 0; i < 16; ++i)
	{
		int byte0Index = i*2;
		int byte1Index = byte0Index + 1;
		unsigned short indexed = (pal->PalletteBytes[byte1Index] << 8) | pal->PalletteBytes[byte0Index];
		int rgb = SnesB5G5R5ToR8B8G8(indexed);
		rgb |= 0xFF000000;
		snesToRgb[i] = rgb;
	}

	int destBlock = 0;
	for (int i = 0; i < 0x480; i += 0x20)
	{
		unsigned char* blockStart = &(img->ImageBytes[i]);
		int ind[8][8]{};

		for (int row = 0; row < 8; ++row)
		{
			int mask = 0x80;
			for (int col = 0; col < 8; ++col)
			{
				int byteIndex0 = row * 2;
				int byteIndex1 = byteIndex0 + 1;
				int byteIndex2 = byteIndex0 + 0x10;
				int byteIndex3 = byteIndex2 + 1;

				if (blockStart[byteIndex0] & mask)
				{
					ind[row][col] |= 0x1;
				}
				if (blockStart[byteIndex1] & mask)
				{
					ind[row][col] |= 0x2;
				}
				if (blockStart[byteIndex2] & mask)
				{
					ind[row][col] |= 0x4;
				}
				if (blockStart[byteIndex3] & mask)
				{
					ind[row][col] |= 0x8;
				}

				mask >>= 1;
			}
		}

		int blockIndexX = destBlock % 6;
		int blockIndexY = destBlock / 6;

		int blockStartX = blockIndexX * 8;
		int blockStartY = blockIndexY * 8;

		for (int row = 0; row < 8; ++row)
		{
			for (int col = 0; col < 8; ++col)
			{
				int index = ind[row][col];
				assert(index >= 0 && index <= 15);
				int rgb = snesToRgb[index];

				System::Drawing::Color color = System::Drawing::Color::FromArgb(rgb);
				demoBitmap2->SetPixel(blockStartX + col, blockStartY + row, color);
			}
		}

		destBlock++;
	}
}

void nhl94e::Form2::panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) 
{
	int scaling = 4;
	e->Graphics->InterpolationMode = System::Drawing::Drawing2D::InterpolationMode::NearestNeighbor;
	e->Graphics->Clear(System::Drawing::Color::Transparent);

	// I don't know why the +2 is necessary and at this point I can't be bothered to find out.
	e->Graphics->DrawImage(demoBitmap2, 0, 0, 48 * scaling + 2, 48 * scaling + 2);
}

void nhl94e::Form2::saveTemplateBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{

}