#include "InlineHook.h"
#include <include/ExceptionLib/Exception.h>
#include <include/ProcessLib/Memory/Memory.h>
#include "LdrHeader.h"

#ifdef _DEBUG
#pragma comment(lib,"ExceptionLib_Debug.lib")
#pragma comment(lib,"ProcessLib_Debug.lib")
#else
#pragma comment(lib,"ExceptionLib.lib")
#pragma comment(lib,"ProcessLib.lib")
#endif // _DEBUG

BOOL libTools::CInlineHook::Hook(_In_ _Out_ HookContent* pHookContent)
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		if (pHookContent == nullptr)
			return FALSE;


		// 初始化变量
		BYTE bJmpFlag = 0xE9;
		BYTE bNopFlag = 0x90;
		BOOL  bRetCode = FALSE;
		DWORD dwOldProtent = NULL;
		HANDLE curProcess = ::GetCurrentProcess();

		// 计算内存偏移地址
		DWORD dwJmpAddr = pHookContent->dwFunAddr - pHookContent->dwHookAddr - 5;

		// 获取旧的代码
		pHookContent->dwOldCode1 = CMemory::ReadDWORD(pHookContent->dwHookAddr + 0x0);
		pHookContent->dwOldCode2 = CMemory::ReadDWORD(pHookContent->dwHookAddr + 0x4);

		// 修改内存页属性,不然可能不让写的权限
		::VirtualProtect((LPVOID)pHookContent->dwHookAddr, 8, PAGE_EXECUTE_READWRITE, &dwOldProtent);

		// 写入Jmp指令
		//*(BYTE*)pHookContent->dwHookAddr = bJmpFlag;
		::WriteProcessMemory(curProcess, (LPVOID)pHookContent->dwHookAddr, &bJmpFlag, 1, 0);

		// 写入自己代码的Addr
		//*(DWORD*)(pHookContent->dwHookAddr + 1) = dwJmpAddr;
		bRetCode = ::WriteProcessMemory(curProcess, (LPVOID)(pHookContent->dwHookAddr + 1), &dwJmpAddr, 4, 0);

		// Nop掉多余的代码
		for (UINT i = 0; i < pHookContent->uNopCount; ++i)
		{
			::WriteProcessMemory(curProcess, (LPVOID)(pHookContent->dwHookAddr + 5 + i), &bNopFlag, 1, 0);
			//*(BYTE*)(pHookContent->dwHookAddr + 5 + i) = bNopFlag;
		}

		// 恢复现场
		DWORD dwProtect = NULL;
		::VirtualProtect((LPVOID)pHookContent->dwHookAddr, 8, dwOldProtent, &dwProtect);
		return TRUE;
	});
}

BOOL libTools::CInlineHook::UnHook(_In_ LPVOID OrgProc, _In_ LPVOID RealProc)
{
	CLdrHeader::UnInlineHook(OrgProc, RealProc);
	return TRUE;
}

BOOL libTools::CInlineHook::UnHook(_In_ _Out_ HookContent* pHookContent)
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		DWORD dwProtect = NULL;

		// Update Memory Page
		::VirtualProtect((LPVOID)pHookContent->dwHookAddr, 8, PAGE_EXECUTE_READWRITE, &dwProtect);

		// Repair Code
		::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)(pHookContent->dwHookAddr + 0x0), &pHookContent->dwOldCode1, 4, NULL);
		::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)(pHookContent->dwHookAddr + 0x4), &pHookContent->dwOldCode2, 4, NULL);

		DWORD dwOldProtect = NULL;
		::VirtualProtect((LPVOID)pHookContent->dwHookAddr, 8, dwProtect, &dwOldProtect);
		return TRUE;
	});
}

BOOL libTools::CInlineHook::Hook(_In_ LPVOID OrgProc, _In_ LPVOID NewProc, _Out_ LPVOID* RealProc)
{
	return CLdrHeader::InlineHook(OrgProc, NewProc, RealProc);
}
