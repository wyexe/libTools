#include "Exception.h"

namespace libTools
{
	std::function<VOID(CONST std::wstring&)> g_EchoExceptionMsgPtr = nullptr;
	VOID CException::InvokeExceptionPtr(_In_ LPCWSTR pwszFormat, ...)
	{
		if (g_EchoExceptionMsgPtr != nullptr)
		{
			va_list		args;
			WCHAR		szBuffer[1024];
			va_start(args, pwszFormat);
			_vsnwprintf_s(szBuffer, _countof(szBuffer) - 1, _TRUNCATE, pwszFormat, args);
			va_end(args);

			g_EchoExceptionMsgPtr(szBuffer);
		}
	}

	BOOL CException::PrintExceptionCode(_In_ LPEXCEPTION_POINTERS ExceptionPtr)
	{
		//LOG_CF_E(L"ExceptionCode=%X, EIP=%X, Addr=%X", ExceptionPtr->ExceptionRecord->ExceptionCode, ExceptionPtr->ContextRecord->Eip, ExceptionPtr->ExceptionRecord->ExceptionAddress);
		switch (ExceptionPtr->ExceptionRecord->ExceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			InvokeExceptionPtr(L" �ڴ��ַ�Ƿ������쳣!");
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			InvokeExceptionPtr(L" ��������δ�����쳣 ");
			break;
		case EXCEPTION_BREAKPOINT:
			InvokeExceptionPtr(L" �ж��쳣!");
			break;
		case EXCEPTION_SINGLE_STEP:
			InvokeExceptionPtr(L" �����ж��쳣 ");
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			InvokeExceptionPtr(L" ����Խ�� ");
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: case EXCEPTION_INT_DIVIDE_BY_ZERO:
			InvokeExceptionPtr(L" ���� 0 �쳣!");
			break;
		case EXCEPTION_INT_OVERFLOW: case EXCEPTION_FLT_OVERFLOW:
			InvokeExceptionPtr(L" ��������쳣 ");
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			InvokeExceptionPtr(L" �����������쳣 ");
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			InvokeExceptionPtr(L" ҳ�����쳣 ");
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			InvokeExceptionPtr(L" �Ƿ�ָ���쳣 ");
			break;
		case EXCEPTION_STACK_OVERFLOW:
			InvokeExceptionPtr(L" ��ջ���!");
			break;
		case EXCEPTION_INVALID_HANDLE:
			InvokeExceptionPtr(L" ��Ч����쳣!");
			break;
		default:
			break;
		}
		return TRUE; // return TRUE ��ʾ�쳣������, �����������һ����!
	}

}