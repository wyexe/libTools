#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "CmdLog.h"
#include <thread>
#include <WS2tcpip.h>
#include <process.h>	// 
#include <include/CharacterLib/Character.h>
#include "Log.h"
#include "LogExpressionCalc.h"
#include "LogExpressionPeLoader.h"
#pragma  comment (lib,"wsock32.lib") 
#pragma  comment (lib, "Ws2_32.lib")


#define _SELF L"CmdLog.cpp"

namespace libTools
{
	CCmdLog::CCmdLog() : _Run(FALSE), _skClient(INVALID_SOCKET), _hRecvThread(NULL)
	{

	}

	CCmdLog::~CCmdLog()
	{
		Stop();
	}

	libTools::CCmdLog& CCmdLog::GetInstance()
	{
		static CCmdLog Instance;
		return Instance;
	}

	BOOL CCmdLog::Run(_In_ CONST std::wstring& wsClientName, _In_ CONST std::vector<ExpressionFunPtr>& ParmVecFunPtr)
	{
		if (wsClientName == L"--")
		{
			LOG_MSG_CF(L"Invalid ClientName[--]!!");
			return FALSE;
		}


		WSAData wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);

		_Run = TRUE;
		_wsClientName = wsClientName;
		_LogExpression.SetVecExprFunPtr(ParmVecFunPtr);
		_hRecvThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, NULL, reinterpret_cast<_beginthreadex_proc_type>(_RecvThread), this, NULL, NULL));
		return TRUE;
	}

	VOID CCmdLog::Stop()
	{
		_Run = FALSE;
		BreakLogConnect();
		if (_hRecvThread != NULL)
		{
			if (::WaitForSingleObject(_hRecvThread, 3 * 1000) == WAIT_TIMEOUT)
				::TerminateThread(_hRecvThread, 0);

			::CloseHandle(_hRecvThread);
			_hRecvThread = NULL;
		}
	}

	VOID CCmdLog::ReSetClientName(_In_ CONST std::wstring& wsClientName)
	{
		_wsClientName = wsClientName;
		BreakLogConnect();
	}

	DWORD WINAPI CCmdLog::_RecvThread(LPVOID lpParm)
	{
		CCmdLog* pCCmdLog = (CCmdLog *)lpParm;

		while (pCCmdLog->_Run)
		{
			if (!pCCmdLog->ConnectLogServer())
			{
				::Sleep(100);
				continue;
			}

			if (!pCCmdLog->SendClientName())
			{
				pCCmdLog->BreakLogConnect();
				continue;
			}

			while (pCCmdLog->_Run)
			{
				std::wstring wsCmdText;
				if (!pCCmdLog->Recv(wsCmdText))
					break;


				pCCmdLog->ExcuteLogServerCmd(wsCmdText);
			}

		}
		return 0;
	}

	VOID CCmdLog::ExcuteLogServerCmd(_In_ CONST std::wstring& wsCmdText)
	{
		WCHAR* pwszCmdText = new WCHAR[1024];
		CCharacter::strcpy_my(pwszCmdText, wsCmdText.c_str());

		std::thread t([pwszCmdText, this]
		{
			CONST std::wstring wsCmdText = pwszCmdText;
			LOG_C(libTools::CLog::em_Log_Type::em_Log_Type_Custome, L"Client:%s Excute Cmd Text:%s", _wsClientName.c_str(), wsCmdText.c_str());
			CLogExpressionCalc ExpAnalysis;

			if (ExpAnalysis.IsConformToCmdType(wsCmdText))
				ExpAnalysis.Analysis(wsCmdText);
			else
				_LogExpression.Run(wsCmdText);

			delete[] pwszCmdText;
		});
		t.detach();
	}

	VOID CCmdLog::EnableGloablMethod()
	{
#ifndef _WIN64
		_LogExpression.Push(std::bind(&CLogExpressionPeLoader::PeLoader, std::placeholders::_1), L"PeLoader");
#endif
	}

	VOID CCmdLog::PrintHelp()
	{
		_LogExpression.Run(L"Help()");
	}

	BOOL CCmdLog::ConnectLogServer()
	{
		if (_skClient == INVALID_SOCKET)
		{
			_skClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (_skClient == INVALID_SOCKET)
				return FALSE;

			CONST static int nSendTimeout = 3 * 1000;
			setsockopt(_skClient, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<CONST CHAR*>(&nSendTimeout), sizeof(int));
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(em_CmdLog_Port);

		inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
		return connect(_skClient, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR ? FALSE : TRUE;
	}

	VOID CCmdLog::BreakLogConnect()
	{
		if (_skClient != INVALID_SOCKET)
		{
			::shutdown(_skClient, SD_BOTH);
			::closesocket(_skClient);
			_skClient = INVALID_SOCKET;
		}
	}

	BOOL CCmdLog::SendClientName()
	{
		SendText SendText_ = { 0 };
		SendText_.dwParm1 = 0x434C436C;
		SendText_.dwParm2 = 0x69656E74;
		SendText_.dwMsg = static_cast<DWORD>(em_CmdLog_Msg_SendClientInfo);
		CCharacter::strcpy_my(SendText_.wszText, _wsClientName.c_str());


		return Send(reinterpret_cast<CONST CHAR*>(&SendText_), sizeof(SendText_));
	}

	int CCmdLog::Send(_In_ CONST CHAR* Buffer, _In_ INT nLen)
	{
		return ::send(_skClient, reinterpret_cast<CONST CHAR*>(Buffer), nLen, 0);
	}

	BOOL CCmdLog::Recv(_Out_ std::wstring& wsCmdText)
	{
		RecvText RecvText_ = { 0 };
		int nRetCode = ::recv(_skClient, reinterpret_cast<CHAR*>(&RecvText_), sizeof(RecvText_), 0);
		if (nRetCode == -1 || nRetCode != sizeof(RecvText_))
		{
			BreakLogConnect();
			wsCmdText.clear();
			return FALSE;
		}
		else if (RecvText_.dwParm1 != 0x434C536F || RecvText_.dwParm2 != 0x69656E74)
		{
			BreakLogConnect();
			wsCmdText.clear();
			return FALSE;
		}

		wsCmdText = RecvText_.wszText;
		return TRUE;
	}

}
