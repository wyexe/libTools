#include "IATHook.h"
#include <include/LogLib/Log.h>
#include <include/CharacterLib/Character.h>

#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#endif // _DEBUG


#define _SELF L"IATHook.cpp"
BOOL libTools::CIATHook::Hook(_In_ CONST std::string& szDLLName, _In_ CONST std::string& szMethodName, _In_ LPVOID HookProcPtr, _Out_opt_ LPVOID* pRealProcPtr)
{
	UINT_PTR dwImageBase = reinterpret_cast<UINT_PTR>(::GetModuleHandleW(NULL));

	PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(dwImageBase);
	if (pDosHeader == nullptr)
	{
		LOG_C_E(L"pDosHeader = nullptr");
		return FALSE;
	}


	PIMAGE_NT_HEADERS pNtHeader = reinterpret_cast<IMAGE_NT_HEADERS *>(pDosHeader->e_lfanew + dwImageBase);
	if (pNtHeader == nullptr)
	{
		LOG_C_E(L"pNtHeader = nullptr");
		return FALSE;
	}


	if (pNtHeader->Signature == NULL)
	{
		LOG_C_E(L"pNtHeader->Signature");
		return FALSE;
	}


	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(dwImageBase + pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	for (std::string szLowerDllName = CCharacter::MakeTextToLower(szDLLName); pImportDescriptor->Name != NULL; pImportDescriptor++)
	{
		CHAR* pszImportDLLName = reinterpret_cast<CHAR*>(dwImageBase) + pImportDescriptor->Name;
		if (CCharacter::MakeTextToLower(std::string(pszImportDLLName)) != szLowerDllName)
		{
			continue;
		}


		auto pOriginThunk = reinterpret_cast<IMAGE_THUNK_DATA *>(dwImageBase + pImportDescriptor->OriginalFirstThunk);
		auto pRealThunk = reinterpret_cast<IMAGE_THUNK_DATA *>(dwImageBase + pImportDescriptor->FirstThunk);
		for (std::string szLowerMethodName = CCharacter::MakeTextToLower(szMethodName); pOriginThunk->u1.Function != NULL; pOriginThunk++, pRealThunk++)
		{
#ifdef _WIN64
			if ((pOriginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) == 0)
#endif // _WIN64
			if ((pOriginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32) == 0)
			{
				IMAGE_IMPORT_BY_NAME* pImageImportName = reinterpret_cast<IMAGE_IMPORT_BY_NAME *>(dwImageBase + pOriginThunk->u1.AddressOfData);
				if (szLowerMethodName == CCharacter::MakeTextToLower(std::string(pImageImportName->Name)))
				{
					if (pRealProcPtr != nullptr)
					{
						*pRealProcPtr = reinterpret_cast<LPVOID>(pRealThunk->u1.Function);
					}


					DWORD dwOldProtect = NULL;
					::VirtualProtect(&pRealThunk->u1.Function, 8, PAGE_EXECUTE_READWRITE, &dwOldProtect);
					pRealThunk->u1.Function = reinterpret_cast<UINT_PTR>(HookProcPtr);
					::VirtualProtect(&pRealThunk->u1.Function, 8, dwOldProtect, &dwOldProtect);
					return TRUE;
				}
			}
		}

		break;
	}

	LOG_C_E(L"Search Method Faild!");
	return FALSE;
}
