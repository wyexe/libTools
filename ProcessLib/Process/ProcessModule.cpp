#include "ProcessModule.h"
#include "psapi.h"

#pragma comment(lib,"Psapi.lib")
BOOL libTools::CProcessModule::GetProcessModule(_In_ HANDLE hProcess, _Out_ std::vector<ProcessModuleContent>& Vec)
{
	return QueryProcessModule(hProcess, [&Vec](CONST ProcessModuleContent& Instance)
	{
		Vec.push_back(Instance);
		return FALSE;
	});
}

BOOL libTools::CProcessModule::FindProcessModule(_In_ HANDLE hProcess, _In_ CONST std::wstring& wsModuleName, _Out_ ProcessModuleContent& Content)
{
	return QueryProcessModule(hProcess, [wsModuleName, &Content](CONST ProcessModuleContent& Instance)
	{
		if (Instance.wsModuleName == wsModuleName)
		{
			Content = Instance;
			return TRUE;
		}


		return FALSE;
	});
}

BOOL libTools::CProcessModule::QueryProcessModule(_In_ HANDLE hProcess, _In_ std::function<BOOL(CONST ProcessModuleContent&)> ActionPtr)
{
	HMODULE hMods[1024] = { 0 };
	DWORD   dwModsSize  = 0;


	if (!::EnumProcessModules(hProcess, hMods, sizeof(hMods), &dwModsSize))
		return FALSE;


	WCHAR wszText[MAX_PATH] = { 0 };
	for (DWORD i = 0;i < dwModsSize / sizeof(HMODULE); ++i)
	{
		ProcessModuleContent Instance;

		// Module Path
		::GetModuleFileNameExW(hProcess, hMods[i], wszText, _countof(wszText));
		Instance.wsModulePath = wszText;


		// Module Name
		::GetModuleBaseNameW(hProcess, hMods[i], wszText, _countof(wszText));
		Instance.wsModuleName = wszText;


		// Module Info
		MODULEINFO ModInfo;
		if (::GetModuleInformation(hProcess, hMods[i], &ModInfo, sizeof(ModInfo)))
		{
			Instance.dwImageBase = reinterpret_cast<DWORD>(ModInfo.lpBaseOfDll);
			Instance.dwImageSize = ModInfo.SizeOfImage;
		}


		// is searcher to break?
		if (ActionPtr(Instance))
		{
			break;
		}
	}

	return TRUE;
}
