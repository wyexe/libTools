#include "Process.h"
#include <tlhelp32.h>
#include <algorithm> // std::find_if
#include "psapi.h"  // QueryWorkingSet
#include <memory>   // std::shared_ptr
#include <Shlwapi.h> // PathRemoveFileSpecW
#include <include/CharacterLib/Character.h>

#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Shlwapi.lib")


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



DWORD libTools::CProcess::FindPidByProcName(_In_ CONST std::wstring& wsProcName)
{
	std::vector<ProcessContent> Vec;
	if (!GetVecProcess(Vec) || Vec.size() == 0)
	{
		return 0xFFFFFFFF;
	}
	

	std::wstring wsLowerProcName = libTools::CCharacter::MakeTextToLower(std::wstring(wsProcName));
	auto itr = std::find_if(Vec.begin(), Vec.end(), [wsLowerProcName](CONST ProcessContent& itm) { return itm.wsClientName == wsLowerProcName; });
	return itr == Vec.end() ? 0xFFFFFFFF : itr->dwPid;
}

BOOL libTools::CProcess::IsExistPid(_In_ DWORD dwPid)
{
	std::vector<ProcessContent> Vec;
	if (!GetVecProcess(Vec) || Vec.size() == 0)
	{
		return FALSE;
	}


	auto itr = std::find_if(Vec.begin(), Vec.end(), [dwPid](CONST ProcessContent& itm) { return itm.dwPid == dwPid; });
	return itr != Vec.end() ? TRUE : FALSE;
}

BOOL libTools::CProcess::IsExistProcName(_In_ CONST std::wstring& wsProcName)
{
	return FindPidByProcName(wsProcName) != 0xFFFFFFFF;
}

VOID libTools::CProcess::TerminateProcByProcName(_In_ CONST std::wstring& wsProcName)
{
	std::vector<ProcessContent> Vec;
	GetVecProcess(Vec);

	std::wstring wsLowerProcName = libTools::CCharacter::MakeTextToLower(std::wstring(wsProcName));
	for (CONST auto& itm : Vec)
	{
		if (itm.wsClientName != wsLowerProcName)
		{
			continue;
		}

		TerminateProcByPid(itm.dwPid);
	}
}

VOID libTools::CProcess::TerminateProcByPid(_In_ DWORD dwPid)
{
	HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);
	if (hProcess == NULL)
	{
		return;
	}


	::TerminateProcess(hProcess, 0);
	::CloseHandle(hProcess);
}

BOOL libTools::CProcess::GetVecProcess(_Out_ std::vector<ProcessContent>& VecProc)
{
	HANDLE hThSnap32 = NULL;
	PROCESSENTRY32W pe32;

	hThSnap32 = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hThSnap32 == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32FirstW(hThSnap32, &pe32))
	{
		::CloseHandle(hThSnap32);
		return FALSE;
	}

	
	do
	{
		ProcessContent PC;
		PC.dwPid = pe32.th32ProcessID;
		PC.wsClientName = libTools::CCharacter::MakeTextToLower(std::wstring(pe32.szExeFile));
		VecProc.push_back(PC);
	} while (Process32NextW(hThSnap32, &pe32));
	::CloseHandle(hThSnap32);
	return TRUE;
}

int libTools::CProcess::GetCpuUsageByPid(_In_ DWORD dwPid, _In_ _Out_ LONGLONG& llLastTime, _In_ _Out_ LONGLONG& llLastSysTime)
{
	static int nCpuKernelCount = -1;
	if (nCpuKernelCount == -1)
	{
		SYSTEM_INFO SysInfo;
		::GetSystemInfo(&SysInfo);
		nCpuKernelCount = static_cast<int>(SysInfo.dwNumberOfProcessors);
	}


	auto file_time_2_utc = [](const FILETIME* ftime)
	{
		LARGE_INTEGER li;

		li.LowPart = ftime->dwLowDateTime;
		li.HighPart = ftime->dwHighDateTime;
		return li.QuadPart;
	};

	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	int64_t system_time;
	int64_t time;
	int64_t system_time_delta;
	int64_t time_delta;

	int cpu = -1;

	GetSystemTimeAsFileTime(&now);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL)
		return -1;

	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
		return -1;

	system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / nCpuKernelCount;
	time = file_time_2_utc(&now);

	if ((llLastTime == 0) || (llLastSysTime == 0))
	{
		llLastSysTime = system_time;
		llLastTime = time;
		return -1;
	}

	system_time_delta = system_time - llLastSysTime;
	time_delta = time - llLastTime;

	if (time_delta == 0)
		return -1;

	cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
	llLastSysTime = system_time;
	llLastTime = time;
	return cpu;
}

ULONGLONG libTools::CProcess::CalcWorkSetPrivate(_In_ DWORD dwPid)
{
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL)
	{
		return 0;
	}


	BOOL bRet = TRUE;
	PSAPI_WORKING_SET_INFORMATION workSetInfo;
	PBYTE pByte = NULL;
	PSAPI_WORKING_SET_BLOCK *pWorkSetBlock = workSetInfo.WorkingSetInfo;
	memset(&workSetInfo, 0, sizeof(workSetInfo));
	// 要求操作进程的权限：PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
	// 第一次调用获取实际缓冲区大小
	bRet = ::QueryWorkingSet(hProcess, &workSetInfo, sizeof(workSetInfo));
	if (!bRet) // 调用失败
	{
		if (GetLastError() == ERROR_BAD_LENGTH) // 需要重新分配缓冲区
		{
			UINT_PTR realSize = sizeof(workSetInfo.NumberOfEntries) + workSetInfo.NumberOfEntries * sizeof(PSAPI_WORKING_SET_BLOCK);
			try
			{
				pByte = new BYTE[realSize];
				memset(pByte, 0, realSize);
				pWorkSetBlock = reinterpret_cast<PSAPI_WORKING_SET_BLOCK *>(pByte + sizeof(workSetInfo.NumberOfEntries));
				// 重新获取
				if (!::QueryWorkingSet(hProcess, pByte, static_cast<DWORD>(realSize)))
				{
					delete[] pByte; // 清理内存
					return 0;
				}
			}
			catch (...) // 分配内存失败
			{
				return 0;
			}

		}
		else // 其它错误，认为获取失败
		{
			return 0;
		}
	}


	ULONGLONG workSetPrivate = 0;
	for (ULONG_PTR i = 0; i < workSetInfo.NumberOfEntries; ++i)
	{
		if (!pWorkSetBlock[i].Shared) // 如果不是共享页
		{
			workSetPrivate += 4096;
		}
	}
	if (pByte)
	{
		delete[] pByte;
	}
	return workSetPrivate;

}

BOOL libTools::CProcess::CreateProc(_In_ CONST std::wstring& wsProcPath, _In_ BOOL IsSupended, _In_ CONST std::wstring wsSpecifyWorkDirectory, _Out_opt_ PROCESS_INFORMATION* pRetProcInfo)
{
	WCHAR					wszProcWorkDirectory[MAX_PATH] = { 0 };
	std::shared_ptr<WCHAR>	wszProcPath(new WCHAR[wsProcPath.length() + 1], [](WCHAR* p) { delete[] p; });


	CCharacter::strcpy_my(wszProcPath.get(), wsProcPath.c_str());

	if (wsSpecifyWorkDirectory.empty())
	{
		CCharacter::strcpy_my(wszProcWorkDirectory, wsProcPath.c_str());
		if (!::PathRemoveFileSpecW(wszProcWorkDirectory))
		{
			::MessageBoxW(nullptr, CCharacter::MakeFormatText(L"Remove ProcPath[%s] FileSpec Faild", wsProcPath.c_str()).c_str(), L"Error", NULL);
			return FALSE;
		}
	}
	else
	{
		CCharacter::strcpy_my(wszProcWorkDirectory, wsSpecifyWorkDirectory.c_str());
	}

	
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi = { 0 };
	if (!::CreateProcessW(NULL, wszProcPath.get(), NULL, NULL, FALSE, IsSupended ? CREATE_SUSPENDED : CREATE_NEW_CONSOLE, NULL, wszProcWorkDirectory, &si, &pi))
	{
		::MessageBoxW(nullptr, CCharacter::MakeFormatText(L"CreateProcess Fiald! ErrorCode=%X", ::GetLastError()).c_str(), L"Error", NULL);
		return FALSE;
	}

	if (pRetProcInfo != nullptr)
	{
		*pRetProcInfo = pi;
	}
	else
	{
		::CloseHandle(pi.hProcess);
	}
	return TRUE;
}
