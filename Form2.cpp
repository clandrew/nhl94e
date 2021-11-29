#include "pch.h"
#include "Form2.h"
#include "Utils.h"

MultiFormatPallette s_default{};
MultiFormatPallette s_importedPallette{};
std::vector<unsigned char> s_snesImageData;

void nhl94e::Form2::OnLoad(System::Object^ sender, System::EventArgs^ e)
{
	m_importedSomethingValid = false;
}

void nhl94e::Form2::SetProfileData(ProfileImageData* img, ProfilePalletteData* pal)
{
	m_profileImageData = img;
	m_profilePalletteData = pal;

	m_demoBitmap = gcnew System::Drawing::Bitmap(48 * 6, 48);
	m_demoBitmap->SetResolution(96, 96);

	for (int i = 0; i < 16; ++i)
	{
		int byte0Index = i*2;
		int byte1Index = byte0Index + 1;
		unsigned short snesB5G5R5 = (pal->PalletteBytes[byte1Index] << 8) | pal->PalletteBytes[byte0Index];
		int portableR8G8B8 = SnesB5G5R5ToR8B8G8(snesB5G5R5);
		portableR8G8B8 |= 0xFF000000;

		s_default.SnesB5G5R5[i] = snesB5G5R5;
		s_default.PortableR8G8B8[i] = portableR8G8B8;
	}

	for (int imgIndex = 0; imgIndex < 6; ++imgIndex)
	{
		int imgBytesOffset = imgIndex * 0x600;

		int destBlock = 0;
		for (int i = 0; i < 0x480; i += 0x20)
		{
			unsigned char* blockStart = &(img->ImageBytes[imgBytesOffset + i]);
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

			blockStartX += imgIndex * 48;

			for (int row = 0; row < 8; ++row)
			{
				for (int col = 0; col < 8; ++col)
				{
					int index = ind[row][col];
					assert(index >= 0 && index <= 15);
					int rgb = s_default.PortableR8G8B8[index];

					System::Drawing::Color color = System::Drawing::Color::FromArgb(rgb);
					m_demoBitmap->SetPixel(blockStartX + col, blockStartY + row, color);
				}
			}

			destBlock++;
		}
	}
}

void nhl94e::Form2::panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) 
{
	int scaling = 4;
	e->Graphics->InterpolationMode = System::Drawing::Drawing2D::InterpolationMode::NearestNeighbor;
	e->Graphics->Clear(System::Drawing::Color::Transparent);

	// I don't know why the +2 is necessary and at this point I can't be bothered to find out.
	e->Graphics->DrawImage(m_demoBitmap, 0, 0, 48 * 6 * scaling + 2, 48 * scaling + 2);
}

void nhl94e::Form2::saveTemplateBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	SaveFileDialog^ dialog = gcnew SaveFileDialog();
#if _DEBUG
	dialog->FileName = L"D:\\repos\\nhl94e\\template.png";
#endif
	dialog->Filter = "PNG files (*.png)|*.png|All files (*.*)|*.*";
	System::Windows::Forms::DialogResult result = dialog->ShowDialog();
	if (result != System::Windows::Forms::DialogResult::OK)
		return;

	System::Drawing::Bitmap^ templateBitmap = gcnew System::Drawing::Bitmap(48 * 6, 50);
	
	{
		System::Drawing::Graphics^ ctx = System::Drawing::Graphics::FromImage(templateBitmap);
		ctx->Clear(System::Drawing::Color::Black);
		ctx->DrawImage(m_demoBitmap, 0, 0);
	}
	for (int i = 0; i < 16; ++i)
	{
		int rgb = s_default.PortableR8G8B8[i];
		System::Drawing::Color color = System::Drawing::Color::FromArgb(rgb);
		templateBitmap->SetPixel(i, 49, color);
	}

	templateBitmap->Save(dialog->FileName);
}

struct RgbToIndexedResult
{
	unsigned char Indexed;
	bool ValidColor;
};

RgbToIndexedResult RgbToIndexed(int rgb, MultiFormatPallette* importedPallette)
{
	RgbToIndexedResult result{};

	// Try to find rgb in the pallette
	for (unsigned char i = 0; i < 16; ++i)
	{
		int rgbIgnoreAlpha = rgb & 0xFFFFFF;
		int palletteIgnoreAlpha = importedPallette->PortableR8G8B8[i] & 0xFFFFFF;

		if (rgbIgnoreAlpha == palletteIgnoreAlpha)
		{
			result.Indexed = i;
			result.ValidColor = true;
			return result;
		}
	}

	result.Indexed = 0;
	result.ValidColor = false;
	return result;
}

struct TranscodeBlockResult
{
	std::vector<unsigned char> TranscodedBlock;
	bool ValidColors;
	int InvalidColorX;
	int InvalidColorY;
};

TranscodeBlockResult TranscodeBlock(std::vector<int> const& rgbBlock, MultiFormatPallette* importedPallette)
{
	TranscodeBlockResult result{};
	result.TranscodedBlock.resize(32);

	for (int yi = 0; yi < 8; ++yi)
	{
		for (int xi = 0; xi < 8; ++xi)
		{
			int rgbx = 8 - 1 - xi;
			int rgby = yi;

			RgbToIndexedResult rgbToIndexedResult = RgbToIndexed(rgbBlock[rgby * 8 + rgbx], importedPallette);
			if (!rgbToIndexedResult.ValidColor)
			{
				result.ValidColors = false;
				result.InvalidColorX = 8 - xi - 1;
				result.InvalidColorY = yi;
				return result;
			}

			unsigned char ic = rgbToIndexedResult.Indexed;

			unsigned char i0 = (ic & 0x1) >> 0;
			unsigned char i1 = (ic & 0x2) >> 1;
			unsigned char i2 = (ic & 0x4) >> 2;
			unsigned char i3 = (ic & 0x8) >> 3;

			if (i0)
			{
				result.TranscodedBlock[yi * 2 + 0x0] |= (0x1 << xi);
			}
			if (i1)
			{
				result.TranscodedBlock[yi * 2 + 0x1] |= (0x1 << xi);
			}
			if (i2)
			{
				result.TranscodedBlock[yi * 2 + 0x10] |= (0x1 << xi);
			}
			if (i3)
			{
				result.TranscodedBlock[yi * 2 + 0x11] |= (0x1 << xi);
			}
		}
	}

	result.ValidColors = true;
	return result;
}

void nhl94e::Form2::importButton_Click(System::Object^ sender, System::EventArgs^ e) 
{
	OpenFileDialog^ dialog = gcnew OpenFileDialog();
#if _DEBUG
	dialog->FileName = L"D:\\repos\\nhl94e\\template.png";
#endif

	System::Windows::Forms::DialogResult result = dialog->ShowDialog();
	if (result != System::Windows::Forms::DialogResult::OK)
		return;

	System::Drawing::Bitmap^ templateBitmap = gcnew System::Drawing::Bitmap(dialog->FileName);

	// Verify expected sizes
	if (templateBitmap->Size.Width != 48 * 6)
	{
		System::Windows::Forms::MessageBox::Show("The template image was an unexpected width; 288 was expected.");
		m_importedSomethingValid = false;
		return;
	}

	if (templateBitmap->Size.Height != 48 + 2)
	{
		System::Windows::Forms::MessageBox::Show("The template image was an unexpected height; 50 was expected.");
		m_importedSomethingValid = false;
		return;
	}

	for (int i = 0; i < 16; ++i)
	{
		System::Drawing::Color pixelColor = templateBitmap->GetPixel(i, 48 - 1 + 2);
		int argb = pixelColor.ToArgb();
		s_importedPallette.PortableR8G8B8[i] = argb;
		s_importedPallette.SnesB5G5R5[i] = R8B8G8ToSnesB5G5R5(s_importedPallette.PortableR8G8B8[i]);
	}

	// Commit image data back into rom data, validating as we go
	int dstOffset = 0;
	s_snesImageData.clear();
	s_snesImageData.resize(0x2400);
	std::fill(s_snesImageData.begin(), s_snesImageData.end(), 0);

	for (int imageIndex = 0; imageIndex < 6; ++imageIndex)
	{
		for (int y = 0; y < 48; y += 8)
		{
			for (int x = 0; x < 48; x += 8)
			{
				std::vector<int> block;
				block.resize(8 * 8);
				for (int yi = 0; yi < 8; ++yi)
				{
					for (int xi = 0; xi < 8; ++xi)
					{
						int srcX = x + xi;
						int srcY = y + yi;

						srcX += imageIndex * 48;

						System::Drawing::Color pixelColor = templateBitmap->GetPixel(srcX, srcY);
						int argb = pixelColor.ToArgb();
						block[yi * 8 + xi] = argb;
					}
				}

				TranscodeBlockResult transcodeBlockResult = TranscodeBlock(block, &s_importedPallette);

				if (!transcodeBlockResult.ValidColors)
				{
					int invalidColorX = x + transcodeBlockResult.InvalidColorX;
					int invalidColorY = y + transcodeBlockResult.InvalidColorY;
					String^ msg = String::Format(
						"The image contained a color at X={0}, Y={1} which was not present in the pallette.",
						invalidColorX,
						invalidColorY);
					System::Windows::Forms::MessageBox::Show(msg);
					m_importedSomethingValid = false;
					return;
				}

				for (size_t i = 0; i < transcodeBlockResult.TranscodedBlock.size(); ++i)
				{
					s_snesImageData[dstOffset] = transcodeBlockResult.TranscodedBlock[i];
					++dstOffset;
				}
			}
		}

		// After each image, pad with non-viable bytes
		dstOffset += 0x180;
	}

	m_importedSomethingValid = true;

	// Update the demo image
	{
		System::Drawing::Graphics^ ctx = System::Drawing::Graphics::FromImage(m_demoBitmap);
		ctx->Clear(System::Drawing::Color::Black);
		ctx->DrawImage(templateBitmap, 0, 0);
		m_panel1->Invalidate();
	}
}

MultiFormatPallette* nhl94e::Form2::GetImportedPallette()
{
	return &s_importedPallette;
}

std::vector<unsigned char>* nhl94e::Form2::GetImportedSnesImageData()
{
	return &s_snesImageData;
}

bool nhl94e::Form2::ImportedSomethingValid()
{
	return m_importedSomethingValid;
}

void nhl94e::Form2::m_okButton_Click(System::Object^ sender, System::EventArgs^ e) 
{
	this->DialogResult = System::Windows::Forms::DialogResult::OK;
	this->Close();
}