#include "pch.h"
#include "ProfileImageImporter.h"
#include "Utils.h"

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
	int InvalidColorRgb;
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

			int rgb = rgbBlock[rgby * 8 + rgbx];

			RgbToIndexedResult rgbToIndexedResult = RgbToIndexed(rgb, importedPallette);
			if (!rgbToIndexedResult.ValidColor)
			{
				result.ValidColors = false;
				result.InvalidColorX = 8 - xi - 1;
				result.InvalidColorY = yi;
				result.InvalidColorRgb = rgb;
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

ProfileImageImporter::ProfileImageImporter()
	: m_importedSomethingValid(false)
{}

System::Drawing::Bitmap^ ProfileImageImporter::Import(wchar_t const* fileName)
{
	m_importedSomethingValid = false;

	System::Drawing::Bitmap^ templateBitmap = gcnew System::Drawing::Bitmap(gcnew System::String(fileName));

	// Verify expected sizes
	if (templateBitmap->Size.Width != 48 * 6)
	{
		m_errorMessage = "The template image was an unexpected width; 288 was expected.";
		m_importedSomethingValid = false;
		return nullptr;
	}

	if (templateBitmap->Size.Height != 48 + 2)
	{
		m_errorMessage = "The template image was an unexpected height; 50 was expected.";
		m_importedSomethingValid = false;
		return nullptr;
	}

	for (int i = 0; i < 16; ++i)
	{
		System::Drawing::Color pixelColor = templateBitmap->GetPixel(i, 48 - 1 + 2);
		int argb = pixelColor.ToArgb();
		m_importedPallette.PortableR8G8B8[i] = argb;
		m_importedPallette.SnesB5G5R5[i] = R8B8G8ToSnesB5G5R5(m_importedPallette.PortableR8G8B8[i]);
	}

	// Commit image data back into rom data, validating as we go
	int dstOffset = 0;
	m_snesImageData.clear();
	m_snesImageData.resize(0x2400);
	std::fill(m_snesImageData.begin(), m_snesImageData.end(), 0);

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

				TranscodeBlockResult transcodeBlockResult = TranscodeBlock(block, &m_importedPallette);

				if (!transcodeBlockResult.ValidColors)
				{
					int invalidColorX = x + transcodeBlockResult.InvalidColorX;
					int invalidColorY = y + transcodeBlockResult.InvalidColorY;
					int invalidColorRgb = transcodeBlockResult.InvalidColorRgb;

					std::stringstream strm;
					strm << "The image contained a color, 0x" << std::hex << invalidColorRgb << std::dec << " at X=" << invalidColorX << ", Y=" << invalidColorY << " which was not present in the pallette.";
					m_errorMessage = strm.str();
					m_importedSomethingValid = false;
					return nullptr;
				}

				for (size_t i = 0; i < transcodeBlockResult.TranscodedBlock.size(); ++i)
				{
					m_snesImageData[dstOffset] = transcodeBlockResult.TranscodedBlock[i];
					++dstOffset;
				}
			}
		}

		// After each image, pad with non-viable bytes
		dstOffset += 0x180;
	}

	m_importedSomethingValid = true;
	return templateBitmap;
}


std::vector<unsigned char>* ProfileImageImporter::GetImportedSnesImageData()
{
	return &m_snesImageData;
}

bool ProfileImageImporter::ImportedSomethingValid()
{
	return m_importedSomethingValid;
}

std::string const& ProfileImageImporter::GetErrorMessage()
{
	return m_errorMessage;
}

MultiFormatPallette* ProfileImageImporter::GetImportedPallette()
{
	return &m_importedPallette;
}