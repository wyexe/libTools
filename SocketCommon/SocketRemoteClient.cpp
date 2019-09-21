#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <mswsock.h>	// GetAcceptExSockaddrs
#include "SocketRemoteClient.h"
#include <include/CharacterLib/Character.h>
#include <include/ExceptionLib/Exception.h>
#include "SocketBuffer.h"

#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"ExceptionLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"ExceptionLib.lib")
#endif // _DEBUG

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")
namespace libTools
{
	CSocketRemoteClient::CSocketRemoteClient(_In_ UINT_PTR Sock) : _Socket(Sock), _pTpIo(nullptr), _uBufferSize(0), _uBufferMaxSize(0), _bExist(false), _bLock(false)
	{
		_uRefCount = 0;
		ZeroMemory(_RecvBuffer, sizeof(_RecvBuffer));
		_ulCreateTick = ::GetTickCount64();

		_ExistPostRecv = FALSE;
	}
	VOID CSocketRemoteClient::SetTpIo(_In_ TP_IO* pTpIo)
	{
		_pTpIo = pTpIo;
	}

	TP_IO* CSocketRemoteClient::GetTpIo()
	{
		return _pTpIo;
	}

	SOCKET CSocketRemoteClient::GetSocket()
	{
		return _Socket;
	}

	VOID CSocketRemoteClient::Remove()
	{

	}

	VOID CSocketRemoteClient::Add()
	{

	}

	BOOL CSocketRemoteClient::IsOnLine() CONST
	{
		return _bExist;
	}

	BOOL CSocketRemoteClient::InExist() CONST
	{
		return _bExist;
	}

	BOOL CSocketRemoteClient::IsKeepALiveTimeout() CONST
	{
		return _ulKeepALiveTick != NULL && ::GetTickCount64() - _ulKeepALiveTick >= 60 * 1000;
	}

	VOID CSocketRemoteClient::SetBuffer(_In_ CONST CHAR* Buffer, _In_ UINT uLen)
	{
		if (uLen >= em_Buffer_Len)
		{
			::MessageBoxW(NULL, CCharacter::MakeFormatText(L"uLen[%d] >= em_Buffer_Len[%d]", uLen, em_Buffer_Len).c_str(), L"Error", NULL);
			return;
		}
		for (UINT i = 0; i < uLen; ++i)
			_VecBuffer.push_back(Buffer[i]);

		_uBufferSize += uLen;
	}

	VOID CSocketRemoteClient::ClearBuffer()
	{
		_VecBuffer.clear();
		_uBufferSize = 0;
	}

	BYTE* CSocketRemoteClient::GetRecvBuffer()
	{
		return _RecvBuffer;
	}

	UINT CSocketRemoteClient::GetBufferLength() CONST
	{
		return _uBufferSize;
	}


	VOID CSocketRemoteClient::GetBuffer(_Out_ CSocketBuffer& SocketBuffer)
	{
		SocketBuffer.SetDataPtr(reinterpret_cast<CHAR*>(_VecBuffer.data()), _uBufferSize);
	}

	VOID CSocketRemoteClient::DisConnect()
	{
		if (_Socket != INVALID_SOCKET)
		{
			::shutdown(_Socket, SD_BOTH);
			_bExist = false;
		}
	}

	VOID CSocketRemoteClient::Release()
	{
		CException::InvokeAction(__FUNCTIONW__, [&]
		{
			if (_Socket != INVALID_SOCKET)
			{
				::closesocket(_Socket);
				::CancelIoEx(reinterpret_cast<HANDLE>(_Socket), NULL);
				_Socket = INVALID_SOCKET;
			}


			if (_pTpIo != nullptr)
			{
				::WaitForThreadpoolIoCallbacks(_pTpIo, TRUE);
				::CloseThreadpoolIo(_pTpIo);
				_pTpIo = nullptr;
			}
		});
	}

	UINT CSocketRemoteClient::GetMaxSize() CONST
	{
		return _uBufferMaxSize;
	}

	VOID CSocketRemoteClient::SetMaxSize(_In_ UINT uSize)
	{
		_uBufferMaxSize = uSize;
	}

	UINT CSocketRemoteClient::GetRefCount() CONST
	{
		return _uRefCount;
	}

	VOID CSocketRemoteClient::AddRefCount()
	{
		_uRefCount += 1;
	}

	VOID CSocketRemoteClient::SubRefCount()
	{
		if (_uRefCount > 0)
		{
			_uRefCount -= 1;
		}
	}

	BOOL CSocketRemoteClient::ExistPostRecv() CONST
	{
		return _ExistPostRecv;
	}

	VOID CSocketRemoteClient::SetExistPostRecv(_In_ BOOL IsExistPostRecv)
	{
		_ExistPostRecv = IsExistPostRecv;
	}

	VOID CSocketRemoteClient::SetKeepALive()
	{
		_ulKeepALiveTick = ::GetTickCount64();
	}

	CONST std::wstring& CSocketRemoteClient::GetRemoteClientIp()
	{
		if (_wsClientIp.empty())
		{
			sockaddr RemoteAddr;
			INT uRemoteSize = sizeof(sockaddr);


			getpeername(_Socket, &RemoteAddr, &uRemoteSize);
			std::string ClientIp = inet_ntoa(reinterpret_cast<sockaddr_in *>(&RemoteAddr)->sin_addr);
			_wsClientIp = CCharacter::ASCIIToUnicode(ClientIp);
		}
		return _wsClientIp;
	}

	VOID CSocketRemoteClient::BeginOnLine()
	{
		_bExist = true;
		_ulKeepALiveTick = ::GetTickCount64();
	}

}