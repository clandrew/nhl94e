#pragma once

// Putting this in Form1.h keeps screwing up the Windows Forms designer :(
// So here it's in its own file.
ref class DoubleBufferedDataGridView : public System::Windows::Forms::DataGridView
{
public:
    DoubleBufferedDataGridView() { DoubleBuffered = true; }
};