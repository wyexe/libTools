#include "LdrHeader.h"
#include <stdlib.h>
#include <include/CharacterLib/Character.h>
#include <ProcessLib/Process/ProcessModule.h>
#include "Dasm.h"

#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#endif // _DEBUG

namespace libTools
{
#define JMP_SIZE		5
#define __malloc(_s)	VirtualAlloc(NULL, _s, MEM_COMMIT, PAGE_EXECUTE_READWRITE)
#define __free(_p)		VirtualFree(_p, 0, MEM_RELEASE)
	BOOL WINAPI CLdrHeader::WriteReadOnlyMemory(_In_ LPBYTE	lpDest, _In_  LPBYTE	lpSource, _In_ ULONG	Length)
	{
		BOOL bRet;
		DWORD dwOldProtect;
		bRet = FALSE;

		if (!VirtualProtect(lpDest, Length, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		{
			return bRet;
		}


		memcpy(lpDest, lpSource, Length);
		bRet = VirtualProtect(lpDest, Length, dwOldProtect, &dwOldProtect);
		return	bRet;
	}

	BOOL WINAPI CLdrHeader::GetPatchSize(_In_ LPVOID Proc, /* 需要Hook的函数地址 */_In_ DWORD dwNeedSize, /* Hook函数头部占用的字节大小 */_Out_ LPDWORD lpPatchSize/* 返回根据函数头分析需要修补的大小 */)
	{
		DWORD Length;
		PUCHAR pOpcode;
		DWORD PatchSize = 0;

		if (!Proc || !lpPatchSize)
		{
			return FALSE;
		}

		CDasm LDasm;

		do
		{
			Length = LDasm.SizeOfCode(Proc, &pOpcode);
			if ((Length == 1) && (*pOpcode == 0xC3))
				break;
			if ((Length == 3) && (*pOpcode == 0xC2))
				break;
			Proc = (PVOID)((DWORD)Proc + Length);

			PatchSize += Length;
			if (PatchSize >= dwNeedSize)
			{
				break;
			}

		} while (Length);

		*lpPatchSize = PatchSize;

		return TRUE;
	}

	BOOL WINAPI CLdrHeader::InlineHook(_In_ LPVOID OrgProc, _In_ LPVOID NewProc, _Out_ LPVOID* RealProc)
	{
		DWORD dwPatchSize;    // 得到需要patch的字节大小
							  //DWORD dwOldProtect;
		LPVOID lpHookFunc;    // 分配的Hook函数的内存
		DWORD dwBytesNeed;    // 分配的Hook函数的大小
		LPBYTE lpPatchBuffer; // jmp 指令的临时缓冲区

		if (!OrgProc || !NewProc || !RealProc)
		{
			return FALSE;
		}
		// 得到需要patch的字节大小
		if (!GetPatchSize(OrgProc, JMP_SIZE, &dwPatchSize))
		{
			return FALSE;
		}

		/*
		0x00000800					0x00000800		sizeof(DWORD)	// dwPatchSize
		JMP	/ FAR 0xAABBCCDD		E9 DDCCBBAA		JMP_SIZE
		...							...				dwPatchSize		// Backup instruction
		JMP	/ FAR 0xAABBCCDD		E9 DDCCBBAA		JMP_SIZE
		*/

		dwBytesNeed = sizeof(DWORD) + JMP_SIZE + dwPatchSize + JMP_SIZE + 0x5/*最初始的代码*/;

		lpHookFunc = __malloc(dwBytesNeed);

		//备份dwPatchSize到lpHookFunc
		*(DWORD *)lpHookFunc = dwPatchSize;

		//跳过开头的4个字节
		lpHookFunc = (LPVOID)((DWORD)lpHookFunc + sizeof(DWORD));

		//开始backup函数开头的字
		memcpy((BYTE *)lpHookFunc + JMP_SIZE, OrgProc, dwPatchSize);
		if (*reinterpret_cast<BYTE*>(OrgProc) == 0xE9)
		{
			std::vector<CProcessModule::ProcessModuleContent> Vec;
			CProcessModule::GetProcessModule(::GetCurrentProcess(), Vec);
			for (auto& itm : Vec)
			{
				if (reinterpret_cast<DWORD>(OrgProc) > itm.dwImageBase && reinterpret_cast<DWORD>(OrgProc) < itm.dwImageBase + itm.dwImageSize)
				{
					DWORD dwRelativeAddr = (DWORD)OrgProc - (itm.dwImageBase + 0x1000) + 0x1000 + 0x1;
					dwRelativeAddr += itm.dwImageBase;
					DWORD dwReadAddr = *(DWORD*)(dwRelativeAddr);
					dwReadAddr += 4;
					dwReadAddr += dwRelativeAddr;
					DWORD dwCALL = dwReadAddr & 0xFFFFFFFF;


					DWORD dwNewCALL = dwCALL - ((DWORD)lpHookFunc + JMP_SIZE) - JMP_SIZE;
					*(DWORD *)((DWORD)lpHookFunc + JMP_SIZE + 0x1) = dwNewCALL;
					memcpy((BYTE *)lpHookFunc + JMP_SIZE + dwPatchSize + JMP_SIZE, OrgProc, 5);
					break;
				}
			}
		}

		lpPatchBuffer = (LPBYTE)__malloc(dwPatchSize);

		//NOP填充
		memset(lpPatchBuffer, 0x90, dwPatchSize);

		//jmp到Hook
		*(BYTE *)lpHookFunc = 0xE9;
		*(DWORD*)((DWORD)lpHookFunc + 1) = (DWORD)NewProc - (DWORD)lpHookFunc - JMP_SIZE;

		//跳回原始
		*(BYTE *)((DWORD)lpHookFunc + 5 + dwPatchSize) = 0xE9;
		*(DWORD*)((DWORD)lpHookFunc + 5 + dwPatchSize + 1) = ((DWORD)OrgProc + dwPatchSize) - ((DWORD)lpHookFunc + JMP_SIZE + dwPatchSize) - JMP_SIZE;


		//jmp 
		*(BYTE *)lpPatchBuffer = 0xE9;
		//注意计算长度的时候得用OrgProc
		*(DWORD*)(lpPatchBuffer + 1) = (DWORD)lpHookFunc - (DWORD)OrgProc - JMP_SIZE;

		WriteReadOnlyMemory((LPBYTE)OrgProc, lpPatchBuffer, dwPatchSize);

		__free(lpPatchBuffer);

		*RealProc = (void *)((DWORD)lpHookFunc + JMP_SIZE);

		return TRUE;
	}

	VOID WINAPI CLdrHeader::UnInlineHook(_In_ LPVOID OrgProc, _In_ LPVOID RealProc)
	{
		DWORD dwPatchSize;
		//DWORD dwOldProtect;
		LPBYTE lpBuffer;

		//找到分配的空间
		lpBuffer = (LPBYTE)((DWORD)RealProc - (sizeof(DWORD) + JMP_SIZE));
		//得到dwPatchSize
		dwPatchSize = *(DWORD *)lpBuffer;

		if (*reinterpret_cast<BYTE*>(RealProc) == 0xE9)
			WriteReadOnlyMemory((LPBYTE)OrgProc, (LPBYTE)(lpBuffer + sizeof(DWORD) + JMP_SIZE + dwPatchSize + JMP_SIZE), dwPatchSize);
		else
			WriteReadOnlyMemory((LPBYTE)OrgProc, (LPBYTE)RealProc, dwPatchSize);

		//释放分配的跳转函数的空间
		__free(lpBuffer);

		return;
	}
}