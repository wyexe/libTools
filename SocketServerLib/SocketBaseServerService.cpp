#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "SocketBaseServerService.h"
#include <mswsock.h>	// SO_UPDATE_ACCEPT_CONTEXT
#include <Ws2tcpip.h>	// getaddrinfo
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


#define _SELF L"SocketBaseServerService.cpp"
libTools::CSocketBaseServerService::ServerThreadContent* libTools::CSocketBaseServerService::ServerThreadContent::Create(_In_ CSocketBaseServerService* pServer_, _In_ LPVOID Reserve_ /*= nullptr*/)
{
	ServerThreadContent* pContent = new ServerThreadContent;
	if (pContent == nullptr)
	{
		LOG_MSG_CF(L"new ServerThreadContent = nullptr");
		return nullptr;
	}

	pContent->pServer = pServer_;
	pContent->Reserve = Reserve_;
	return pContent;
}

libTools::CSocketBaseServerService::CSocketBaseServerService() : 
_ServerSocket(INVALID_SOCKET),
_pServerTpIo(nullptr),
_uMaxClientCount(NULL),
_pClent_Tp_Ggoup(nullptr),
_bRun(FALSE),
_uAccpetCount(0),
_hClearThread(NULL)
{
	ZeroMemory(&_Client_Tp_Env, sizeof(_Client_Tp_Env));

	::InitializeCriticalSection(&_VecSocketClientLock);
}

libTools::CSocketBaseServerService::~CSocketBaseServerService()
{
	StopServer();
}

BOOL libTools::CSocketBaseServerService::RunServer(_In_ SHORT shPort, _In_ UINT uMaxAccept)
{
	WSADATA wd = { 0 };
	WSAStartup(WINSOCK_VERSION, &wd);
	//
	_uMaxClientCount = uMaxAccept;

	// Create Thread Pool Env
	::InitializeThreadpoolEnvironment(&_Client_Tp_Env);

	// Create Cleanup Group for shutdown Thread Pool
	_pClent_Tp_Ggoup = ::CreateThreadpoolCleanupGroup();
	if (_pClent_Tp_Ggoup == nullptr)
	{
		LOG_CF_E(L"CreateThreadpoolCleanupGroup = nullptr!");
		StopServer();
		return FALSE;
	}

	// Set Cleanup Group to Thread Pool Env
	::SetThreadpoolCallbackCleanupGroup(&_Client_Tp_Env, _pClent_Tp_Ggoup, nullptr);

	// Create Socket!
	if (!CreateServerSocket(shPort))
	{
		LOG_CF_E(L"CreateServerSocket = INVALID_SOCKET!");
		StopServer();
		return FALSE;
	}

	// Set Refuse Addr to Repeat Useable
	//bool bReuseAddr = true;
	//::setsockopt(_ServerSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<CONST CHAR*>(&bReuseAddr), sizeof(bReuseAddr));


	// Create Thread Pool to Socket IO
	_pServerTpIo = ::CreateThreadpoolIo(reinterpret_cast<HANDLE>(_ServerSocket), IoCompletionCallback, this, NULL);
	if (_pServerTpIo == nullptr)
	{
		LOG_CF_E(L"CreateThreadpoolIo = nullptr!");
		StopServer();
		return FALSE;
	}

	// Run Thread Pool Io && listen Server Socket 
	::StartThreadpoolIo(_pServerTpIo);
	if (::listen(_ServerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		LOG_CF_E(L"listen = SOCKET_ERROR, Err=%d", ::WSAGetLastError());
		StopServer();
		return FALSE;
	}

	// Post First Accept 
	_uAccpetCount += 1;
	PostAccept();

	// Run Thread Pool Worker
	_bRun = TRUE;
	_hClearThread = ::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)_ClearThread, this, NULL, NULL);
	return TRUE;
}

VOID libTools::CSocketBaseServerService::StopServer()
{
	_bRun = FALSE;

	if (_ServerSocket != INVALID_SOCKET)
	{
		LOG_CF_D(L"Close Socket");
		::closesocket(_ServerSocket);
		::CancelIoEx(reinterpret_cast<HANDLE>(_ServerSocket), NULL);
		_ServerSocket = INVALID_SOCKET;
	}

	if (_pServerTpIo != nullptr)
	{
		LOG_CF_D(L"Wait 线程池IO回调执行完毕");
		::WaitForThreadpoolIoCallbacks(_pServerTpIo, TRUE);

		LOG_CF_D(L"Close 线程池IO");
		::CloseThreadpoolIo(_pServerTpIo);
		_pServerTpIo = nullptr;
	}

	if (_pClent_Tp_Ggoup != nullptr)
	{
		LOG_CF_D(L"Close 线程池清理组");
		::CloseThreadpoolCleanupGroupMembers(_pClent_Tp_Ggoup, FALSE, NULL);
		::CloseThreadpoolCleanupGroup(_pClent_Tp_Ggoup);
		_pClent_Tp_Ggoup = NULL;
	}

	if (_Client_Tp_Env.Size != NULL)
	{
		LOG_CF_D(L"Close 线程池环境!");
		::DestroyThreadpoolEnvironment(&_Client_Tp_Env);
		ZeroMemory(&_Client_Tp_Env, sizeof(_Client_Tp_Env));
	}

	if (_hClearThread != NULL)
	{
		::WaitForSingleObject(_hClearThread, INFINITE);
		::CloseHandle(_hClearThread);
		_hClearThread = NULL;


		::DeleteCriticalSection(&_VecSocketClientLock);
		WSACleanup();
		LOG_CF_D(L"Release 所有客户端!");
	}
}

VOID CALLBACK libTools::CSocketBaseServerService::IoCompletionCallback(PTP_CALLBACK_INSTANCE , PVOID Context, PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred, PTP_IO )
{
	CException::InvokeAction(__FUNCTIONW__, [&]
	{
		CSocketBaseServerService* pSocketBaseServerService = static_cast<CSocketBaseServerService *>(Context);
		CSocketIoEvent* pIoEvent = CONTAINING_RECORD(Overlapped, CSocketIoEvent, GetOverlapped());


		pIoEvent->GetSocketClient()->SubRefCount();
		if (IoResult != ERROR_SUCCESS)
		{
			switch (IoResult)
			{
			case ERROR_NETNAME_DELETED: // Client Close Socket~
				LOG_C_D(L"ERROR_NETNAME_DELETED");
				pSocketBaseServerService->OnClose(pIoEvent->GetSocketClient());
				break;
			case ERROR_SEM_TIMEOUT:
				if (pIoEvent->GetType() == CSocketIoEvent::em_Type::em_Type_Recv && !pIoEvent->GetSocketClient()->ExistPostRecv())
				{
					pSocketBaseServerService->PostRecv(pIoEvent->GetSocketClient());
				}
				break;
			default:
				LOG_CF_E(L"IoCompletionCallback.IoResult=%d, IO operation Faild!, Socket=[%X], Type=%s,NumberOfBytesTransferred=%d", IoResult, pIoEvent->GetSocketClient()->GetSocket(),
					pIoEvent->GetTypeText().c_str(),
					NumberOfBytesTransferred);

				pSocketBaseServerService->OnClose(pIoEvent->GetSocketClient());
				break;
			}

		}
		else
		{
			switch (pIoEvent->GetType())
			{
			case CSocketIoEvent::em_Type::em_Type_Accept:
				// New Client Connected
				pSocketBaseServerService->OnAccept(pIoEvent);
				break;
			case CSocketIoEvent::em_Type::em_Type_Recv:
				if (NumberOfBytesTransferred == 0)
				{
					// Release Socket...
					//LOG_C_E(L"NumberOfBytesTransferred = 0");
					pSocketBaseServerService->OnClose(pIoEvent->GetSocketClient());
				}
				else
				{
					pIoEvent->GetSocketClient()->SetExistPostRecv(FALSE);
					if (!pSocketBaseServerService->OnRecv(pIoEvent, static_cast<int>(NumberOfBytesTransferred)))
					{
						LOG_CF_E(L"OnRecv = FALSE;");
						pSocketBaseServerService->OnClose(pIoEvent->GetSocketClient());
					}
				}
				break;
			case CSocketIoEvent::em_Type::em_Type_Send:
				pSocketBaseServerService->OnSend(pIoEvent, NumberOfBytesTransferred);
				break;
			default:
				break;
			}
		}


		delete pIoEvent;
		pIoEvent = nullptr;
	});
}

DWORD WINAPI libTools::CSocketBaseServerService::_ClearThread(_In_ LPVOID lpParam)
{
	CSocketBaseServerService* pSocketBaseServerService = reinterpret_cast<CSocketBaseServerService *>(lpParam);
	while (pSocketBaseServerService->_bRun)
	{
		pSocketBaseServerService->InvokeVecSocketClient([=] 
		{
			for (auto itr = pSocketBaseServerService->_VecSocketClient.begin(); itr != pSocketBaseServerService->_VecSocketClient.end(); itr++)
			{
				// 是否被使用了(是否该socket被Client使用了)
				CONST auto& itm = *itr;
				if (itm->IsOnLine() && itm->IsKeepALiveTimeout())
				{
					LOG_C_E(L"Timeout ->  DisConnect");
					itm->DisConnect();
				}
			}
		});
		::Sleep(1000);
	}
	return 0;
}

VOID libTools::CSocketBaseServerService::PostAccept()
{
	SOCKET ClientSocket = CreateClientSocket();
	if (ClientSocket == INVALID_SOCKET)
	{
		LOG_MSG_CF(L"CreateClientSocket = INVALID_SOCKET");
		return;
	}

	CSocketRemoteClient* pSocketClient = CreateNewSocketClient(ClientSocket);
	if (pSocketClient == nullptr)
	{
		LOG_MSG_CF(L"CreateNewSocketClient = nullptr!");
		return;
	}

	InvokeVecSocketClient([this, pSocketClient]
	{
		_VecSocketClient.insert(pSocketClient);
	});

	// Set CLServer*
	pSocketClient->SetTag(this);

	// malloc 
	auto pIoEvent = new CSocketIoEvent(CSocketIoEvent::em_Type::em_Type_Accept, pSocketClient);
	if (pIoEvent == nullptr)
	{
		LOG_MSG_CF(L"Alloc pIoEvent = nullptr in PostAccept");
		return;
	}



	::StartThreadpoolIo(_pServerTpIo);
	if (AcceptEx(pSocketClient, &pIoEvent->GetOverlapped()) == FALSE) // Wait to Client Connect to wake up 'IoCompletionCallback'
	{
		auto Error = ::WSAGetLastError();
		if (Error != ERROR_IO_PENDING)
		{
			LOG_MSG_CF(L"Post AcceptEx Faild!, Error=%d", Error);
			return;
		}
	}
}

BOOL libTools::CSocketBaseServerService::PostRecv(_In_ CSocketRemoteClient* pSocketClient)
{
	return !pSocketClient->ExistPostRecv() && CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		WSABUF WSABuffer;
		WSABuffer.buf = reinterpret_cast<CHAR*>(pSocketClient->GetRecvBuffer());
		WSABuffer.len = CSocketRemoteClient::em_Buffer_Len - 1;

		DWORD dwNumberOfBytes = 0, dwFlag = 0;

		auto pIoEvent = new CSocketIoEvent(CSocketIoEvent::em_Type::em_Type_Recv, pSocketClient);
		if (pIoEvent == nullptr)
		{
			LOG_MSG_CF(L"Alloc pIoEvent = nullptr in PostRecv");
			return FALSE;
		}


		pSocketClient->AddRefCount();
		::StartThreadpoolIo(pSocketClient->GetTpIo());

		pSocketClient->SetExistPostRecv(TRUE);
		if (::WSARecv(pSocketClient->GetSocket(), &WSABuffer, 1, &dwNumberOfBytes, &dwFlag, &pIoEvent->GetOverlapped(), NULL) == SOCKET_ERROR)
		{
			int Error = ::WSAGetLastError();
			if (Error != ERROR_IO_PENDING) // socket was closed
			{
				LOG_CF_E(L"WSARecv Err=%d", Error);
				pSocketClient->SubRefCount();
				pSocketClient->SetExistPostRecv(FALSE);

				delete pIoEvent;
				pIoEvent = nullptr;

				return FALSE;
			}
		}

		return TRUE;
	});
}

BOOL libTools::CSocketBaseServerService::PostSend(_In_ CSocketRemoteClient* pSocketClient, _In_ CSocketBuffer* pSocketBuffer)
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		UINT uSize = 0;
		std::shared_ptr<CHAR> DataPtr = pSocketBuffer->GetDataPtr(uSize);

		SocketSendBuffer * pSendBuffer = new SocketSendBuffer;
		pSendBuffer->Buffer = new CHAR[uSize];
		pSendBuffer->CurLength = 0;
		pSendBuffer->MaxLength = uSize;
		if (pSendBuffer->Buffer == nullptr)
		{
			LOG_MSG_CF(L"Alloc Buffer = nullptr in PostSend");
			return FALSE;
		}


		memcpy(pSendBuffer->Buffer, DataPtr.get(), uSize);
		// delete SocketBuffer

		WSABUF WSABuffer;
		WSABuffer.buf = pSendBuffer->Buffer;
		WSABuffer.len = uSize;


		auto pIoEvent = new CSocketIoEvent(CSocketIoEvent::em_Type::em_Type_Send, pSocketClient);
		if (pIoEvent == nullptr)
		{
			LOG_MSG_CF(L"Alloc pIoEvent = nullptr in PostSend");
			return FALSE;
		}
		pIoEvent->SetTag(pSendBuffer);
		pSocketClient->AddRefCount();

		DWORD dwSendLength = 0;
		::StartThreadpoolIo(pSocketClient->GetTpIo());
		if (::WSASend(pSocketClient->GetSocket(), &WSABuffer, 1, &dwSendLength, 0, &pIoEvent->GetOverlapped(), NULL) == SOCKET_ERROR)
		{
			int Error = ::WSAGetLastError();
			if (Error != ERROR_IO_PENDING)
			{
				LOG_CF_E(L"WSASend Err=%d", Error);
				//OnClose(pIoEvent->GetSocketClient());
				pSocketClient->SubRefCount();

				// Release ....
				delete pIoEvent;
				pIoEvent = nullptr;

				delete[] pSendBuffer->Buffer;
				pSendBuffer->Buffer = nullptr;

				delete pSendBuffer;
				pSendBuffer = nullptr;
				return FALSE;
			}
		}

		return TRUE;
	});
}

VOID libTools::CSocketBaseServerService::OnAccept(_In_ CSocketIoEvent* pSocketIoEvent)
{
	CException::InvokeAction(__FUNCTIONW__, [&]
	{
		AddClient(pSocketIoEvent->GetSocketClient());

		_uAccpetCount -= 1;
		if (_uAccpetCount <= 3)
		{
			// Keep Accpet Socket Count Probably = 10
			UINT uCount = 10 - _uAccpetCount;
			for (UINT i = 0; i < uCount; ++i)
			{
				PostAccept();
				_uAccpetCount += 1;
			}
		}
	});
}

BOOL libTools::CSocketBaseServerService::OnRecv(_In_ CSocketIoEvent* pSocketIoEvent, _In_ int dwNumberOfBytesTransfered)
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		CSocketRemoteClient* pSocketClient = pSocketIoEvent->GetSocketClient();

		auto Buffer = pSocketIoEvent->GetSocketClient()->GetRecvBuffer();
		if (dwNumberOfBytesTransfered < 4)
		{
			// Packet Stype: [Size + Flag1 + Flag2 + MsgHead]
			pSocketClient->SetBuffer(reinterpret_cast<CONST CHAR*>(Buffer), static_cast<UINT>(dwNumberOfBytesTransfered));

			// Wait for Recv Next Packet~
			return PostRecv(pSocketIoEvent->GetSocketClient());
		}


		// Get Single Packet Size
		if (pSocketClient->GetMaxSize() == 0)
		{
			DWORD dwSize = 0;
			CSocketBuffer::SetValue_By_Buffer(dwSize, reinterpret_cast<CONST BYTE*>(Buffer));
			pSocketClient->SetMaxSize(dwSize);
		}

		while (dwNumberOfBytesTransfered + pSocketClient->GetBufferLength() >= pSocketClient->GetMaxSize())
		{
			// 
			auto dwBufferIndex = static_cast<int>(pSocketClient->GetMaxSize()) - static_cast<int>(pSocketClient->GetBufferLength());
			//if (dwBufferIndex < 0 || dwBufferIndex > 1024)
			//	LOG_CF_E(L"dwBufferSize=%d", dwBufferIndex);

			pSocketClient->SetBuffer(reinterpret_cast<CHAR*>(Buffer), dwBufferIndex);
			if (pSocketClient->GetBufferLength() == 0)
			{
				LOG_C_E(L"OnRecv.GetBufferLength=0");
				break;
			}

			Buffer += dwBufferIndex;


			if (!InvokeEchoPakcet(pSocketClient))
			{
				LOG_CF_E(L"InvokeEchoPakcet = FALSE;");
				return FALSE;
			}
			//LOG_CF_D(L"RecvMsg=%X", SocketClient->GetVecBufferValue<DWORD>(3));

			pSocketClient->ClearBuffer();
			pSocketClient->SetMaxSize(0);

			dwNumberOfBytesTransfered -= dwBufferIndex;
			if (dwNumberOfBytesTransfered >= 4)
			{
				DWORD dwBufferSize = 0;
				CSocketBuffer::SetValue_By_Buffer(dwBufferSize, Buffer);
				pSocketClient->SetMaxSize(dwBufferSize);
			}
			else
			{
				dwNumberOfBytesTransfered = NULL;
				pSocketClient->SetMaxSize(0);
				break;
			}
		}
		if (dwNumberOfBytesTransfered > 0)
		{
			pSocketClient->SetBuffer(reinterpret_cast<CHAR*>(Buffer), static_cast<DWORD>(dwNumberOfBytesTransfered));

			// 投递下一次的Recv, 等待Client的send
			return PostRecv(pSocketIoEvent->GetSocketClient());
		}

		// 投递下一次的Recv, 等待Client的send
		return PostRecv(pSocketIoEvent->GetSocketClient());
	});
}

VOID libTools::CSocketBaseServerService::OnSend(_In_ CSocketIoEvent* pSocketIoEvent, _In_ int dwNumberOfBytesTransfered)
{
	SocketSendBuffer * pSendBuffer = pSocketIoEvent->GetTag<SocketSendBuffer>();
	if (pSendBuffer != nullptr)
	{
		pSendBuffer->CurLength += dwNumberOfBytesTransfered;
		if (pSendBuffer->CurLength >= pSendBuffer->MaxLength)
		{
			delete[] pSendBuffer->Buffer;
			delete pSendBuffer;
			pSocketIoEvent->SetTag<SocketSendBuffer>(nullptr);
		}
		else
		{
			LOG_CF_E(L"OnSend.pSendBuffer->CurLength[%d] < pSendBuffer->MaxLength[%d]", pSendBuffer->CurLength, pSendBuffer->MaxLength);
		}
	}
}

VOID libTools::CSocketBaseServerService::OnClose(_In_ CSocketRemoteClient* pSocketClient)
{
	LOG_C_D(L"CSocketBaseServerService::OnClose");
	CException::InvokeAction(__FUNCTIONW__, [pSocketClient, this]
	{
		if (pSocketClient->GetRefCount() == 0)
		{
			InvokeVecSocketClient([this, pSocketClient]
			{
				auto itr = _VecSocketClient.find(pSocketClient);
				if (itr != _VecSocketClient.end())
				{
					_VecSocketClient.erase(itr);
					ReleaseSocketClient(pSocketClient);
				}
				else
				{
					LOG_C_E(L"UnExist pSocketClient=%X", pSocketClient);
				}
			});
		}
		else
		{
			LOG_C_E(L"Release Socket[%X], RefCount=%d", pSocketClient->GetSocket(), pSocketClient->GetRefCount());
		}
	});
}

BOOL libTools::CSocketBaseServerService::InvokeEchoPakcet(_In_ CSocketRemoteClient* pSocketClient)
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		auto pSocketBuffer = new CSocketBuffer;
		if (pSocketBuffer == nullptr)
		{
			LOG_MSG_CF(L"Alloc pSocketBuffer = nullptr in OnRecv");
			return FALSE;
		}

		pSocketClient->GetBuffer(*pSocketBuffer);

		// Echo Packet...
		BOOL bRetCode = EchoRecive(pSocketClient, pSocketBuffer);


		delete pSocketBuffer;
		pSocketBuffer = nullptr;

		return bRetCode;
	});
}

VOID libTools::CSocketBaseServerService::AddClient(_In_ CSocketRemoteClient* pSocketClient)
{
	::setsockopt(pSocketClient->GetSocket(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<CONST CHAR*>(&_ServerSocket), sizeof(_ServerSocket));

	auto pTpIo = ::CreateThreadpoolIo(reinterpret_cast<HANDLE>(pSocketClient->GetSocket()), IoCompletionCallback, this, NULL);
	if (pTpIo == nullptr)
	{
		LOG_MSG_CF(L"CreateThreadpoolIo = nullptr!");
	}
	else
	{
		pSocketClient->SetTpIo(pTpIo);
		pSocketClient->Add();
		pSocketClient->BeginOnLine();
		PostRecv(pSocketClient);
	}
}

VOID libTools::CSocketBaseServerService::InvokeVecSocketClient(_In_ std::function<VOID(VOID)> ActionPtr)
{
	::EnterCriticalSection(&_VecSocketClientLock);
	ActionPtr();
	::LeaveCriticalSection(&_VecSocketClientLock);
}

BOOL libTools::CSocketBaseServerService::CreateServerSocket(_In_ SHORT Port)
{
	// Get Address Info
	addrinfo hints;
	ZeroMemory(&hints, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	CHAR szPortText[32];
	wsprintfA(szPortText, "%d", Port);

	// First Param was IP, if IP = NULL, pAddrinfo.IP = INADDR_ANY
	struct addrinfo* pAddrinfo = nullptr;
	if (getaddrinfo(NULL, szPortText, &hints, &pAddrinfo) != 0)
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
	return FALSE;
}

UINT_PTR libTools::CSocketBaseServerService::CreateClientSocket()
{
	// Get Address Info
	addrinfo hints;
	ZeroMemory(&hints, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0;


	// First Param was IP, if IP = NULL, pAddrinfo.IP = INADDR_ANY
	addrinfo* pAddrinfo = nullptr;
	if (::getaddrinfo(NULL, "0", &hints, &pAddrinfo) != 0)
	{
		LOG_CF_E(L"getaddrinfo = NULL, Err=%d", ::WSAGetLastError());
		return INVALID_SOCKET;
	}

	// Find Useful Socket and Port since Param 'Port'
	for (auto pAddr = pAddrinfo; pAddr != nullptr; pAddr = pAddr->ai_next)
	{
		// Get Overlapped Socket!
		auto Sock = WSASocketW(pAddr->ai_family, pAddr->ai_socktype, pAddr->ai_protocol, NULL, NULL, WSA_FLAG_OVERLAPPED);
		if (Sock == INVALID_SOCKET)
		{
			LOG_CF_E(L"WSASocketW = INVALID_SOCKET! Err=%d", static_cast<int>(reinterpret_cast<sockaddr_in*>(pAddr->ai_addr)->sin_port), WSAGetLastError());
			continue;
		}

		::freeaddrinfo(pAddrinfo);
		return Sock;
	}

	::freeaddrinfo(pAddrinfo);
	return INVALID_SOCKET;
}

BOOL libTools::CSocketBaseServerService::AcceptEx(_In_ _Out_ CSocketRemoteClient* pSocketClient, _In_ LPOVERLAPPED Overlapped)
{
	static LPFN_ACCEPTEX _AcceptEx = nullptr;
	if (_AcceptEx == nullptr)
	{
		DWORD dwBytes = 0;
		GUID guidAcceptEx = WSAID_ACCEPTEX;
		if (::WSAIoctl(_ServerSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &_AcceptEx, sizeof(_AcceptEx), &dwBytes, 0, 0) == SOCKET_ERROR)
		{
			LOG_CF_E(L"WSAIoctl() to get AcceptEx() failed. Err=%d", WSAGetLastError());
			return FALSE;
		}
	}
	

	static BYTE Buffer[1024];
	return _AcceptEx(_ServerSocket, pSocketClient->GetSocket(), Buffer, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, Overlapped);
}
