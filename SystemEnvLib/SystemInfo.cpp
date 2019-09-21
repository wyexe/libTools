#include "SystemInfo.h"
#include <Windows.h>


CSystemInfo::em_System_Version CSystemInfo::GetSystemVersion()
{
	HMODULE hmDLL = ::GetModuleHandleW(L"ntdll.dll");
	if (hmDLL == NULL)
		return em_System_Version::Unknow;


	// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/wdm/ns-wdm-_osversioninfoexw
	using RtlGetVersionPtr = NTSTATUS(WINAPI *)(RTL_OSVERSIONINFOEXW *);
	RtlGetVersionPtr GetVersionPtr = (RtlGetVersionPtr)::GetProcAddress(hmDLL, "RtlGetVersion");
	if (GetVersionPtr == nullptr)
		return em_System_Version::Unknow;


	RTL_OSVERSIONINFOEXW Info = { 0 };
	Info.dwOSVersionInfoSize = sizeof(Info);
	if (GetVersionPtr(&Info) != 0)
		return em_System_Version::Unknow;


	if (Info.dwMajorVersion == 10 && Info.dwMinorVersion == 0)
		return em_System_Version::Windows10;
	else if (Info.dwMajorVersion == 6 && Info.dwMinorVersion == 2)
		return Info.wProductType == VER_NT_WORKSTATION ? em_System_Version::Windows8 : em_System_Version::WindowsServer2012;
	else if (Info.dwMajorVersion == 6 && Info.dwMinorVersion == 1)
		return Info.wProductType == VER_NT_WORKSTATION ? em_System_Version::Windows7 : em_System_Version::WindowServer2008R2;
	else if (Info.dwMajorVersion == 6 && Info.dwMinorVersion == 0)
		return Info.wProductType == VER_NT_WORKSTATION ? em_System_Version::WindowsVista : em_System_Version::WindowsServer2008;
	else if (Info.dwMajorVersion == 5 && Info.dwMinorVersion == 2)
		return em_System_Version::WindowsServer2003;
	else if (Info.dwMajorVersion == 5 && Info.dwMinorVersion == 1)
		return em_System_Version::WindowsXp;


	return em_System_Version::Unknow;
}
