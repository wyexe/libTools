#include "SocketIoEvent.h"

libTools::CSocketIoEvent::CSocketIoEvent(_In_ em_Type emType, _In_ CSocketRemoteClient* pSocketClient) : _emType(emType), _pSocketClient(pSocketClient)
{
	ZeroMemory(&_Overlapped, sizeof(_Overlapped));
	_ulCreateTick = ::GetTickCount64();
}

OVERLAPPED& libTools::CSocketIoEvent::GetOverlapped()
{
	return _Overlapped;
}

libTools::CSocketIoEvent::em_Type libTools::CSocketIoEvent::GetType() CONST
{
	return _emType;
}

libTools::CSocketRemoteClient* libTools::CSocketIoEvent::GetSocketClient()
{
	return _pSocketClient;
}

DWORD libTools::CSocketIoEvent::GetSpentTime() CONST
{
	return static_cast<DWORD>(::GetTickCount64() - _ulCreateTick);
}

std::wstring libTools::CSocketIoEvent::GetTypeText() CONST
{
	switch (_emType)
	{
	case em_Type::em_Type_Send:
		return L"Send";
	case em_Type::em_Type_Recv:
		return L"Recv";
	case em_Type::em_Type_Accept:
		return L"Accept";
	default:
		break;
	}

	return L"Unknow";
}

