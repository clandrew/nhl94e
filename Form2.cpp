#include "pch.h"
#include "Form2.h"

void nhl94e::Form2::OnLoad(System::Object^ sender, System::EventArgs^ e)
{
	demoBitmap = gcnew System::Drawing::Bitmap(L"0B_0.png");
	demoBitmap->SetResolution(96, 96);
}

void nhl94e::Form2::panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) 
{
	int scaling = 4;
	e->Graphics->InterpolationMode = System::Drawing::Drawing2D::InterpolationMode::NearestNeighbor;
	e->Graphics->Clear(System::Drawing::Color::Transparent);

	// I don't know why the +2 is necessary and at this point I can't be bothered to find out.
	e->Graphics->DrawImage(demoBitmap, 0, 0, 48 * scaling + 2, 48 * scaling + 2);
}
