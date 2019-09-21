#include "AntiRootkit.h"

BOOL libTools::CAntiRootkit::IsAntiLoader()
{
#ifdef _WIN64
	return FALSE;
#else
	BOOL bRetCode = FALSE;
	__asm
	{
		PUSHAD;
		MOV EAX, DWORD PTR FS : [0x30];

		XOR EBX, EBX;
		MOVZX EBX, BYTE PTR DS : [EAX + 0x2];
		MOV bRetCode, EBX;
		POPAD;
	}
	return bRetCode;
#endif // _WIN64
}

BOOL libTools::CAntiRootkit::IsExistDebugger()
{
	return ::IsDebuggerPresent();
}
