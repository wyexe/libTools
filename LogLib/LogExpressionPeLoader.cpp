#include "LogExpressionPeLoader.h"
#include <include/FileLib/File.h>
#include <include/CharacterLib/Character.h>
#include "Log.h"


#define _SELF L"ExprPE.cpp"
#ifndef _WIN64
libTools::CLogExpressionPeLoader::~CLogExpressionPeLoader()
{
	if (_bAlloc && _pvFileContent != nullptr)
	{
		::VirtualFree(_pvFileContent, 0, MEM_RELEASE);
		_pvFileContent = nullptr;
		_uFileSize = NULL;
	}
}

VOID libTools::CLogExpressionPeLoader::PeLoader(_In_ CONST std::vector<std::wstring>& VecParam)
{
	if (VecParam.size() != 2)
	{
		LOG_C_E(L"LoadPE(FromFile,FilePath)");
		return;
	}

	CONST std::wstring& wsEchoType = VecParam.at(0);
	if (CCharacter::MakeTextToLower(wsEchoType) == CCharacter::MakeTextToLower(std::wstring(L"FromFile")))
	{
		CLogExpressionPeLoader ExprPe;
		if (!ExprPe.SetFileContent(VecParam.at(1)))
		{
			LOG_C_E(L"Load File[%s] Faild!!!", VecParam.at(1).c_str());
			return;
		}

		std::vector<ExportTable> VecExportTable;
		if (ExprPe.GetVecExportTable(VecExportTable))
		{
			LOG_C_D(L"Export Table.Count=%d", VecExportTable.size());
			for (CONST auto& itm : VecExportTable)
			{
				LOG_C_D(L"Export Ordinal=%llx,Offset=%llx,Name=%s", itm.dwOrdinal, itm.dwMethodPtr, itm.wsFunName.c_str());
			}
		}

		std::vector<ImportTable> VecImportTable;
		if (ExprPe.GetVecImportTable(VecImportTable))
		{
			LOG_C_D(L"Import Table.Count=%d", VecImportTable.size());
			for (CONST auto& itm : VecImportTable)
			{
				LOG_C_D(L"Import.DLLName=%s", itm.wsDLLName.c_str());
				for (CONST auto& ImportItem : itm.VecTable)
				{
					LOG_C_D(L"Import Ordinal=%llx, Name=%s", ImportItem.dwFuncRVA, ImportItem.wsAPIName.c_str());
				}
			}
		}

	}
}

BOOL libTools::CLogExpressionPeLoader::SetFileContent(_In_ CONST std::wstring& wsFilePath)
{
	if (!CFile::ReadFileContent(wsFilePath, _pvFileContent, _uFileSize))
	{
		LOG_C_E(L"UnExist File:%s", wsFilePath.c_str());
		return FALSE;
	}

	_bAlloc = TRUE;
	return IsValidPE();
}

BOOL libTools::CLogExpressionPeLoader::SetFileContent(_In_ LPVOID pvCode, _In_ UINT uSize)
{
	_pvFileContent = pvCode;
	_uFileSize = uSize;
	_bAlloc = FALSE;
	return IsValidPE();
}

BOOL libTools::CLogExpressionPeLoader::GetVecImportTable(_Out_ std::vector<ImportTable>& Vec) CONST
{
	return IsX86() ? GetVecImportTableX86(Vec) : GetVecImportTableX64(Vec);
}

BOOL libTools::CLogExpressionPeLoader::GetVecExportTable(_Out_ std::vector<ExportTable>& Vec) CONST
{
	return IsX86() ? GetVecExportTableX86(Vec) : GetVecExportTableX64(Vec);
}

DWORD libTools::CLogExpressionPeLoader::GetImageBase() CONST
{
	return reinterpret_cast<DWORD>(_pvFileContent);
}

CONST IMAGE_DOS_HEADER* libTools::CLogExpressionPeLoader::GetDosHeader() CONST
{
	return reinterpret_cast<CONST IMAGE_DOS_HEADER *>(_pvFileContent);
}

CONST IMAGE_NT_HEADERS* libTools::CLogExpressionPeLoader::GetNtHeaderX86() CONST
{
	DWORD dwImageBase = reinterpret_cast<DWORD>(_pvFileContent);
	return reinterpret_cast<CONST IMAGE_NT_HEADERS *>(dwImageBase + GetDosHeader()->e_lfanew);
}

CONST IMAGE_NT_HEADERS64* libTools::CLogExpressionPeLoader::GetNtHeaderX64() CONST
{
	DWORD dwImageBase = reinterpret_cast<DWORD>(_pvFileContent);
	return reinterpret_cast<CONST IMAGE_NT_HEADERS64 *>(dwImageBase + GetDosHeader()->e_lfanew);
}

BOOL libTools::CLogExpressionPeLoader::IsX86() CONST
{
	return GetNtHeaderX86()->FileHeader.Machine == IMAGE_FILE_MACHINE_I386;
}

BOOL libTools::CLogExpressionPeLoader::IsValidPE() CONST
{
	if (GetDosHeader()->e_magic != IMAGE_DOS_SIGNATURE)
	{
		LOG_C_E(L"DosHeader.Magic=%4X", GetDosHeader()->e_magic);
		return FALSE;
	}

	CONST IMAGE_NT_HEADERS* pNtHeader = GetNtHeaderX86();
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		LOG_C_E(L"pNtHeader->Signature[%04X] != PE00", pNtHeader->Signature);
		return FALSE;
	}
	else if (pNtHeader->OptionalHeader.SectionAlignment & 1)
	{
		LOG_C_E(L"Invalid SectionAlignment[%X]", pNtHeader->OptionalHeader.SectionAlignment);
		return FALSE;
	}

	return TRUE;
}

DWORD libTools::CLogExpressionPeLoader::RvaToOffsetX86(_In_ DWORD dwRva) CONST
{
	CONST IMAGE_NT_HEADERS* pNtHeader = GetNtHeaderX86();
	CONST IMAGE_SECTION_HEADER* pSectionHeader = reinterpret_cast<CONST IMAGE_SECTION_HEADER *>(reinterpret_cast<CONST CHAR*>(pNtHeader) + sizeof(IMAGE_NT_HEADERS));

	for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i, ++pSectionHeader)
	{
		if (pSectionHeader->VirtualAddress <= dwRva && dwRva < pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
		{
			return dwRva - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
		}
	}

	return 0;
}

DWORD64 libTools::CLogExpressionPeLoader::RvaToOffsetX64(_In_ DWORD64 dwRva) CONST
{
	CONST IMAGE_NT_HEADERS64* pNtHeader = GetNtHeaderX64();
	CONST IMAGE_SECTION_HEADER* pSectionHeader = reinterpret_cast<CONST IMAGE_SECTION_HEADER *>(reinterpret_cast<CONST CHAR*>(pNtHeader) + sizeof(IMAGE_NT_HEADERS64));

	for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i, ++pSectionHeader)
	{
		if (pSectionHeader->VirtualAddress <= dwRva && dwRva < pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
		{
			return dwRva - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
		}
	}

	return 0;
}

BOOL libTools::CLogExpressionPeLoader::GetVecImportTableX86(_Out_ std::vector<ImportTable>& Vec) CONST
{
	CONST IMAGE_NT_HEADERS32* pNtHeader = GetNtHeaderX86();
	CONST IMAGE_IMPORT_DESCRIPTOR* pImportDescriptor = reinterpret_cast<CONST PIMAGE_IMPORT_DESCRIPTOR>(GetImageBase() + RvaToOffsetX86(pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));

	if (pImportDescriptor == nullptr)
	{
		LOG_C_E(L"pImportDescriptor = nullptr");
		return FALSE;
	}

	for (CONST IMAGE_IMPORT_DESCRIPTOR* pImportDesc = pImportDescriptor; pImportDesc->FirstThunk != 0; ++pImportDesc)
	{
		ImportTable ImportTable_;

		CONST CHAR* szName = reinterpret_cast<CONST CHAR*>(RvaToOffsetX86(pImportDesc->Name) + GetImageBase());
		if (szName == nullptr)
		{
			LOG_C_E(L"szName = nullptr");
			continue;
		}

		ImportTable_.wsDLLName = CCharacter::ASCIIToUnicode(std::string(szName));
		for (IMAGE_THUNK_DATA* pOriginThunk = reinterpret_cast<IMAGE_THUNK_DATA *>(RvaToOffsetX86(pImportDesc->OriginalFirstThunk) + GetImageBase()), *pRealThunk = reinterpret_cast<IMAGE_THUNK_DATA *>(RvaToOffsetX86(pImportDesc->FirstThunk) + GetImageBase()); pOriginThunk->u1.Function != NULL; pOriginThunk++, pRealThunk++)
		{
			ImportDLLContent DLLTable;
			if (pOriginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32)
			{
				DLLTable.wsAPIName = L"--";
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pImportName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(RvaToOffsetX86(pOriginThunk->u1.AddressOfData) + GetImageBase());
				DLLTable.wsAPIName = pImportName == nullptr ? L"--" : CCharacter::ASCIIToUnicode(std::string(pImportName->Name));
				DLLTable.dwFuncRVA = pRealThunk->u1.Function;
			}

			ImportTable_.VecTable.push_back(DLLTable);
		}
		Vec.push_back(ImportTable_);
	}
	return TRUE;
}

BOOL libTools::CLogExpressionPeLoader::GetVecImportTableX64(_Out_ std::vector<ImportTable>& Vec) CONST
{
	CONST IMAGE_NT_HEADERS64* pNtHeader = GetNtHeaderX64();
	CONST IMAGE_IMPORT_DESCRIPTOR* pImportDescriptor = reinterpret_cast<CONST PIMAGE_IMPORT_DESCRIPTOR>(GetImageBase() + RvaToOffsetX64(pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));

	if (pImportDescriptor == nullptr)
	{
		LOG_C_E(L"pImportDescriptor = nullptr");
		return FALSE;
	}

	for (CONST IMAGE_IMPORT_DESCRIPTOR* pImportDesc = pImportDescriptor; pImportDesc->FirstThunk != 0; ++pImportDesc)
	{
		ImportTable ImportTable_;

		CONST CHAR* szName = reinterpret_cast<CONST CHAR*>(RvaToOffsetX64(pImportDesc->Name) + GetImageBase());
		if (szName == nullptr)
		{
			LOG_C_E(L"szName = nullptr");
			continue;
		}

		ImportTable_.wsDLLName = CCharacter::ASCIIToUnicode(std::string(szName));
		for (IMAGE_THUNK_DATA64* pOriginThunk = reinterpret_cast<IMAGE_THUNK_DATA64 *>(RvaToOffsetX64(pImportDesc->OriginalFirstThunk) + GetImageBase()), *pRealThunk = reinterpret_cast<IMAGE_THUNK_DATA64 *>(RvaToOffsetX64(pImportDesc->FirstThunk) + GetImageBase()); pOriginThunk->u1.Function != NULL; pOriginThunk++, pRealThunk++)
		{
			ImportDLLContent DLLTable;
			if (pOriginThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64)
			{
				DLLTable.wsAPIName = L"--";
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pImportName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(RvaToOffsetX64(pOriginThunk->u1.AddressOfData) + GetImageBase());
				DLLTable.wsAPIName = pImportName == nullptr ? L"--" : CCharacter::ASCIIToUnicode(std::string(pImportName->Name));
				DLLTable.dwFuncRVA = pRealThunk->u1.Function;
			}

			ImportTable_.VecTable.push_back(DLLTable);
		}
		Vec.push_back(ImportTable_);
	}
	return TRUE;
}

BOOL libTools::CLogExpressionPeLoader::GetVecExportTableX86(_Out_ std::vector<ExportTable>& Vec) CONST
{
	CONST IMAGE_NT_HEADERS32*		pNtHeader = GetNtHeaderX86();
	CONST IMAGE_EXPORT_DIRECTORY*	pExportDirectory = reinterpret_cast<CONST IMAGE_EXPORT_DIRECTORY *>(GetImageBase() + RvaToOffsetX86(pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));


	// 地址表 EAT
	DWORD* pdwEAT = reinterpret_cast<DWORD*>(GetImageBase() + RvaToOffsetX86(pExportDirectory->AddressOfFunctions));
	DWORD* pdwENT = reinterpret_cast<DWORD*>(GetImageBase() + RvaToOffsetX86(pExportDirectory->AddressOfNames));
	WORD*  pdwEIT = reinterpret_cast<WORD*>(GetImageBase() + RvaToOffsetX86(pExportDirectory->AddressOfNameOrdinals));


	for (DWORD i = 0; i < pExportDirectory->NumberOfFunctions; ++i)
	{
		ExportTable ExportTable_;
		ExportTable_.dwOrdinal = pExportDirectory->Base + i;
		ExportTable_.dwMethodPtr = pdwEAT[i];
		ExportTable_.wsFunName = L"--";
		for (DWORD dwNameIndex = 0; dwNameIndex < pExportDirectory->NumberOfNames; ++dwNameIndex)
		{
			if (pdwEIT[dwNameIndex] == i)
			{
				CONST CHAR* szNamePtr = reinterpret_cast<CONST CHAR*>(RvaToOffsetX86(pdwENT[i]) + GetImageBase());
				if (szNamePtr != nullptr)
				{
					ExportTable_.wsFunName = CCharacter::ASCIIToUnicode(std::string(szNamePtr));
				}

				break;
			}
		}

		Vec.push_back(ExportTable_);
	}

	return TRUE;
}

BOOL libTools::CLogExpressionPeLoader::GetVecExportTableX64(_Out_ std::vector<ExportTable>& Vec) CONST
{
	CONST IMAGE_NT_HEADERS64*		pNtHeader = GetNtHeaderX64();
	CONST IMAGE_EXPORT_DIRECTORY*	pExportDirectory = reinterpret_cast<CONST IMAGE_EXPORT_DIRECTORY *>(GetImageBase() + RvaToOffsetX64(pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));


	// 地址表 EAT
	DWORD* pdwEAT = reinterpret_cast<DWORD*>(GetImageBase() + RvaToOffsetX64(pExportDirectory->AddressOfFunctions));
	DWORD* pdwENT = reinterpret_cast<DWORD*>(GetImageBase() + RvaToOffsetX64(pExportDirectory->AddressOfNames));
	WORD*  pdwEIT = reinterpret_cast<WORD*>(GetImageBase() + RvaToOffsetX64(pExportDirectory->AddressOfNameOrdinals));


	for (DWORD i = 0; i < pExportDirectory->NumberOfFunctions; ++i)
	{
		ExportTable ExportTable_;
		ExportTable_.dwOrdinal = pExportDirectory->Base + i;
		ExportTable_.dwMethodPtr = pdwEAT[i];
		ExportTable_.wsFunName = L"--";
		for (DWORD dwNameIndex = 0; dwNameIndex < pExportDirectory->NumberOfNames; ++dwNameIndex)
		{
			if (pdwEIT[dwNameIndex] == i)
			{
				CONST CHAR* szNamePtr = reinterpret_cast<CONST CHAR*>(RvaToOffsetX64(pdwENT[i]) + GetImageBase());
				if (szNamePtr != nullptr)
				{
					ExportTable_.wsFunName = CCharacter::ASCIIToUnicode(std::string(szNamePtr));
				}

				break;
			}
		}

		Vec.push_back(ExportTable_);
	}

	return TRUE;
}

#endif