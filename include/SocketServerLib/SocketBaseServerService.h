#ifndef __LIBTOOLS_SOCKETSERVERLIB_SOCKETBASESERVERSERVICE_H__
#define __LIBTOOLS_SOCKETSERVERLIB_SOCKETBASESERVERSERVICE_H__

#include <Windows.h>
#include <functional>
#include <set>
#include <atomic>

namespace libTools
{
	class CSocketRemoteClient;
	class CSocketBuffer;
	class CSocketIoEvent;

	class CSocketBaseServerService
	{
	private:
		struct ServerThreadContent
		{
			CSocketBaseServerService* pServer = nullptr;
			LPVOID    Reserve = nullptr;

			static ServerThreadContent* Create(_In_ CSocketBaseServerService* pServer_, _In_ LPVOID Reserve_ = nullptr);
		};

		struct SocketSendBuffer
		{
			CHAR* Buffer = nullptr;
			int   MaxLength = 0;
			int   CurLength = 0;
		};
	public:
		CSocketBaseServerService();
		~CSocketBaseServerService();

		virtual BOOL Run(_In_ SHORT shPort, _In_ UINT uMaxAccept) = NULL;

		virtual VOID Stop() = NULL;
	private:
		//////static ////////////////////////////////////////////////////////////////////

		// ��ɶ˿ڻص�
		static VOID CALLBACK IoCompletionCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred, PTP_IO Io);

		static DWORD WINAPI  _ClearThread(_In_ LPVOID lpParam);
	protected:
		BOOL RunServer(_In_ SHORT shPort, _In_ UINT uMaxAccept);

		VOID StopServer();

		/////IOCP Action/////////////////////////////////////////////////////////////////////
		VOID PostAccept();

		BOOL PostRecv(_In_ CSocketRemoteClient* pSocketClient);

		BOOL PostSend(_In_ CSocketRemoteClient* pSocketClient, _In_ CSocketBuffer* pSocketBuffer);

		VOID OnAccept(_In_ CSocketIoEvent* pSocketIoEvent);

		BOOL OnRecv(_In_ CSocketIoEvent* pSocketIoEvent, _In_ int dwNumberOfBytesTransfered);

		VOID OnSend(_In_ CSocketIoEvent* pSocketIoEvent, _In_ int dwNumberOfBytesTransfered);

		VOID OnClose(_In_ CSocketRemoteClient* pSocketClient);

		BOOL InvokeEchoPakcet(_In_ CSocketRemoteClient* pSocketClient);
	private:
		///////Client Action///////////////////////////////////////////////////////////////////
		VOID AddClient(_In_ CSocketRemoteClient* pSocketClient);

	protected:
		VOID InvokeVecSocketClient(_In_ std::function<VOID(VOID)> ActionPtr);
	protected:
		// Ӧ���Լ�����һ���µ���ȥ�̳�CSocketRemoteClient, Ȼ�󷵻�һ������ָ�����
		virtual CSocketRemoteClient* CreateNewSocketClient(_In_ UINT_PTR ClientSock) = NULL;

		virtual VOID			ReleaseSocketClient(_In_ CSocketRemoteClient* pSocketClient) = NULL;

		// �����հ�
		virtual BOOL			EchoRecive(_In_ CSocketRemoteClient* pSocketClient, _In_ CSocketBuffer* pSocketBuffer) = NULL;
	private:
		/////Socket Network/////////////////////////////////////////////////////////////////////

		// ���������Socket
		BOOL CreateServerSocket(_In_ SHORT Port);

		// �����ͻ���Socket
		UINT_PTR CreateClientSocket();

		//
		BOOL AcceptEx(_In_ _Out_ CSocketRemoteClient* pSocketClient, _In_ LPOVERLAPPED Overlapped);

	private:
		/////Class Variable/////////////////////////////////////////////////////////////////////
		UINT_PTR                                    _ServerSocket;

		TP_IO*                                      _pServerTpIo;

		UINT                                        _uMaxClientCount;

		TP_CALLBACK_ENVIRON                         _Client_Tp_Env;

		TP_CLEANUP_GROUP*                           _pClent_Tp_Ggoup;

		CRITICAL_SECTION							_VecSocketClientLock;

		HANDLE										_hClearThread;
	protected:
		std::atomic<BOOL>							_bRun;

		std::set<CSocketRemoteClient*>				_VecSocketClient;

		std::atomic<UINT>							_uAccpetCount;
	};
}

#endif // !__LIBTOOLS_SOCKETSERVERLIB_SOCKETBASESERVERSERVICE_H__
