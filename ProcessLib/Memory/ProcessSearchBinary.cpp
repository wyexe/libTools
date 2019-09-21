#include "ProcessSearchBinary.h"
#include "../Process/ProcessModule.h"

#ifndef _WIN64
VOID libTools::CProcessSearchBinary::Initialize(_In_ DWORD dwMaxSearchCount, _In_ HANDLE hProcess)
{
	_dwMaxSearchCount = dwMaxSearchCount;
	_hProcess = hProcess;
}

DWORD libTools::CProcessSearchBinary::FindAddr(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nOrderNum, _In_ LPCWSTR lpszModule)
{
	if (!SetModuleSearchBinaryRange(lpszModule))
		return 0;


	std::vector<DWORD> Vec;
	if (!SearchBase(lpszCode, Vec) || static_cast<std::size_t>(nOrderNum) >= Vec.size())
		return 0;


	return nOffset >= 0 ? Vec.at(nOrderNum) - abs(nOffset) : Vec.at(nOrderNum) + abs(nOffset);
}

DWORD libTools::CProcessSearchBinary::FindCALL(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nMov, _In_ int nOrderNum, _In_ LPCWSTR lpszModule)
{
	if (!SetModuleSearchBinaryRange(lpszModule))
		return 0;


	std::vector<DWORD> Vec;
	if (!SearchBase(lpszCode, Vec) || static_cast<std::size_t>(nOrderNum) >= Vec.size())
		return 0;



	DWORD dwAddr = nOffset >= 0 ? Vec.at(nOrderNum) - abs(nOffset) : Vec.at(nOrderNum) + abs(nOffset);
	if (ReadValue<BYTE>(dwAddr) == 0xE8)
	{
		//首先计算相对地址
		DWORD dwModuleAddr = reinterpret_cast<DWORD>(::GetModuleHandleW(lpszModule));
		DWORD dwRelativeAddr = dwAddr - (dwModuleAddr + 0x1000) + 0x1000 + nMov;
		dwRelativeAddr += dwModuleAddr;
		DWORD dwReadAddr = ReadValue<DWORD>(dwRelativeAddr);
		dwReadAddr += 4;
		dwReadAddr += dwRelativeAddr;
		return dwReadAddr & 0xFFFFFFFF;
	}

	return 0;
}

DWORD libTools::CProcessSearchBinary::FindBase(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nMov, _In_ int nOrderNum, _In_ LPCWSTR lpszModule, DWORD dwAddrLen /*= 0xFFFFFFFF*/)
{
	if (!SetModuleSearchBinaryRange(lpszModule))
		return 0;


	std::vector<DWORD> Vec;
	if (!SearchBase(lpszCode, Vec) || static_cast<std::size_t>(nOrderNum) >= Vec.size())
		return 0;


	DWORD dwAddr = nOffset >= 0 ? Vec.at(nOrderNum) - abs(nOffset) : Vec.at(nOrderNum) + abs(nOffset);
	dwAddr += nMov;
	return ReadValue<DWORD>(dwAddr) & dwAddrLen;
}

DWORD libTools::CProcessSearchBinary::FindBase_ByCALL(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nMov, _In_ int nOrderNum, _In_ LPCWSTR lpszModule, _In_ int nBaseOffset, _In_opt_ DWORD dwAddrLen /*= 0xFFFFFFFF*/)
{
	DWORD dwCALL = FindCALL(lpszCode, nOffset, nMov, nOrderNum, lpszModule);
	if (dwCALL == NULL)
		return 0;


	dwCALL += nBaseOffset;
	return ReadValue<DWORD>(dwCALL) & dwAddrLen;
}

UINT libTools::CProcessSearchBinary::FindAddr(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ LPCWSTR lpszModule, _Out_ std::vector<DWORD>& Vec)
{
	if (!SetModuleSearchBinaryRange(lpszModule))
		return 0;


	std::vector<DWORD> VecAddr;
	if (!SearchBase(lpszCode, VecAddr))
		return 0;


	for (auto& itm : VecAddr)
	{
		Vec.push_back(nOffset >= 0 ? itm - abs(nOffset) : itm + abs(nOffset));
	}
	return Vec.size();
}

BOOL libTools::CProcessSearchBinary::SetModuleSearchBinaryRange(_In_ LPCWSTR lpszModule)
{
	if (lpszModule == nullptr)
	{
		_SearchRange.dwBeginAddr = 0x1000;
		_SearchRange.dwEndAddr   = 0x7FFFFFFF;
		return TRUE;
	}


	CProcessModule::ProcessModuleContent ModuleContent;
	if (!CProcessModule::FindProcessModule(_hProcess, lpszModule, ModuleContent))
		return FALSE;


	_SearchRange.dwBeginAddr = ModuleContent.dwImageBase;
	_SearchRange.dwEndAddr = ModuleContent.dwImageBase + ModuleContent.dwImageSize;
	return TRUE;
}

BOOL libTools::CProcessSearchBinary::SearchBase(_In_ LPCSTR szCode, _Out_ std::vector<DWORD>& Vec)
{
	SYSTEM_INFO		si;
	MEMORY_BASIC_INFORMATION		mbi;


	//将字符串转换成BYTE数组
	UINT uCodeLen = static_cast<UINT>(strlen(szCode)) / 2;
	if (strlen(szCode) % 2 != 0)
		return FALSE;


	std::vector<DWORD> VecCode;
	for (UINT i = 0; i < uCodeLen; ++i)
	{
		if (szCode[i * 2] != '?')
		{
			char szText[] = { szCode[i * 2], szCode[i * 2 + 1], '\0' };
			VecCode.push_back(static_cast<DWORD>(strtol(szText, NULL, 16)));
		}
		else
		{
			VecCode.push_back(0x100);
		}
	}

	//初始化
	::GetSystemInfo(&si);
	for (DWORD dwAddr = _SearchRange.dwBeginAddr; dwAddr < _SearchRange.dwEndAddr; dwAddr += mbi.RegionSize)
		//for (BYTE * pCurPos = (LPBYTE)si.lpMinimumApplicationAddress; pCurPos < (LPBYTE)si.lpMaximumApplicationAddress - uCodeLen; pCurPos = (PBYTE)mbi.BaseAddress + mbi.RegionSize,nCount++ )
	{
		//查询当前内存页的属性
		if(::VirtualQueryEx(_hProcess, (LPCVOID)dwAddr, &mbi, sizeof(mbi)) == 0)
			continue;


		if (mbi.State == MEM_COMMIT && (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE || PAGE_READONLY))
		{
			std::vector<int> vlst;
			CL_sunday(VecCode.data(), uCodeLen, reinterpret_cast<BYTE *>(mbi.BaseAddress), mbi.RegionSize, vlst);

			for (UINT i = 0; i < vlst.size() && vlst.size() < _dwMaxSearchCount; ++i)
			{
				Vec.push_back(reinterpret_cast<DWORD>(mbi.BaseAddress) + vlst.at(i));
			}
		}
	}


	return !Vec.empty();
}

BOOL libTools::CProcessSearchBinary::CL_sunday(_In_ CONST DWORD* pKey, _In_ UINT uKeyLen, _In_ BYTE* pCode, _In_ UINT uCodeLen, _Out_ std::vector<int>& vlst)
{
	UINT uNowPos = 0;
	while (uNowPos + uKeyLen < uCodeLen)
	{
		if (CompCode(pKey, pCode + uNowPos, uKeyLen))
		{
			vlst.push_back(uNowPos);
			uNowPos += uKeyLen + 1;
			continue;
		}

		BYTE bWord = pCode[uKeyLen + uNowPos];

		int nWordPos = GetWord_By_Char(bWord, pKey, uKeyLen);
		if (nWordPos == -1)
			uNowPos += uKeyLen + 1;
		else
			uNowPos += (UINT)nWordPos;
	}

	return TRUE;
}

int libTools::CProcessSearchBinary::GetWord_By_Char(_In_ BYTE dwWord, _In_ CONST DWORD* pKey, _In_ UINT uKeyLen)
{
	int uLen = uKeyLen - 1;
	for (int i = uLen; i >= 0; --i)
	{
		if (pKey[i] == 0x100 || (BYTE)pKey[i] == dwWord)
		{
			return uKeyLen - i;
		}
	}
	return -1;
}

BOOL libTools::CProcessSearchBinary::CompCode(_In_ const DWORD * pCode, _In_ const BYTE * pMem, _In_ UINT uLen)
{
	BOOL bComp = TRUE;
	for (UINT i = 0; i < uLen; ++i)
	{
		if (pCode[i] != 0x100 && (BYTE)pCode[i] != (BYTE)pMem[i])
		{
			bComp = FALSE;
			break;
		}
	}

	return bComp;
}

#endif