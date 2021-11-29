#include "pch.h"
#include "Form2.h"
#include "ProfileImageImporter.h"
#include "Utils.h"

MultiFormatPallette s_default{};

void nhl94e::Form2::OnLoad(System::Object^ sender, System::EventArgs^ e)
{
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

ProfileImageImporter s_profileImageImporter;

void nhl94e::Form2::importButton_Click(System::Object^ sender, System::EventArgs^ e) 
{
	OpenFileDialog^ dialog = gcnew OpenFileDialog();
#if _DEBUG
	dialog->FileName = L"D:\\repos\\nhl94e\\template.png";
#endif

	System::Windows::Forms::DialogResult result = dialog->ShowDialog();
	if (result != System::Windows::Forms::DialogResult::OK)
		return;

	std::wstring fileName = ManagedToWideString(dialog->FileName);

	System::Drawing::Bitmap^ templateBitmap = s_profileImageImporter.Import(fileName.c_str());

	if (!s_profileImageImporter.ImportedSomethingValid())
	{
		System::Windows::Forms::MessageBox::Show(NarrowASCIIStringToManaged(s_profileImageImporter.GetErrorMessage()));
		return;
	}

	// Update the demo image
	{
		System::Drawing::Graphics^ ctx = System::Drawing::Graphics::FromImage(m_demoBitmap);
		ctx->Clear(System::Drawing::Color::Black);
		ctx->DrawImage(templateBitmap, 0, 0);
		m_panel1->Invalidate();
	}

	templateBitmap->~Bitmap(); // no idea why I have to do this but apparently I do
}

MultiFormatPallette* nhl94e::Form2::GetImportedPallette()
{
	return s_profileImageImporter.GetImportedPallette();
}

std::vector<unsigned char>* nhl94e::Form2::GetImportedSnesImageData()
{
	return s_profileImageImporter.GetImportedSnesImageData();
}

bool nhl94e::Form2::ImportedSomethingValid()
{
	return s_profileImageImporter.ImportedSomethingValid();
}

void nhl94e::Form2::m_okButton_Click(System::Object^ sender, System::EventArgs^ e) 
{
	this->DialogResult = System::Windows::Forms::DialogResult::OK;
	this->Close();
}