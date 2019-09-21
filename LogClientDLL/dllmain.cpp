#include "CmdLogServer.h"
#include <include/CharacterLib/Character.h>
#include <include/LogLib/CmdLog.h>
#include <include/LogLib/Log.h>

#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"LogLib.lib")

static libTools::CCmdLog	g_CmdLog;
static CCmdLogServer		g_CmdLogServer;

extern "C" __declspec(dllexport) int WINAPIV WriteMsgToClient(_In_ LPCWSTR pwszClientName, _In_ LPCWSTR pwszContent, _Out_ LPWSTR pwszErrMsg)
{
	if (wcslen(pwszContent) >= 1024 || wcslen(pwszClientName) >= 32)
	{
		libTools::CCharacter::strcpy_my(pwszErrMsg, L"Buffer to long!");
		return 0;
	}

	std::wstring wsClientName(pwszClientName);
	std::wstring wsContent(pwszContent);

	if (libTools::CCharacter::MakeTextToLower(wsClientName) == L"local")
	{
		g_CmdLog.ExcuteLogServerCmd(wsContent);
	}

	g_CmdLogServer.SendContent(wsClientName, wsContent);
	return 1;
}

extern "C" __declspec(dllexport) void WINAPIV Release()
{
	libTools::CLog::GetInstance().Release();
	g_CmdLog.Stop();
	g_CmdLogServer.Stop();
}

BOOL APIENTRY DllMain(HMODULE,DWORD  ul_reason_for_call,LPVOID)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		libTools::CLog::GetInstance().SetClientName(L"local", libTools::CCharacter::MakeCurrentPath(L"\\").c_str());
		g_CmdLog.EnableGloablMethod();
		g_CmdLogServer.Run();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}

