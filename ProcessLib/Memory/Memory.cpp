#include "Memory.h"
#include "psapi.h" // EmptyWorkingSet
#include <include/CharacterLib/Character.h>

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



#pragma comment(lib,"Psapi.lib")

BOOL libTools::CMemory::ClearWorkingMemory()
{
	return EmptyWorkingSet(::GetCurrentProcess());
}

UINT_PTR libTools::CMemory::ReadMemValue(_In_ UINT_PTR dwAddr)
{
	return ReadMemoryValue<UINT_PTR>(dwAddr);
}

DWORD libTools::CMemory::ReadDWORD(_In_ UINT_PTR dwAddr)
{
	return ReadMemoryValue<DWORD>(dwAddr);
}

WORD libTools::CMemory::ReadWORD(_In_ UINT_PTR dwAddr)
{
	return ReadMemoryValue<WORD>(dwAddr);
}

BYTE libTools::CMemory::ReadBYTE(_In_ UINT_PTR dwAddr)
{
	return ReadMemoryValue<BYTE>(dwAddr);
}

FLOAT libTools::CMemory::ReadFloat(_In_ UINT_PTR dwAddr)
{
	return ReadMemoryValue<FLOAT>(dwAddr);
}

double libTools::CMemory::ReadDouble(_In_ UINT_PTR dwAddr)
{
	return ReadMemoryValue<DOUBLE>(dwAddr);
}

BOOL libTools::CMemory::WriteDWORD64(_In_ DWORD64 dwAddr, _In_ DWORD64 dwValue)
{
	return WriteMemoryValue(dwAddr, dwValue);
}

BOOL libTools::CMemory::WriteDWORD(_In_ UINT_PTR dwAddr, _In_ DWORD dwValue)
{
	return WriteMemoryValue(dwAddr, dwValue);
}

BOOL libTools::CMemory::WriteFloat(_In_ UINT_PTR dwAddr, _In_ FLOAT fValue)
{
	return WriteMemoryValue(dwAddr, fValue);
}

BOOL libTools::CMemory::WriteBYTE(_In_ UINT_PTR dwAddr, _In_ BYTE bValue)
{
	return WriteMemoryValue(dwAddr, bValue);
}

std::wstring libTools::CMemory::ReadUTF8Text(_In_ UINT_PTR dwAddr)
{
	if (::IsBadCodePtr(reinterpret_cast<FARPROC>(dwAddr)))
	{
		return std::wstring();
	}
	else if (ReadDWORD(dwAddr + 0x14) > 0xF)
	{
		CONST CHAR* pszText = reinterpret_cast<CONST CHAR*>(ReadMemValue(dwAddr));
		if (pszText == nullptr)
		{
			return std::wstring();
		}

		
		return CCharacter::UTF8ToUnicode(std::string(pszText));
	}


	CONST CHAR* pszText = reinterpret_cast<CONST CHAR*>(dwAddr);
	return CCharacter::UTF8ToUnicode(std::string(pszText));
}

std::wstring libTools::CMemory::ReadASCIIText(_In_ UINT_PTR dwAddr)
{
	if (::IsBadCodePtr(reinterpret_cast<FARPROC>(dwAddr)))
	{
		return std::wstring();
	}
	else if (ReadDWORD(dwAddr + 0x14) > 0xF)
	{
		CONST CHAR* pszText = reinterpret_cast<CONST CHAR*>(ReadMemValue(dwAddr));
		if (pszText == nullptr)
		{
			return std::wstring();
		}
		if (::IsBadCodePtr(reinterpret_cast<FARPROC>(pszText)))
		{
			return std::wstring();
		}

		return CCharacter::ASCIIToUnicode(std::string(pszText));
	}


	CONST CHAR* pszText = reinterpret_cast<CONST CHAR*>(dwAddr);
	return CCharacter::ASCIIToUnicode(std::string(pszText));
}

std::wstring libTools::CMemory::ReadUnicodeText(_In_ UINT_PTR dwAddr)
{
	if (::IsBadCodePtr(reinterpret_cast<FARPROC>(dwAddr)))
	{
		return std::wstring();
	}
	else if (ReadDWORD(dwAddr + 0x14) > 0xF)
	{
		CONST WCHAR* pszText = reinterpret_cast<CONST WCHAR*>(ReadMemValue(dwAddr));
		if (pszText == nullptr)
		{
			return std::wstring();
		}


		return std::wstring(pszText);
	}


	CONST WCHAR* pszText = reinterpret_cast<CONST WCHAR*>(dwAddr);
	return std::wstring(pszText);
}
