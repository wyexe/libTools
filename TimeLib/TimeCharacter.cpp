#include "TimeCharacter.h"
#include <vector>
#include <time.h> // mktime()
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



bool libTools::CTimeCharacter::TimeContent::operator==(_In_ CONST TimeContent& Content) CONST
{
	return GetTimeValue(SysTime) == GetTimeValue(Content.SysTime);
}

bool libTools::CTimeCharacter::TimeContent::operator<(_In_ CONST TimeContent& Content) CONST
{
	return GetTimeValue(SysTime) < GetTimeValue(Content.SysTime);
}

bool libTools::CTimeCharacter::TimeContent::operator<=(_In_ CONST TimeContent& Content) CONST
{
	return GetTimeValue(SysTime) <= GetTimeValue(Content.SysTime);
}

bool libTools::CTimeCharacter::TimeContent::operator>(_In_ CONST TimeContent& Content) CONST
{
	return GetTimeValue(SysTime) > GetTimeValue(Content.SysTime);
}

bool libTools::CTimeCharacter::TimeContent::operator>=(_In_ CONST TimeContent& Content) CONST
{
	return GetTimeValue(SysTime) >= GetTimeValue(Content.SysTime);
}

bool libTools::CTimeCharacter::TimeContent::operator!=(_In_ CONST TimeContent& Content) CONST
{
	return GetTimeValue(SysTime) != GetTimeValue(Content.SysTime);
}

LONGLONG libTools::CTimeCharacter::DateDiff(_In_ CONST TimeContent& Time1, _In_ CONST TimeContent& Time2, CTimeTick::em_TimeTick emTimeTick)
{
	LONGLONG TimeValue = abs(GetTimeValue(Time1.SysTime) - GetTimeValue(Time2.SysTime));

	switch (emTimeTick)
	{
	case CTimeTick::em_TimeTick::em_TimeTick_Hour:
		return TimeValue / 3600;
	case CTimeTick::em_TimeTick::em_TimeTick_Minute:
		return TimeValue / 60;
	case CTimeTick::em_TimeTick::em_TimeTick_Second:
		return TimeValue;
	default:
		::MessageBoxW(NULL, L"UnSupport emTimeTick Other Type!", L"Error", NULL);
		break;
	}
	return 0;
}

BOOL libTools::CTimeCharacter::GetTimeContent_By_Text(_In_ CONST std::wstring& wsText, _Out_opt_ TimeContent& TimeInfo)
{
	//GetTimeContent_By_Text(L"2017-1-1 1:1:1" | L"2017年1月1日 6时3分4秒", Return)
	auto ConvertTextPtrToWord = [](CONST std::wstring& wsParam)
	{
		return static_cast<WORD>(std::stoi(wsParam));
	};


	std::vector<std::wstring> Vec;
	if (wsText.find(L"-") != -1)
	{
		if (!CCharacter::SplitFormatText(wsText, L"*-*-* *:*:*", Vec) || Vec.size() != 6)
		{
			return FALSE;
		}
	}
	else if (wsText.find(L"年") != -1)
	{
		if (!CCharacter::SplitFormatText(wsText, L"*年*月*日 *时*分*秒", Vec) || Vec.size() != 6)
		{
			return FALSE;
		}
	}
	else
	{
		// Invalid Format
		return FALSE;
	}


	TimeInfo.SysTime.wYear = static_cast<WORD>(std::stoi(Vec.at(0)));
	TimeInfo.SysTime.wMonth = static_cast<WORD>(std::stoi(Vec.at(1)));
	TimeInfo.SysTime.wDay = static_cast<WORD>(std::stoi(Vec.at(2)));
	TimeInfo.SysTime.wHour = static_cast<WORD>(std::stoi(Vec.at(3)));
	TimeInfo.SysTime.wMinute = static_cast<WORD>(std::stoi(Vec.at(4)));
	TimeInfo.SysTime.wSecond = static_cast<WORD>(std::stoi(Vec.at(5)));
	return TRUE;
}

LONGLONG libTools::CTimeCharacter::GetTimeValue(_In_ CONST SYSTEMTIME& SysTime)
{
	struct tm t = { SysTime.wSecond, SysTime.wMinute, SysTime.wHour, SysTime.wDay, SysTime.wMonth - 1, SysTime.wYear - 1900, SysTime.wDayOfWeek, 0 ,0 };
	return mktime(&t);
}

VOID libTools::CTimeCharacter::FormatTime(_In_ ULONGLONG ulResult, _Out_opt_ std::wstring& wsText)
{
	ULONGLONG ulDay = 0;
	ULONGLONG ulHour = 0;
	ULONGLONG ulMin = 0;
	ULONGLONG ulSecond = 0;

	//convert MilliSecond to Second
	ulResult /= 1000;

	if (ulResult >= 60 * 60 * 24)//Separate Day
	{
		ulDay = ulResult / (60 * 60 * 24);
		ulResult -= ulDay * (60 * 60 * 24);
	}
	if (ulResult >= 60 * 60)//Separate Hour
	{
		ulHour = ulResult / (60 * 60);
		ulResult -= ulHour * (60 * 60);
	}
	if (ulResult >= 60)//Separate Minunte
	{
		ulMin = ulResult / 60;
		ulResult -= ulMin * 60;
	}

	//Get Last Second
	ulSecond = ulResult;
	wsText = CCharacter::MakeFormatText(L"%I64dDay %I64dHour %I64dMin %I64dSec", ulDay, ulHour, ulMin, ulSecond);
}


libTools::CTimeCharacter::TimeContent libTools::CTimeCharacter::GetCurrentTick()
{
	SYSTEMTIME SysTime = { 0 };
	::GetLocalTime(&SysTime);
	return TimeContent(SysTime);
}

VOID libTools::CTimeCharacter::FormatTime(_In_ em_FormatType emType, _Out_ std::wstring& wsText)
{
	SYSTEMTIME SysTime = { 0 };
	::GetLocalTime(&SysTime);

	switch (emType)
	{
	case libTools::CTimeCharacter::em_FormatType::Chinese:
		wsText = CCharacter::MakeFormatText(L"%d年%d月%d日 %d:%d:%d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		break;
	case libTools::CTimeCharacter::em_FormatType::English:
		wsText = CCharacter::MakeFormatText(L"%d-%d-%d %d:%d:%d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		break;
	default:
		break;
	}
}
