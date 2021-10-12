#pragma once
#include "TeamData.h"
#include "CustomUI.h"

namespace nhl94e
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Zusammenfassung für Form1
	/// </summary>
	public ref class Form2 : public System::Windows::Forms::Form
	{
	public:
		Form2(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~Form2()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Drawing::Bitmap^ demoBitmap2;
	private: System::Windows::Forms::Button^ saveTemplateBtn;
	private: ProfileImageData* profileImageData;
	private: ProfilePalletteData* profilePalletteData;
	private: int imageIndex;
	protected:

	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->saveTemplateBtn = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->Location = System::Drawing::Point(13, 13);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(192, 192);
			this->panel1->TabIndex = 0;
			this->panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form2::panel1_Paint);
			// 
			// saveTemplateBtn
			// 
			this->saveTemplateBtn->Location = System::Drawing::Point(13, 230);
			this->saveTemplateBtn->Name = L"saveTemplateBtn";
			this->saveTemplateBtn->Size = System::Drawing::Size(193, 39);
			this->saveTemplateBtn->TabIndex = 1;
			this->saveTemplateBtn->Text = L"Save Template";
			this->saveTemplateBtn->UseVisualStyleBackColor = true;
			this->saveTemplateBtn->Click += gcnew System::EventHandler(this, &Form2::saveTemplateBtn_Click);
			// 
			// Form2
			// 
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(218, 383);
			this->Controls->Add(this->saveTemplateBtn);
			this->Controls->Add(this->panel1);
			this->Name = L"Form2";
			this->Load += gcnew System::EventHandler(this, &Form2::OnLoad);
			this->ResumeLayout(false);

		}
#pragma endregion

	private: 
	private: System::Void panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e);
		   void OnLoad(System::Object^ sender, System::EventArgs^ e);
	private: System::Void saveTemplateBtn_Click(System::Object^ sender, System::EventArgs^ e);
	public: void SetProfileData(ProfileImageData* img, ProfilePalletteData* pal, int imgIndex);
	};
}
