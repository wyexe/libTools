#ifndef __LIBTOOLS_HOOKLIB_INLINEHOOK_LDRHEADER
#define __LIBTOOLS_HOOKLIB_INLINEHOOK_LDRHEADER

#pragma warning(disable:4005)
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>

namespace libTools
{
	class CLdrHeader
	{
	public:
		CLdrHeader();
		~CLdrHeader() = default;

		// InlineHook(需要Hook的函数地址,代替被Hook函数的地址,返回原始函数的入口地址)
		static BOOL WINAPI InlineHook(_In_ LPVOID OrgProc, _In_	LPVOID NewProc, _Out_ LPVOID* RealProc);

		// UnInlineHook(需要恢复Hook的函数地址,原始函数的入口地址)
		static VOID WINAPI UnInlineHook(_In_ LPVOID OrgProc, _In_ LPVOID RealProc);
	private:
		static BOOL WINAPI WriteReadOnlyMemory(_In_ LPBYTE	lpDest, _In_ LPBYTE	lpSource, _In_ ULONG	Length);

		static BOOL WINAPI GetPatchSize(_In_ LPVOID Proc, _In_	DWORD dwNeedSize, _Out_ LPDWORD lpPatchSize);
	};
}


#endif // !__LIBTOOLS_HOOKLIB_INLINEHOOK_LDRHEADER
