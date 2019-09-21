#ifndef __LIBTOOLS_LOGCLIENTDLL_CMDLOGSERVER_H__
#define __LIBTOOLS_LOGCLIENTDLL_CMDLOGSERVER_H__

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <mutex>

class CCmdLogServer
{
private:
	struct CmdLogClient
	{
		std::wstring wsClientName;
		SOCKET skClient;
	};
public:
	CCmdLogServer();
	~CCmdLogServer();

	BOOL Run();
	VOID Stop();

	VOID SendContent(_In_ CONST std::wstring& wsClientName, _In_ CONST std::wstring& wsContent);

private:
	static DWORD WINAPI _AcceptThread(LPVOID lpParm);

	UINT GetClientSocket_By_ClientName(_In_ CONST std::wstring& wsClientName, _Out_ std::vector<SOCKET>& VecClientSocket);

	VOID RemoveSocket(_In_ SOCKET skClient);
private:
	SOCKET						_servSocket;
	std::vector<CmdLogClient>	_VecClient;
	std::mutex					_LockCmdLogClient;
	HANDLE						_hAcceptThread;
	BOOL						_Run;
	CONST int					_nSendTimeout;
	CONST int					_nRecvTimeout;
};

#endif // !__LIBTOOLS_LOGCLIENTDLL_CMDLOGSERVER_H__
