#include "stdafx.h"
#include "ToriController.h"

#include "Offsets.h"

#include <iostream>
#include <string>

using namespace std;

std::wstring s2ws(const std::string& s)	
{
	// convert string to LPCWSTR for logging
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void log(std::string s)
{
	std::wstring stemp = s2ws(s);
	LPCWSTR result = stemp.c_str();
	OutputDebugString(result);
}

ToriController::ToriController()
{
}

bool ToriController::ConnectToToribashWindow()
{
	int fps = 0;
	HWND hwnd = NULL;
	while (!hwnd && fps < 100)
	{
		++fps;

		string possibleWindowName = string("TORIBASH 5.33 fps:") + to_string(fps);
		hwnd = FindWindowA(NULL, possibleWindowName.c_str());
	}
	
	if (hwnd == NULL) {
		log("Cannot find toribash window");
		return false;
	}
	else
	{
		m_toribashWindow = hwnd;
	}

	return true;
}

bool ToriController::ConnectToToribashHandle()
{
	DWORD procID;
	GetWindowThreadProcessId(m_toribashWindow, &procID);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID); 

	if (procID == NULL) {
		log("Cannot connect to toribash process");
		return false;
	}
	else
	{
		m_toribashProcessHandle = handle;
	}

	return true;
}

void ToriController::WriteFloat(int address, float newValue)
{
	WriteProcessMemory(m_toribashProcessHandle, (LPVOID)address, &newValue, sizeof(newValue), NULL);
}

void ToriController::ReadFloat(int address, float &outValue)
{
	ReadProcessMemory(m_toribashProcessHandle, (LPVOID)address, &outValue, sizeof(outValue), NULL);
}

void ToriController::WriteInt(int address, int newValue)
{
	WriteProcessMemory(m_toribashProcessHandle, (LPVOID)address, &newValue, sizeof(newValue), NULL);
}

void ToriController::ReadInt(int address, int &outValue)
{
	ReadProcessMemory(m_toribashProcessHandle, (LPVOID)address, &outValue, sizeof(outValue), NULL);
}

bool ToriController::SendScanCodeKeyPress(WORD scanCode)
{
	if (!m_toribashWindow) return false;
	SetForegroundWindow(m_toribashWindow);

	INPUT ip;

	//Set up the INPUT structure
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.wVk = 0; //We're doing scan codes instead
	ip.ki.dwExtraInfo = 0;

	//This let's you do a hardware scan instead of a virtual keypress
	ip.ki.dwFlags = KEYEVENTF_SCANCODE;
	ip.ki.wScan = scanCode;

	//Send the press
	SendInput(1, &ip, sizeof(INPUT));

	//Prepare a keyup event
	ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	return true;
}

bool ToriController::ResetToribash()
{
	return SendScanCodeKeyPress(scancode_r) && SendScanCodeKeyPress(scancode_space);
}

float ToriController::PlaySequence(std::vector<int>& sequence)
{
	const int movesInSequence = (int)(sequence.size() / numJoints);

	for (int m = 0; m < movesInSequence; ++m)
	{
		for (int j = 0; j < numJoints; ++j)
		{	
			const int jointState = sequence[m * numJoints + j];
			DWORD pointerAddress = baseToriOffset + j * stepOffset;
			WriteInt(pointerAddress, jointState);
		}
		
		ProgressXFrames(10);
	}

	float score = 0.f;
	ReadFloat(toriScoreOffset, score);

	return score;
}
