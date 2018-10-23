#pragma once

#include "stdafx.h"

#include <vector>
#include <Windows.h>

class ToriController
{
public:
	ToriController();

	bool ConnectToToribashWindow();
	bool ConnectToToribashHandle();
	
	void WriteFloat(int address, float newValue);
	void ReadFloat(int address, float &outValue);

	void WriteInt(int address, int newValue);
	void ReadInt(int address, int &outValue);

	bool SendScanCodeKeyPress(WORD scanCode);
	bool ResetToribash();

	float PlaySequence(std::vector<int>& sequence);

	void ProgressXFrames(int x) {};

private:
	HWND m_toribashWindow;
	HANDLE m_toribashProcessHandle;
};

