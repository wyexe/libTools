#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include "SocketBaseClientService.h"
#include <WS2tcpip.h> // getaddrinfo
#include <MSWSock.h>  // WSAIoctl
#include <include/LogLib/Log.h>
#include <include/ExceptionLib/Exception.h>
#include <include/SocketCommon/SocketBuffer.h>
#include <include/SocketCommon/SocketIoEvent.h>
#include <include/SocketCommon/SocketRemoteClient.h>
#include <include/CharacterLib/Character.h>
#include <include/TimeLib/TimeTick.h>

#ifdef _DEBUG
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"SocketCommonLib_Debug.lib")
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"TimeLib_Debug.lib")
#pragma comment(lib,"ExceptionLib_Debug.lib")
#else
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"SocketCommonLib.lib")
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"TimeLib.lib")
#pragma comment(lib,"ExceptionLib.lib")
#endif // _DEBUG

#pragma comment(lib,"Ws2_32.lib")

#define _SELF L"SocketBaseClientService.cpp"
libTools::CSocketBaseClientService::CSocketBaseClientService() :_ServerSocket(INVALID_SOCKET), _pServerTpIo(nullptr), _dwRecvSize(NULL)
{
	ZeroMemory(_RecvBuffer, sizeof(_RecvBuffer));
	_ExistPostRecv = FALSE;
}

libTools::CSocketBaseClientService::~CSocketBaseClientService()
{
	Stop();
}

BOOL libTools::CSocketBaseClientService::Run(_In_ CONST std::wstring& wsIp, _In_ SHORT Port, _In_ DWORD dwMaxTimeout)
{
	_dwMaxTimeout = dwMaxTimeout;
	_wsIp = wsIp;
	_Port = Port;

	if (!CreateServerSocket())
		return FALSE;

	bool bReuseAddr = true;
	::setsockopt(_ServerSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<CHAR*>(&bReuseAddr), sizeof(bReuseAddr));

	_pServerTpIo = ::CreateThreadpoolIo(reinterpret_cast<HANDLE>(_ServerSocket), IoCompletionCallback, this, NULL);
	if (_pServerTpIo == nullptr)
	{
		LOG_CF_E(L"CreateThreadpoolIo = nullptr!");
		Stop();
		return FALSE;
	}

	return PostConnect();
}

VOID libTools::CSocketBaseClientService::Stop()
{
	_bConnect = FALSE;
	if (_ServerSocket != INVALID_SOCKET)
	{
		::shutdown(_ServerSocket, SD_BOTH);
		::closesocket(_ServerSocket);
		_ServerSocket = INVALID_SOCKET;
	}
	if (_pServerTpIo != nullptr)
	{
		::WaitForThreadpoolIoCallbacks(_pServerTpIo, TRUE);
		::CancelThreadpoolIo(_pServerTpIo);
		_pServerTpIo = nullptr;
	}
}

VOID CALLBACK libTools::CSocketBaseClientService::IoCompletionCallback(PTP_CALLBACK_INSTANCE , PVOID Context, PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred, PTP_IO)
{
	CException::InvokeAction(__FUNCTIONW__, [=]
	{
		auto pSocketBaseClientService = reinterpret_cast<CSocketBaseClientService *>(Context);
		CSocketIoEvent* pIoEvent = CONTAINING_RECORD(Overlapped, CSocketIoEvent, GetOverlapped());
		if (IoResult == NO_ERROR)
		{
			switch (pIoEvent->GetType())
			{
			case CSocketIoEvent::em_Type::em_Type_Accept:
				pSocketBaseClientService->OnConnect();
				break;
			case CSocketIoEvent::em_Type::em_Type_Recv:
				if (NumberOfBytesTransferred > 0)
					pSocketBaseClientService->OnRecv(static_cast<DWORD>(NumberOfBytesTransferred));
				else
				{
					LOG_CF_E(L"NumberOfBytesTransferred = 0, OnClose");
					pSocketBaseClientService->OnClose();
				}
				break;
			case CSocketIoEvent::em_Type::em_Type_Send:
				pSocketBaseClientService->OnSend(pIoEvent, NumberOfBytesTransferred);
				break;
			default:
				break;
			}
		}
		else
		{
			switch (IoResult)
			{
			case ERROR_SEM_TIMEOUT:
				if (pIoEvent->GetType() == CSocketIoEvent::em_Type::em_Type_Recv && !pSocketBaseClientService->_ExistPostRecv)
				{
					pSocketBaseClientService->PostRecv();
				}
				break;
			default:
				LOG_CF_E(L"I/O operation failed. Err=%d", IoResult);
				pSocketBaseClientService->OnClose();
				break;
			}
		}

		delete pIoEvent;
	});
}

BOOL libTools::CSocketBaseClientService::PostConnect()
{
	// Get Address Info
	addrinfo hints = { 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0;

	char portStr[32] = { 0 };
	sprintf_s(portStr, sizeof(portStr), "%d", _Port);

	// First Param was IP, if IP = NULL, pAddrinfo.IP = INADDR_ANY
	addrinfo* pAddrinfo = nullptr;
	if (::getaddrinfo(CCharacter::UnicodeToASCII(_wsIp).c_str(), portStr, &hints, &pAddrinfo) != 0)
	{
		LOG_CF_E(L"getaddrinfo = NULL, Err=%d", ::WSAGetLastError());
		return FALSE;
	}

	auto pIoEvent = new CSocketIoEvent(CSocketIoEvent::em_Type::em_Type_Accept, nullptr);
	pIoEvent->SetTag(this);

	// Find Useful Socket and Port since Param 'Port'
	for (auto pAddr = pAddrinfo; pAddr != nullptr; pAddr = pAddr->ai_next)
	{
		::StartThreadpoolIo(_pServerTpIo);
		if (!ConnectEx(pAddr->ai_addr, pAddr->ai_addrlen, &pIoEvent->GetOverlapped()))
		{
			::freeaddrinfo(pAddrinfo);
			const auto Error = ::WSAGetLastError();
			if (Error != ERROR_IO_PENDING)
			{
				LOG_CF_E(L"ConnectEx Faild! Err=%d", Error);
				continue;
			}

			return WaitToConnect(_dwMaxTimeout);
		}
	}

	::freeaddrinfo(pAddrinfo);
	return FALSE;
}

VOID libTools::CSocketBaseClientService::OnConnect()
{
	_bConnect = TRUE;
	LOG_C_D(L"成功连接上服务器!");
}

VOID libTools::CSocketBaseClientService::OnClose()
{
	LOG_C_D(L"CSocketBaseClientService::OnClose");
	Stop();
	DisConnectServer();
}

VOID libTools::CSocketBaseClientService::OnRecv(_In_ DWORD NumberOfBytesTransferred)
{
	CException::InvokeAction(__FUNCTIONW__, [&]
	{
		if (_dwRecvSize == 0 && NumberOfBytesTransferred < 4)
		{
			// 没接收完
			SetBuffer(_RecvBuffer, NumberOfBytesTransferred);
			PostRecv();
		}
		else
		{
			if (_dwRecvSize == 0)
			{
				// Set Packet MaxSize
				CSocketBuffer::SetValue_By_Buffer(_dwRecvSize, reinterpret_cast<BYTE*>(_RecvBuffer));
			}


			CHAR* Buffer = _RecvBuffer;
			while (NumberOfBytesTransferred + _VecBuffer.size() >= _dwRecvSize)
			{
				const auto PacketSize = static_cast<int>(_dwRecvSize - _VecBuffer.size());
				if (PacketSize < 0 || PacketSize > 1024)
					LOG_MSG_CF(L"PacketSize=%d", PacketSize);


				// Tranfer Data to _VecBuffer
				CSocketBuffer SocketBuffer;
				SetBuffer(Buffer, PacketSize);


				// Tranfer _VecBuffer Data to SocketBuffer
				SocketBuffer.SetDataPtr(_VecBuffer.data(), _VecBuffer.size());
				_VecBuffer.clear();


				Buffer += PacketSize;
				CException::InvokeAction(L"ExcuteAsync.EchoPacket", [&]
				{
					EchoPacket(SocketBuffer);
					SocketBuffer.clear();
				});



				NumberOfBytesTransferred -= PacketSize;
				if (NumberOfBytesTransferred >= 4)
				{
					// Set Packet MaxSize
					CSocketBuffer::SetValue_By_Buffer(_dwRecvSize, reinterpret_cast<BYTE*>(_RecvBuffer));
				}
				else
				{
					NumberOfBytesTransferred = NULL;
					_dwRecvSize = 0;
					break;
				}
			}

			if (static_cast<int>(NumberOfBytesTransferred) > 0)
			{
				// Save Surplus Data(_RecvBuffer) to _VecBuffer
				SetBuffer(Buffer, NumberOfBytesTransferred);
				ZeroMemory(_RecvBuffer, sizeof(_RecvBuffer));


				// Wait to Next Recv
				PostRecv();
			}
		}

	});
}

VOID libTools::CSocketBaseClientService::OnSend(_In_ CSocketIoEvent* pIoEvent, _In_ UINT NumberOfBytesTransferred)
{
	SocketSendBuffer* pSendBuffer = pIoEvent->GetTag<SocketSendBuffer>();
	if (pSendBuffer != nullptr)
	{
		pSendBuffer->uCurLength += NumberOfBytesTransferred;
		if (pSendBuffer->uCurLength >= pSendBuffer->uMaxLength)
		{
			delete[] pSendBuffer->Buffer;
			pSendBuffer->Buffer = nullptr;

			delete pSendBuffer;
			pSendBuffer = nullptr;

			pIoEvent->SetTag<SocketSendBuffer>(nullptr);
		}
	}
}

VOID libTools::CSocketBaseClientService::PostRecv()
{
	CException::InvokeAction(__FUNCTIONW__, [&]
	{
		WSABUF recvBufferDescriptor;
		recvBufferDescriptor.buf = _RecvBuffer;
		recvBufferDescriptor.len = em_Buffer_Length;

		::StartThreadpoolIo(_pServerTpIo);
		auto pIoEvent = new CSocketIoEvent(CSocketIoEvent::em_Type::em_Type_Recv, nullptr);
		DWORD dwNumberOfBytes, dwRecvFlags = 0;

		_ExistPostRecv = TRUE;
		if (::WSARecv(_ServerSocket, &recvBufferDescriptor, 1, &dwNumberOfBytes, &dwRecvFlags, &pIoEvent->GetOverlapped(), NULL) == SOCKET_ERROR)
		{
			const auto Error = ::WSAGetLastError();
			if (Error != ERROR_IO_PENDING)
			{
				LOG_CF_E(L"WSARecv Faild. Err=%d", Error);
				OnClose();

				_ExistPostRecv = FALSE;
				delete pIoEvent;
				pIoEvent = nullptr;
			}
		}
	});
}

VOID libTools::CSocketBaseClientService::Clear()
{
	std::lock_guard<std::mutex> _Lock(_MtxRecv);
	_VecBuffer.clear();
	ZeroMemory(_RecvBuffer, sizeof(_RecvBuffer));
	_dwRecvSize = NULL;
}

BOOL libTools::CSocketBaseClientService::CreateServerSocket()
{
	WSADATA wd = { 0 };
	WSAStartup(WINSOCK_VERSION, &wd);

	// Get Address Info
	addrinfo hints = { 0 };
	ZeroMemory(&hints, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	// First Param was IP, if IP = NULL, pAddrinfo.IP = INADDR_ANY
	struct addrinfo* pAddrinfo = nullptr;
	if (getaddrinfo(NULL, "0", &hints, &pAddrinfo) != 0)
	{
		LOG_CF_E(L"getaddrinfo = NULL, Err=%d", ::WSAGetLastError());
		return FALSE;
	}

	// Find Useful Socket and Port since Param 'Port'
	for (auto pAddr = pAddrinfo; pAddr != nullptr; pAddr = pAddr->ai_next)
	{
		// Get Overlapped Socket!
		auto Sock = WSASocketW(pAddr->ai_family, pAddr->ai_socktype, pAddr->ai_protocol, NULL, NULL, WSA_FLAG_OVERLAPPED);
		if (Sock == INVALID_SOCKET)
		{
			LOG_CF_E(L"WSASocketW = INVALID_SOCKET, Port:[%d] Couldn't Useful, FindNext useful Port!", static_cast<int>(reinterpret_cast<sockaddr_in*>(pAddr->ai_addr)->sin_port));
			continue;
		}

		if (::bind(Sock, pAddr->ai_addr, static_cast<int>(pAddr->ai_addrlen)) == SOCKET_ERROR)
		{
			LOG_CF_E(L"bind Port Faild!, Port=[%d], Err=%d", static_cast<int>(reinterpret_cast<sockaddr_in*>(pAddr->ai_addr)->sin_port), ::WSAGetLastError());
			break;
		}

		::freeaddrinfo(pAddrinfo);
		_ServerSocket = Sock;
		return TRUE;
	}

	::freeaddrinfo(pAddrinfo);
	LOG_CF_E(L"UnExist useful Socket");
	return FALSE;
}

BOOL libTools::CSocketBaseClientService::ConnectEx(_In_ sockaddr* pSockaddr, _In_ std::size_t uAddrLen, _Out_ LPOVERLAPPED lpOverlapped) CONST
{
	static LPFN_CONNECTEX ConnectEx_ = nullptr;
	if (ConnectEx_ == nullptr)
	{
		DWORD dwBytes = 0;
		GUID guidConnectEx = WSAID_CONNECTEX;
		if (WSAIoctl(_ServerSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), &ConnectEx_, sizeof(ConnectEx_), &dwBytes, 0, 0) == SOCKET_ERROR)
		{
			LOG_CF_E(L"WSAIoctl() to get ConnectEx() failed, Err=%d", WSAGetLastError());
			return FALSE;
		}
	}

	return ConnectEx_(_ServerSocket, pSockaddr, static_cast<int>(uAddrLen), NULL, NULL, NULL, lpOverlapped);
}

BOOL libTools::CSocketBaseClientService::WaitToConnect(_In_ DWORD dwMaxTimeOut)
{
	CTimeTick TimeTick;
	while (static_cast<DWORD>(TimeTick.GetSpentTime(CTimeTick::em_TimeTick::em_TimeTick_Millisecond)) < dwMaxTimeOut)
	{
		if (IsConnect())
		{
			return TRUE;
		}
		::Sleep(100);
	}
	return FALSE;
}

VOID libTools::CSocketBaseClientService::SetBuffer(_In_ CONST CHAR* Buffer, _In_ DWORD NumberOfBytesTransferred)
{
	for (DWORD i = 0; i < NumberOfBytesTransferred; ++i)
	{
		_VecBuffer.push_back(Buffer[i]);
	}
}

BOOL libTools::CSocketBaseClientService::PostSend(_In_ CSocketBuffer& SocketBuffer)
{
	UINT uSize = 0;
	std::shared_ptr<CHAR> DataPtr = SocketBuffer.GetDataPtr(uSize);


	SocketSendBuffer* pSendBuffer = new SocketSendBuffer();
	if (pSendBuffer == nullptr)
	{
		LOG_CF_E(L"pSendBuffer = nullptr");
		OnClose();
		return FALSE;
	}


	pSendBuffer->Buffer = new CHAR[uSize];
	pSendBuffer->uMaxLength = uSize;
	pSendBuffer->uCurLength = 0;
	if (pSendBuffer->Buffer == nullptr)
	{
		LOG_CF_E(L"pSendBuffer->Buffer = nullptr");
		OnClose();
		return FALSE;
	}


	memcpy(pSendBuffer->Buffer, DataPtr.get(), uSize);
	PostRecv();
	return PostSend(pSendBuffer);
}

BOOL libTools::CSocketBaseClientService::PostSend(_In_ SocketSendBuffer* pSendSocket)
{
	auto pIoEvent = new CSocketIoEvent(CSocketIoEvent::em_Type::em_Type_Send, nullptr);
	pIoEvent->SetTag<SocketSendBuffer>(pSendSocket);


	WSABUF WSABuffer;
	WSABuffer.buf = pSendSocket->Buffer + pSendSocket->uCurLength;
	WSABuffer.len = pSendSocket->uMaxLength - pSendSocket->uCurLength;

	::StartThreadpoolIo(_pServerTpIo);
	if (::WSASend(_ServerSocket, &WSABuffer, 1, NULL, 0, &pIoEvent->GetOverlapped(), NULL) == SOCKET_ERROR)
	{
		int Error = ::WSAGetLastError();
		if (Error != ERROR_IO_PENDING)
		{
			LOG_CF_E(L"WSASend Err=%d", Error);
			OnClose();
			delete pIoEvent;
			return FALSE;
		}
	}

	return TRUE;
}

bool libTools::CSocketBaseClientService::IsConnect() CONST
{
	return _bConnect;
}
