#ifndef __LIBTOOLS_TIMELIB_TIMECHARACTER_H__
#define __LIBTOOLS_TIMELIB_TIMECHARACTER_H__

#include "TimeTick.h"
#include <string>

namespace libTools
{
	class CTimeCharacter
	{
	public:
		struct TimeContent
		{
			SYSTEMTIME SysTime;
			TimeContent()
			{
				ZeroMemory(&SysTime, sizeof(SysTime));
			}
			TimeContent(_In_ CONST SYSTEMTIME& SysTime_)
			{
				SysTime = SysTime_;
			}
			TimeContent(_In_ SYSTEMTIME&& SysTime_)
			{
				SysTime = SysTime_;
			}

			bool operator == (_In_ CONST TimeContent& Content) CONST;

			bool operator < (_In_ CONST TimeContent& Content) CONST;

			bool operator <= (_In_ CONST TimeContent& Content) CONST;

			bool operator > (_In_ CONST TimeContent& Content) CONST;

			bool operator >= (_In_ CONST TimeContent& Content) CONST;

			bool operator != (_In_ CONST TimeContent& Content) CONST;
		};

		enum class em_FormatType
		{
			Chinese,
			English
		};
	public:
		CTimeCharacter() = default;
		~CTimeCharacter() = default;

		// 5 = DateDiff("2017.1.1 1:1:1","2017.1.1 6:1:1", em_Hour) 
		static LONGLONG DateDiff(_In_ CONST TimeContent& Time1, _In_ CONST TimeContent& Time2, CTimeTick::em_TimeTick emTimeTick);

		// GetTimeContent_By_Text(L"2017.1.1 1:1:1" | L"2017��1��1�� 6ʱ3��4��" , Return)
		static BOOL GetTimeContent_By_Text(_In_ CONST std::wstring& wsText, _Out_opt_ TimeContent& TimeInfo);

		// 
		static LONGLONG GetTimeValue(_In_ CONST SYSTEMTIME& SysTime);
		
		//
		static VOID FormatTime(_In_ ULONGLONG ulResult, _Out_opt_ std::wstring& wsText);

		//
		static VOID FormatTime(_In_ em_FormatType emType, _Out_ std::wstring& wsText);

		//
		static TimeContent GetCurrentTick();
	private:

	};
}

#endif // !__LIBTOOLS_TIMELIB_TIMECHARACTER_H__
