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


		// ��ʼ������
		BYTE bJmpFlag = 0xE9;
		BYTE bNopFlag = 0x90;
		BOOL  bRetCode = FALSE;
		DWORD dwOldProtent = NULL;
		HANDLE curProcess = ::GetCurrentProcess();

		// �����ڴ�ƫ�Ƶ�ַ
		DWORD dwJmpAddr = pHookContent->dwFunAddr - pHookContent->dwHookAddr - 5;

		// ��ȡ�ɵĴ���
		pHookContent->dwOldCode1 = CMemory::ReadDWORD(pHookContent->dwHookAddr + 0x0);
		pHookContent->dwOldCode2 = CMemory::ReadDWORD(pHookContent->dwHookAddr + 0x4);

		// �޸��ڴ�ҳ����,��Ȼ���ܲ���д��Ȩ��
		::VirtualProtect((LPVOID)pHookContent->dwHookAddr, 8, PAGE_EXECUTE_READWRITE, &dwOldProtent);

		// д��Jmpָ��
		//*(BYTE*)pHookContent->dwHookAddr = bJmpFlag;
		::WriteProcessMemory(curProcess, (LPVOID)pHookContent->dwHookAddr, &bJmpFlag, 1, 0);

		// д���Լ������Addr
		//*(DWORD*)(pHookContent->dwHookAddr + 1) = dwJmpAddr;
		bRetCode = ::WriteProcessMemory(curProcess, (LPVOID)(pHookContent->dwHookAddr + 1), &dwJmpAddr, 4, 0);

		// Nop������Ĵ���
		for (UINT i = 0; i < pHookContent->uNopCount; ++i)
		{
			::WriteProcessMemory(curProcess, (LPVOID)(pHookContent->dwHookAddr + 5 + i), &bNopFlag, 1, 0);
			//*(BYTE*)(pHookContent->dwHookAddr + 5 + i) = bNopFlag;
		}

		// �ָ��ֳ�
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
