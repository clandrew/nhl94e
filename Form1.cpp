#include "pch.h"
#include "Form1.h"


System::Void CppCLRWinformsProjekt::Form1::exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	this->Close();
}

System::Void CppCLRWinformsProjekt::Form1::Form1_Load(System::Object^ sender, System::EventArgs^ e)
{
	this->dataGridView1->Rows->Clear();
	this->dataGridView1->Rows->Insert(0, gcnew cli::array<System::String^>(1) { "Asdf" });
	this->dataGridView1->Rows->Insert(0, gcnew cli::array<System::String^>(1) { "Jkl" });
}