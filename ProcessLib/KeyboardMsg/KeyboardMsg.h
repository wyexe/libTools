#ifndef __LIBTOOLS_PROCESSLIB_KEYBOARDMSG_KEYBOARDMSG_H__
#define __LIBTOOLS_PROCESSLIB_KEYBOARDMSG_KEYBOARDMSG_H__

#include <Windows.h>

namespace libTools
{
	class CKeyboardMsg
	{
	public:
		enum em_Custome_Key
		{
			VK_0			= 48,
			VK_1			= 49,
			VK_2			= 50,
			VK_3			= 51,
			VK_4			= 52,
			VK_5			= 53,
			VK_6			= 54,
			VK_7			= 55,
			VK_8			= 56,
			VK_9			= 57,
			VK_D			= 59,
			VK_J			= 58,
			VK_ARROW_TOP	= 38,
			VK_ARROW_LEFT	= 37,
			VK_ARROW_RIGHT	= 39,
			VK_ARROW_DOWN	= 40,
			VK_SHIFT_0		= 0x101,
			VK_SHIFT_1		= 0x102,
			VK_SHIFT_2		= 0x103,
			VK_SHIFT_3		= 0x104,
			VK_SHIFT_4		= 0x105,
			VK_SHIFT_5		= 0x106,
			VK_SHIFT_6		= 0x107,
			VK_SHIFT_7		= 0x108,
			VK_SHIFT_8		= 0x109,
			VK_SHIFT_9		= 0x10A,
			VK_SHIFT_D		= 0x10B,
			VK_SHIFT_J		= 0x10C,
		};
	public:
		CKeyboardMsg() = default;
		~CKeyboardMsg() = default;

		// ģ�ⰴ��
		static BOOL		SimulationKey(_In_  HWND hWnd, _In_ CHAR dwASCII, _In_opt_ DWORD dwTime = 10);

		// ��ȡ������
		static DWORD	GetVirKey(_In_ CHAR dwASCII);

		// ģ�����
		static BOOL		SimulationMouse(_In_ int x, _In_ int y);

		// ģ�����
		static BOOL		SimulationRightMouse(_In_ int x, _In_ int y);

		// �����ı�
		static BOOL		SendAscii(_In_ WCHAR data, _In_  BOOL shift);

		// �����ı�
		static BOOL		SendUnicode(_In_ WCHAR data);

		// ģ�ⰴ��
		static BOOL		SendKey(_In_ WORD wVk);

		// ģ�ⰴ��
		static BOOL		SendKey(_In_ WORD wVk, _In_ BOOL bPush);

		// �����ı�
		static BOOL		SendKeys(_In_ LPCWSTR data);

		// �����ı�
		static BOOL		SendKeys(_In_ LPCSTR data);
	private:

	};
}

#endif // !__LIBTOOLS_PROCESSLIB_KEYBOARDMSG_KEYBOARDMSG_H__
