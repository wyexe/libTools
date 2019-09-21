#include "KeyboardMsg.h"
#include <include/CharacterLib/Character.h>

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_x64_Debug.lib")
#else
#pragma comment(lib,"CharacterLib_x64.lib")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#endif // _DEBUG
#endif



BOOL libTools::CKeyboardMsg::SimulationKey(_In_ HWND hWnd, _In_ CHAR dwASCII, _In_opt_ DWORD dwTime /*= 10*/)
{
	SHORT tmp = VkKeyScanW(dwASCII);//…®√Ë–Èƒ‚¬Î
	WPARAM wParam = tmp & 0xFF;
	LPARAM lParam = 1;
	lParam += static_cast<UINT_PTR>((wParam, MAPVK_VK_TO_VSC) << 16);//ªÒ»°…®√Ë¬Î≤¢«“∏≥÷µLparm

	PostMessageW(hWnd, WM_KEYDOWN, wParam, lParam);
	Sleep(dwTime);
	lParam += 1 << 30;
	lParam += 1 << 31;
	PostMessageW(hWnd, WM_KEYUP, wParam, lParam);

	return TRUE;
}

DWORD libTools::CKeyboardMsg::GetVirKey(_In_ CHAR dwASCII)
{
	SHORT tmp = VkKeyScanW(dwASCII);//…®√Ë–Èƒ‚¬Î
	DWORD wParam = tmp & 0xFF;
	DWORD lParam = 1;
	lParam += MapVirtualKey(wParam, MAPVK_VK_TO_VSC) << 16;//ªÒ»°…®√Ë¬Î≤¢«“∏≥÷µLparm
	return lParam;
}

BOOL libTools::CKeyboardMsg::SimulationMouse(_In_ int x, _In_ int y)
{
	for (int i = 0; i < 1; ++i)
	{
		// …Ë÷√ Û±Í
		::SetCursorPos(x, y);
		mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, NULL, NULL);
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, NULL, NULL);
	}
	return TRUE;
}

BOOL libTools::CKeyboardMsg::SimulationRightMouse(_In_ int x, _In_ int y)
{
	for (int i = 0; i < 1; ++i)
	{
		// …Ë÷√ Û±Í
		::SetCursorPos(x, y);
		mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, NULL, NULL);
		mouse_event(MOUSEEVENTF_RIGHTUP, x, y, NULL, NULL);
	}
	return TRUE;
}

BOOL libTools::CKeyboardMsg::SendAscii(_In_ WCHAR data, _In_ BOOL shift)
{
	INPUT input[2];
	memset(input, 0, 2 * sizeof(INPUT));

	if (shift)
	{
		input[0].type = INPUT_KEYBOARD;
		input[0].ki.wVk = VK_SHIFT;
		SendInput(1, input, sizeof(INPUT));
	}
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = data;
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = data;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(2, input, sizeof(INPUT));
	if (shift)
	{
		input[0].type = INPUT_KEYBOARD;
		input[0].ki.wVk = VK_SHIFT;
		input[0].ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, input, sizeof(INPUT));
	}
	return TRUE;
}

BOOL libTools::CKeyboardMsg::SendUnicode(_In_ WCHAR data)
{
	INPUT input[2];
	memset(input, 0, 2 * sizeof(INPUT));

	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = 0;
	input[0].ki.wScan = data;
	input[0].ki.dwFlags = 0x4;//KEYEVENTF_UNICODE;

	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = 0;
	input[1].ki.wScan = data;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP | 0x4;//KEYEVENTF_UNICODE;

	SendInput(2, input, sizeof(INPUT));
	return TRUE;
}

BOOL libTools::CKeyboardMsg::SendKey(_In_ WORD wVk)
{
	INPUT input[2];
	memset(input, 0, 2 * sizeof(INPUT));

	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = wVk;

	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = wVk;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, input, sizeof(INPUT));
	return TRUE;
}


BOOL libTools::CKeyboardMsg::SendKey(_In_ WORD wVk, _In_ BOOL bPush)
{
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = wVk;
	input.ki.dwFlags = bPush ? 0 : KEYEVENTF_KEYUP;


	SendInput(1, &input, sizeof(INPUT));
	return TRUE;
}

BOOL libTools::CKeyboardMsg::SendKeys(_In_ LPCWSTR data)
{
	SHORT vk	= 0;
	BOOL shift	= FALSE;
	size_t len	= wcslen(data);
	for (size_t i = 0; i < len; i++)
	{
		if (data[i] >= 0 && data[i] < 256) //ascii◊÷∑˚
		{
			vk = VkKeyScanW(data[i]);
			if (vk == -1)
			{
				SendUnicode(data[i]);
			}
			else
			{
				if (vk < 0)
				{
					vk = ~vk + 0x1;
				}

				shift = vk >> 8 & 0x1;

				if (GetKeyState(VK_CAPITAL) & 0x1)
				{
					if (data[i] >= 'a' && data[i] <= 'z' || data[i] >= 'A' && data[i] <= 'Z')
					{
						shift = !shift;
					}
				}
				SendAscii(vk & 0xFF, shift);
			}
		}
		else //unicode◊÷∑˚
		{
			SendUnicode(data[i]);
		}
	}
	return TRUE;
}

BOOL libTools::CKeyboardMsg::SendKeys(_In_ LPCSTR data)
{
	return SendKeys(CCharacter::ASCIIToUnicode(data).c_str());
}
