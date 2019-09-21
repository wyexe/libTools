#ifndef __LIBTOOLS_HOOKLIB_INLINEHOOK_LASM_H__
#define __LIBTOOLS_HOOKLIB_INLINEHOOK_LASM_H__

#pragma warning(disable:4005)
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>

namespace libTools
{
#define OP_NONE           0x00
#define OP_MODRM          0x01
#define OP_DATA_I8        0x02
#define OP_DATA_I16       0x04
#define OP_DATA_I32       0x08
#define OP_DATA_PRE66_67  0x10
#define OP_WORD           0x20
#define OP_REL32          0x40
#ifdef _WIN64
#else
	class CDasm
	{
	public:

	public:
		CDasm() = default;
		~CDasm() = default;

		ULONG __fastcall SizeOfCode(_In_ LPVOID	Code, UCHAR **pOpcode) CONST;

		ULONG __fastcall SizeOfProc(_In_ LPVOID Proc) CONST;

		char __fastcall IsRelativeCmd(_In_ UCHAR *pOpcode) CONST;
	private:
		CONST static UCHAR OpcodeFlagsExt[256];

		CONST static UCHAR OpcodeFlags[256];
	};
#endif // _WIN64
}

#endif // !__LIBTOOLS_HOOKLIB_INLINEHOOK_LASM_H__
