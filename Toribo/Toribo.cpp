// Toribo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <vector>

#include <Windows.h>
#include <stdlib.h>     
#include <time.h>

#include "Offsets.h"

using namespace std;

int randomJointSetting()
{
	return (rand() % 4) + 1;
}

void SendScanCodeKeyPress(WORD scanCode)
{
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
}

wstring s2ws(const std::string& s)
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

void log(string s)
{
	std::wstring stemp = s2ws(s);
	LPCWSTR result = stemp.c_str();

	OutputDebugString(result);
}

void go(HANDLE& handle)
{
	const int maxIterations = 100;
	const int movesPerSequence = 10;

	float bestScore = 0.f;
	vector<int> bestSequence;

	for (int j = 0; j < maxIterations; ++j)
	{
		SendScanCodeKeyPress(0x13); // r to reset
		SendScanCodeKeyPress(0x39); // space

		vector<int> sequence;

		for (int k = 0; k < movesPerSequence; ++k)
		{
			for (int i = 0; i < numJoints; ++i)
			{
				int jointState = randomJointSetting();
				sequence.push_back(jointState);

				DWORD pointerAddress = baseToriOffset + i * stepOffset;
				WriteProcessMemory(handle, (LPVOID)pointerAddress, &jointState, sizeof(jointState), NULL);
			}

			SendScanCodeKeyPress(0x39);

			Sleep(250);
		}

		float score = 0.f;
		ReadProcessMemory(handle, (LPVOID)toriScoreOffset, &score, sizeof(score), NULL);

		if (score > bestScore)
		{
			bestScore = score;
			bestSequence = sequence;
		}
	}

	log(to_string(bestScore) + "\r\n");

	for (const auto& i : bestSequence)
		log(to_string(i));
	log("\r\n");
}

int main()
{
	srand(time(NULL));

	int fps = 0;
	HWND hwnd = NULL;
	while (!hwnd && fps < 100)
	{
		++fps;

		string possibleWindowName = string("TORIBASH 5.33 fps:") + to_string(fps);
		hwnd = FindWindowA(NULL, possibleWindowName.c_str());
	}
	
	if (hwnd == NULL) {
		cout << "Cannot find toribash window" << endl;
		Sleep(3000);
		exit(-1);
	}
	else {
		DWORD procID;
		GetWindowThreadProcessId(hwnd, &procID);
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID); 

		if (procID == NULL) {
			cout << "process id not found" << endl;
			Sleep(3000);
			exit(-1);
		}
		else
		{
			// START
			SetForegroundWindow(hwnd);
			go(handle);
		}
	}

    return 0;
}


