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

	BOOL WINAPI CLdrHeader::GetPatchSize(_In_ LPVOID Proc, /* ��ҪHook�ĺ�����ַ */_In_ DWORD dwNeedSize, /* Hook����ͷ��ռ�õ��ֽڴ�С */_Out_ LPDWORD lpPatchSize/* ���ظ��ݺ���ͷ������Ҫ�޲��Ĵ�С */)
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
		DWORD dwPatchSize;    // �õ���Ҫpatch���ֽڴ�С
							  //DWORD dwOldProtect;
		LPVOID lpHookFunc;    // �����Hook�������ڴ�
		DWORD dwBytesNeed;    // �����Hook�����Ĵ�С
		LPBYTE lpPatchBuffer; // jmp ָ�����ʱ������

		if (!OrgProc || !NewProc || !RealProc)
		{
			return FALSE;
		}
		// �õ���Ҫpatch���ֽڴ�С
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

		dwBytesNeed = sizeof(DWORD) + JMP_SIZE + dwPatchSize + JMP_SIZE + 0x5/*���ʼ�Ĵ���*/;

		lpHookFunc = __malloc(dwBytesNeed);

		//����dwPatchSize��lpHookFunc
		*(DWORD *)lpHookFunc = dwPatchSize;

		//������ͷ��4���ֽ�
		lpHookFunc = (LPVOID)((DWORD)lpHookFunc + sizeof(DWORD));

		//��ʼbackup������ͷ����
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

		//NOP���
		memset(lpPatchBuffer, 0x90, dwPatchSize);

		//jmp��Hook
		*(BYTE *)lpHookFunc = 0xE9;
		*(DWORD*)((DWORD)lpHookFunc + 1) = (DWORD)NewProc - (DWORD)lpHookFunc - JMP_SIZE;

		//����ԭʼ
		*(BYTE *)((DWORD)lpHookFunc + 5 + dwPatchSize) = 0xE9;
		*(DWORD*)((DWORD)lpHookFunc + 5 + dwPatchSize + 1) = ((DWORD)OrgProc + dwPatchSize) - ((DWORD)lpHookFunc + JMP_SIZE + dwPatchSize) - JMP_SIZE;


		//jmp 
		*(BYTE *)lpPatchBuffer = 0xE9;
		//ע����㳤�ȵ�ʱ�����OrgProc
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

		//�ҵ�����Ŀռ�
		lpBuffer = (LPBYTE)((DWORD)RealProc - (sizeof(DWORD) + JMP_SIZE));
		//�õ�dwPatchSize
		dwPatchSize = *(DWORD *)lpBuffer;

		if (*reinterpret_cast<BYTE*>(RealProc) == 0xE9)
			WriteReadOnlyMemory((LPBYTE)OrgProc, (LPBYTE)(lpBuffer + sizeof(DWORD) + JMP_SIZE + dwPatchSize + JMP_SIZE), dwPatchSize);
		else
			WriteReadOnlyMemory((LPBYTE)OrgProc, (LPBYTE)RealProc, dwPatchSize);

		//�ͷŷ������ת�����Ŀռ�
		__free(lpBuffer);

		return;
	}
}