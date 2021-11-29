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
	private: System::Windows::Forms::Panel^ m_panel1;
	private: System::Drawing::Bitmap^ m_demoBitmap;
	private: System::Windows::Forms::Button^ m_saveTemplateBtn;
	private: ProfileImageData* m_profileImageData;
	private: ProfilePalletteData* m_profilePalletteData;
	private: System::Windows::Forms::Button^ m_importButton;
	private: System::Windows::Forms::Button^ m_okButton;

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
			this->m_panel1 = (gcnew System::Windows::Forms::Panel());
			this->m_saveTemplateBtn = (gcnew System::Windows::Forms::Button());
			this->m_importButton = (gcnew System::Windows::Forms::Button());
			this->m_okButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// m_panel1
			// 
			this->m_panel1->Location = System::Drawing::Point(13, 13);
			this->m_panel1->Name = L"m_panel1";
			this->m_panel1->Size = System::Drawing::Size(1152, 192);
			this->m_panel1->TabIndex = 0;
			this->m_panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form2::panel1_Paint);
			// 
			// m_saveTemplateBtn
			// 
			this->m_saveTemplateBtn->Location = System::Drawing::Point(13, 221);
			this->m_saveTemplateBtn->Name = L"m_saveTemplateBtn";
			this->m_saveTemplateBtn->Size = System::Drawing::Size(193, 39);
			this->m_saveTemplateBtn->TabIndex = 1;
			this->m_saveTemplateBtn->Text = L"Save Template";
			this->m_saveTemplateBtn->UseVisualStyleBackColor = true;
			this->m_saveTemplateBtn->Click += gcnew System::EventHandler(this, &Form2::saveTemplateBtn_Click);
			// 
			// m_importButton
			// 
			this->m_importButton->Location = System::Drawing::Point(212, 221);
			this->m_importButton->Name = L"m_importButton";
			this->m_importButton->Size = System::Drawing::Size(193, 39);
			this->m_importButton->TabIndex = 2;
			this->m_importButton->Text = L"Import From Template";
			this->m_importButton->UseVisualStyleBackColor = true;
			this->m_importButton->Click += gcnew System::EventHandler(this, &Form2::importButton_Click);
			// 
			// m_okButton
			// 
			this->m_okButton->Location = System::Drawing::Point(972, 221);
			this->m_okButton->Name = L"m_okButton";
			this->m_okButton->Size = System::Drawing::Size(193, 39);
			this->m_okButton->TabIndex = 3;
			this->m_okButton->Text = L"OK";
			this->m_okButton->UseVisualStyleBackColor = true;
			this->m_okButton->Click += gcnew System::EventHandler(this, &Form2::m_okButton_Click);
			// 
			// Form2
			// 
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(1180, 276);
			this->Controls->Add(this->m_okButton);
			this->Controls->Add(this->m_importButton);
			this->Controls->Add(this->m_saveTemplateBtn);
			this->Controls->Add(this->m_panel1);
			this->Name = L"Form2";
			this->Load += gcnew System::EventHandler(this, &Form2::OnLoad);
			this->ResumeLayout(false);

		}
#pragma endregion

	private: 
	private: System::Void panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e);
		   void OnLoad(System::Object^ sender, System::EventArgs^ e);
	private: System::Void saveTemplateBtn_Click(System::Object^ sender, System::EventArgs^ e);
	public: void SetProfileData(ProfileImageData* img, ProfilePalletteData* pal);
	private: System::Void importButton_Click(System::Object^ sender, System::EventArgs^ e);
	public: MultiFormatPallette* GetImportedPallette();
	public: std::vector<unsigned char>* GetImportedSnesImageData();
	public: bool ImportedSomethingValid();
	private: System::Void m_okButton_Click(System::Object^ sender, System::EventArgs^ e);
};
}
