#pragma once
#include "TeamData.h"

class ProfileImageImporter
{
	std::vector<unsigned char> m_snesImageData;
	MultiFormatPallette m_importedPallette;
	bool m_importedSomethingValid;
	std::string m_errorMessage;

public:
	ProfileImageImporter();

	System::Drawing::Bitmap^ Import(wchar_t const* fileName);


	std::vector<unsigned char>* GetImportedSnesImageData();

	bool ImportedSomethingValid();

	std::string const& GetErrorMessage();

	MultiFormatPallette* GetImportedPallette();
};