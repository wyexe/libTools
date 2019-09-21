#include "Thread.h"
#include <tlhelp32.h>

DWORD libTools::CThread::GetMainThreadId()
{
	THREADENTRY32 te32 = { sizeof(te32) };
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}


	if (Thread32First(hThreadSnap, &te32))
	{
		do 
		{
			if (::GetCurrentProcessId() == te32.th32OwnerProcessID)
			{
				::CloseHandle(hThreadSnap);
				return te32.th32ThreadID;
			}
		} while (Thread32Next(hThreadSnap, &te32));
	}


	::CloseHandle(hThreadSnap);
	return NULL;
}
