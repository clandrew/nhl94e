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
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->menuStrip1->SuspendLayout();
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
			this->panel1->BackColor = System::Drawing::Color::Transparent;
			this->panel1->Dock = System::Windows::Forms::DockStyle::Right;
			this->panel1->Location = System::Drawing::Point(409, 24);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(206, 522);
			this->panel1->TabIndex = 1;
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
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"icon.ico")));
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"nhl94e";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
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
};
}
