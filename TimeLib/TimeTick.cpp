#include "TimeTick.h"

libTools::CTimeTick::CTimeTick()
{
	Reset();
}

libTools::CTimeTick::CTimeTick(_In_ ULONGLONG ulTick) : _ulTick(ulTick)
{

}

ULONGLONG libTools::CTimeTick::_GetTickCount64()
{
#if _WIN32_WINNT >= 0x0600
	return ::GetTickCount64();
#else
	enum MY_SYSTEM_INFORMATION_CLASS
	{
		SystemBasicInformation = 0,
		SystemPerformanceInformation = 2,
		SystemTimeOfDayInformation = 3,
		SystemProcessInformation = 5,
		SystemProcessorPerformanceInformation = 8,
		SystemInterruptInformation = 23,
		SystemExceptionInformation = 33,
		SystemRegistryQuotaInformation = 37,
		SystemLookasideInformation = 45,
		SystemPolicyInformation = 134,
	};

	using NtQuerySystemInformationPtr = NTSTATUS(WINAPI *)(IN MY_SYSTEM_INFORMATION_CLASS SystemInformationClass, OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength OPTIONAL);
	static NtQuerySystemInformationPtr NtPtr = nullptr;
	if (NtPtr == nullptr)
	{
		NtPtr = reinterpret_cast<NtQuerySystemInformationPtr>(::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation"));
		if (NtPtr == nullptr)
		{
			::MessageBoxW(NULL, L"UnExist NtQuerySystemInformation!", L"Error", NULL);
			return 0;
		}

		struct MY_SYSTEM_TIME_OF_DAY_INFORMATION
		{
			LARGE_INTEGER BootTime;
			LARGE_INTEGER CurrentTime;
			LARGE_INTEGER TimeZoneBias;
			ULONG CurrentTimeZoneId;
		};


		MY_SYSTEM_TIME_OF_DAY_INFORMATION SysTime = { 0 };
		ULONG ulSysTimeRetLen = 0;

		if (NtPtr(SystemTimeOfDayInformation, &SysTime, sizeof(SysTime), &ulSysTimeRetLen) != 0)
		{
			::MessageBoxW(NULL, L"Invoke NtQuerySystemInformation Faild!", L"Error", NULL);
			return 0;
		}

		return (SysTime.CurrentTime.QuadPart - SysTime.BootTime.QuadPart) / 10000;
	}
	return 0;
#endif
}

VOID libTools::CTimeTick::Reset()
{
	_ulTick = _GetTickCount64();
}

DWORD libTools::CTimeTick::GetSpentTime(_In_ em_TimeTick emTimeTick) CONST
{
	return static_cast<DWORD>(GetSpentTime_By_X64(emTimeTick));
}

DWORD64 libTools::CTimeTick::GetSpentTime_By_X64(_In_ em_TimeTick emTimeTick) CONST
{
	switch (emTimeTick)
	{
	case CTimeTick::em_TimeTick::em_TimeTick_Hour:
		return (_GetTickCount64() - _ulTick) / 1000 / 60 / 60;
	case CTimeTick::em_TimeTick::em_TimeTick_Minute:
		return (_GetTickCount64() - _ulTick) / 1000 / 60;
	case CTimeTick::em_TimeTick::em_TimeTick_Second:
		return (_GetTickCount64() - _ulTick) / 1000;
	case CTimeTick::em_TimeTick::em_TimeTick_Millisecond:
		return (_GetTickCount64() - _ulTick);
	default:
		break;
	}
	return NULL;
}

