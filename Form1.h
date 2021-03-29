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
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Konstruktorcode hier hinzufügen.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^ menuStrip1;
	protected:
	private: System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::TabControl^ tabControl1;
	private: System::Windows::Forms::ToolStripMenuItem^ saveROMToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ openROMToolStripMenuItem;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::TextBox^ teamVenueTextBox;
	private: System::Windows::Forms::TextBox^ teamNameTextBox;
	private: System::Windows::Forms::TextBox^ locationTextBox;
	private: System::Windows::Forms::Label^ label5;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::TextBox^ acronymTextBox;

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
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openROMToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveROMToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->teamVenueTextBox = (gcnew System::Windows::Forms::TextBox());
			this->teamNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->locationTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->acronymTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->menuStrip1->SuspendLayout();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(615, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->openROMToolStripMenuItem,
					this->saveROMToolStripMenuItem, this->exitToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// openROMToolStripMenuItem
			// 
			this->openROMToolStripMenuItem->Name = L"openROMToolStripMenuItem";
			this->openROMToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->openROMToolStripMenuItem->Size = System::Drawing::Size(200, 22);
			this->openROMToolStripMenuItem->Text = L"Open ROM";
			this->openROMToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openROMToolStripMenuItem_Click);
			// 
			// saveROMToolStripMenuItem
			// 
			this->saveROMToolStripMenuItem->Name = L"saveROMToolStripMenuItem";
			this->saveROMToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift)
				| System::Windows::Forms::Keys::S));
			this->saveROMToolStripMenuItem->Size = System::Drawing::Size(200, 22);
			this->saveROMToolStripMenuItem->Text = L"Save ROM";
			this->saveROMToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveROMToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::F4));
			this->exitToolStripMenuItem->Size = System::Drawing::Size(200, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::WhiteSmoke;
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel1->Controls->Add(this->teamVenueTextBox);
			this->panel1->Controls->Add(this->teamNameTextBox);
			this->panel1->Controls->Add(this->locationTextBox);
			this->panel1->Controls->Add(this->label5);
			this->panel1->Controls->Add(this->label4);
			this->panel1->Controls->Add(this->label3);
			this->panel1->Controls->Add(this->acronymTextBox);
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Right;
			this->panel1->Location = System::Drawing::Point(409, 24);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(206, 522);
			this->panel1->TabIndex = 1;
			// 
			// teamVenueTextBox
			// 
			this->teamVenueTextBox->Location = System::Drawing::Point(73, 149);
			this->teamVenueTextBox->Name = L"teamVenueTextBox";
			this->teamVenueTextBox->Size = System::Drawing::Size(119, 20);
			this->teamVenueTextBox->TabIndex = 8;
			this->teamVenueTextBox->TextChanged += gcnew System::EventHandler(this, &Form1::teamVenueTextBox_TextChanged);
			// 
			// teamNameTextBox
			// 
			this->teamNameTextBox->Location = System::Drawing::Point(73, 115);
			this->teamNameTextBox->Name = L"teamNameTextBox";
			this->teamNameTextBox->Size = System::Drawing::Size(119, 20);
			this->teamNameTextBox->TabIndex = 7;
			this->teamNameTextBox->TextChanged += gcnew System::EventHandler(this, &Form1::teamNameTextBox_TextChanged);
			// 
			// locationTextBox
			// 
			this->locationTextBox->Location = System::Drawing::Point(73, 41);
			this->locationTextBox->Name = L"locationTextBox";
			this->locationTextBox->Size = System::Drawing::Size(119, 20);
			this->locationTextBox->TabIndex = 6;
			this->locationTextBox->TextChanged += gcnew System::EventHandler(this, &Form1::locationTextBox_TextChanged);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(15, 149);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(41, 13);
			this->label5->TabIndex = 5;
			this->label5->Text = L"Venue:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(15, 115);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(38, 13);
			this->label4->TabIndex = 4;
			this->label4->Text = L"Name:";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(15, 41);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(51, 13);
			this->label3->TabIndex = 3;
			this->label3->Text = L"Location:";
			// 
			// acronymTextBox
			// 
			this->acronymTextBox->Location = System::Drawing::Point(73, 77);
			this->acronymTextBox->Name = L"acronymTextBox";
			this->acronymTextBox->Size = System::Drawing::Size(119, 20);
			this->acronymTextBox->TabIndex = 2;
			this->acronymTextBox->TextChanged += gcnew System::EventHandler(this, &Form1::acronymTextBox_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(15, 80);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(51, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Acronym:";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(15, 10);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(89, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Team Information";
			// 
			// tabControl1
			// 
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(0, 24);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(409, 522);
			this->tabControl1->TabIndex = 2;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(615, 546);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->menuStrip1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"nhl94e";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: 
			System::Void exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);

	private: 
		System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e);
		void AddTeamGridUI(TeamData const& team);
		System::Void saveROMToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
		void OnCellValidating(System::Object^ sender, System::Windows::Forms::DataGridViewCellValidatingEventArgs^ e);
		void OnCellValueChanged(System::Object^ sender, System::Windows::Forms::DataGridViewCellEventArgs^ e);
		System::Void openROMToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
		void OpenROM(std::wstring romFilename);
		void OnSelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
	private: 
		System::Void locationTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
private: System::Void acronymTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
private: System::Void teamNameTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
private: System::Void teamVenueTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
};
}
