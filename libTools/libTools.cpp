// libTools.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <tlhelp32.h>
#include <sstream>
#include <iterator>
#include <include/CharacterLib/Character.h>
#include <include/ProcessLib/Common/ResHandleManager.h>
#include <VersionHelpers.h>
#include <locale>
#include <include/AlgroithmLib/Encrypt/RC4.h>
#include <codecvt>
#include <filesystem>
#include <include/AlgroithmLib/Encrypt/MD5.h>
#include <include/HookLib/InlineHook/InlineHook.h>
#include <include/FileLib/File.h>
#include <include/InjectorLib/ModuleInjector/NoTraceModuleInjector.h>



#pragma comment(lib,"FileLib_Debug.lib")
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"ProcessLib_Debug.lib")
#pragma comment(lib,"ProcessLib_Debug.lib")
#pragma comment(lib,"HookLib_Debug.lib")
#pragma comment(lib,"InjectorLib_Debug.lib")
#pragma comment(lib,"user32.lib")
#define _SELF L"asd"

int main()
{
	std::wcout << sizeof(ULONGLONG);
	system("pause");
    return 0;
}

