#include "BaseDbManager.h"
#include <include/ExceptionLib/Exception.h>
#include <include/CharacterLib/Character.h>
#include <include/LogLib/Log.h>

#pragma comment(lib,"ODBC32.lib")

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"ExceptionLib_x64_Debug.lib")
#pragma comment(lib,"LogLib_x64_Debug.lib")
#pragma comment(lib,"CharacterLib_x64_Debug.lib")
#pragma comment(lib,"ProcessLib_x64_Debug.lib")
#else
#pragma comment(lib,"ExceptionLib_x64.lib")
#pragma comment(lib,"LogLib_x64.lib")
#pragma comment(lib,"CharacterLib_x64.lib")
#pragma comment(lib,"ProcessLib_x64.lib")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"ExceptionLib_Debug.lib")
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"ProcessLib_Debug.lib")
#else
#pragma comment(lib,"ExceptionLib.lib")
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"ProcessLib.lib")
#endif // _DEBUG
#endif




#define _SELF L"DbManager.cpp"

libTools::CBaseDbManager::~CBaseDbManager()
{
	StopAsyncThread();
}

BOOL libTools::CBaseDbManager::ExcuteSQL(_In_ CONST std::wstring wsSQL, _In_ UINT uResultCount, _Out_ std::vector<Table>& VecResult) CONST
{
	std::lock_guard<std::mutex> Lock_(_Mtx);
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		SQLEnvParam Env;
		VecResult.clear();

		if (!InitializeSQLEnv(Env))
		{
			FreeMem(Env);
			return FALSE;
		}

		auto Ret = SQLAllocHandle(SQL_HANDLE_STMT, Env.hDbc, &Env.hStmt);
		if (Ret != SQL_SUCCESS)
		{
			LOG_CF_E(L"SQLAllocHandle(SQL_HANDLE_STMT) Faild, Err=%d", Ret);
			FreeMem(Env);
			return FALSE;
		}

		std::shared_ptr<WCHAR> Buffer(new WCHAR[wsSQL.length() + 1], [](WCHAR* p) { delete[] p; });
		CCharacter::strcpy_my(Buffer.get(), wsSQL.c_str());

		Ret = SQLExecDirectW(Env.hStmt, Buffer.get(), static_cast<SQLINTEGER>(wsSQL.length()));
		if (Ret == SQL_SUCCESS_WITH_INFO)
		{
			LOG_CF_E(L"SQLExecDirectW = SQL_SUCCESS_WITH_INFO");
			PrintSQLErrText(SQL_HANDLE_STMT, Env.hStmt);
		}
		else if (Ret != SQL_SUCCESS)
		{
			LOG_CF_E(L"SQLExecDirectW Faild. Ret=%d,SQL=%s", Ret, wsSQL.c_str());
			FreeMem(Env);
			return FALSE;
		}


		while (::SQLFetch(Env.hStmt) == SQL_SUCCESS)
		{
			Table Table_;
			for (UINT i = 0; i < uResultCount; ++i)
			{
				CONST static UINT uMaxTextSize = 128;
				WCHAR Text[uMaxTextSize] = { 0 };
				::SQLGetData(Env.hStmt, static_cast<SQLSMALLINT>(i + 1), SQL_C_WCHAR, Text, uMaxTextSize, NULL);
				Table_.push_back(Text);
			}
			VecResult.push_back(Table_);
		}

		FreeMem(Env);
		return TRUE;
	});
}

BOOL libTools::CBaseDbManager::ExcuteSQL(_In_ CONST std::wstring wsSQL, _Out_ std::vector<Table>& VecResult) CONST
{
	std::lock_guard<std::mutex> Lock_(_Mtx);
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		SQLEnvParam Env;
		VecResult.clear();

		if (!InitializeSQLEnv(Env))
		{
			FreeMem(Env);
			return FALSE;
		}

		auto Ret = SQLAllocHandle(SQL_HANDLE_STMT, Env.hDbc, &Env.hStmt);
		if (Ret != SQL_SUCCESS)
		{
			LOG_CF_E(L"SQLAllocHandle(SQL_HANDLE_STMT) Faild, Err=%d", Ret);
			FreeMem(Env);
			return FALSE;
		}

		std::shared_ptr<WCHAR> Buffer(new WCHAR[wsSQL.length() + 1], [](WCHAR* p) { delete[] p; });
		CCharacter::strcpy_my(Buffer.get(), wsSQL.c_str());

		Ret = SQLExecDirectW(Env.hStmt, Buffer.get(), static_cast<SQLINTEGER>(wsSQL.length()));
		if (Ret == SQL_SUCCESS_WITH_INFO)
		{
			LOG_CF_E(L"SQLExecDirectW = SQL_SUCCESS_WITH_INFO");
			PrintSQLErrText(SQL_HANDLE_STMT, Env.hStmt);
		}
		else if (Ret != SQL_SUCCESS)
		{
			LOG_CF_E(L"SQLExecDirectW Faild. Ret=%d,SQL=%s", Ret, wsSQL.c_str());
			FreeMem(Env);
			return FALSE;
		}


		while (::SQLFetch(Env.hStmt) == SQL_SUCCESS)
		{
			Table Table_;
			for(UINT i = 0;; ++i)
			{
				CONST static UINT uMaxTextSize = 128;
				WCHAR Text[uMaxTextSize] = { 0 };
				if (::SQLGetData(Env.hStmt, static_cast<SQLSMALLINT>(i + 1), SQL_C_WCHAR, Text, uMaxTextSize, NULL) != SQL_SUCCESS)
					break;

				Table_.push_back(Text);
			}
			VecResult.push_back(Table_);
		}

		FreeMem(Env);
		return TRUE;
	});
}

BOOL libTools::CBaseDbManager::ExcuteSQL_SingleResult(_In_ CONST std::wstring& wsSQL, _Out_ std::wstring& wsResultText) CONST
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		wsResultText.clear();

		std::vector<Table> VecResult;
		if (!ExcuteSQL(wsSQL, 1, VecResult))
			return FALSE;

		if (VecResult.size() == 0 || VecResult.at(0).size() == 0)
		{
			LOG_CF_E(L"VecResult = Empty!");
			return FALSE;
		}

		wsResultText = VecResult.at(0).at(0);
		return TRUE;
	});
}

BOOL libTools::CBaseDbManager::ExcuteSQL_NoneResult(_In_ CONST std::wstring& wsSQL) CONST
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		std::vector<Table> VecResult;
		return ExcuteSQL(wsSQL, 0, VecResult);
	});
}

VOID libTools::CBaseDbManager::AsyncExcuteSQL(_In_ CONST std::wstring& wsSQL)
{
	if (_bRun)
	{
		std::lock_guard<CBaseDbManager> Lck(*this);
		_QueAsyncSQL.push(wsSQL);
	}
}

VOID libTools::CBaseDbManager::RunAsyncThread()
{
	if (!_bRun)
	{
		_bRun = TRUE;
		_hAsyncThread = std::thread(&libTools::CBaseDbManager::_AsyncThread, this);
	}
}

VOID libTools::CBaseDbManager::StopAsyncThread()
{
	_bRun = FALSE;
	if (_hAsyncThread.joinable())
	{
		_hAsyncThread.join();
	}
}

BOOL libTools::CBaseDbManager::InitializeSQLEnv(_Out_ SQLEnvParam& Env) CONST
{
	return CException::InvokeFunc<BOOL>(__FUNCTIONW__, [&]
	{
		WCHAR wsDns[32] = { 0 };
		WCHAR wsDbUser[32] = { 0 };
		WCHAR wsDbPass[32] = { 0 };

		libTools::CCharacter::strcpy_my(wsDns, _wsDns.c_str());
		libTools::CCharacter::strcpy_my(wsDbUser, _wsDbUser.c_str());
		libTools::CCharacter::strcpy_my(wsDbPass, _wsDbPass.c_str());

		auto Ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &Env.hEnv);
		if (Ret != SQL_SUCCESS)
		{
			LOG_CF_E(L"SQLAllocHandle(SQL_HANDLE_ENV) = %d!", Ret);
			return FALSE;
		}

		Ret = SQLSetEnvAttr(Env.hEnv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), SQL_IS_INTEGER);
		if (Ret == SQL_SUCCESS_WITH_INFO)
		{
			LOG_CF_E(L"SQLSetEnvAttr = SQL_SUCCESS_WITH_INFO");
			PrintSQLErrText(SQL_HANDLE_ENV, Env.hEnv);
		}
		else if (Ret != SQL_SUCCESS)
		{
			LOG_CF_E(L"SQLSetEnvAttr = %d", Ret);
			PrintSQLErrText(SQL_HANDLE_ENV, Env.hEnv);
			return FALSE;
		}

		Ret = SQLAllocHandle(SQL_HANDLE_DBC, Env.hEnv, &Env.hDbc);
		if (Ret != SQL_SUCCESS && Ret != SQL_SUCCESS_WITH_INFO)
		{
			LOG_CF_E(L"SQLAllocHandle(SQL_HANDLE_DBC) = %d", Ret);
			return FALSE;
		}

		Ret = SQLConnectW(Env.hDbc, wsDns, SQL_NTS, wsDbUser, SQL_NTS, wsDbPass, SQL_NTS);
		if (Ret == SQL_SUCCESS_WITH_INFO)
		{
			//LOG_CF_E(L"SQLConnectW = SQL_SUCCESS_WITH_INFO");
			PrintSQLErrText(SQL_HANDLE_DBC, Env.hDbc);
			return TRUE;
		}
		else if (Ret != SQL_SUCCESS)
		{
			LOG_CF_E(L"SQLConnectW Faild. Err=%d, wsDns[%s], wsUser[%s],wsPass[%s]", Ret, wsDns, wsDbUser, wsDbPass);
			PrintSQLErrText(SQL_HANDLE_DBC, Env.hDbc);
			return FALSE;
		}

		return TRUE;
	});
}

VOID libTools::CBaseDbManager::PrintSQLErrText(_In_ SQLSMALLINT fHandleType, _In_ SQLHANDLE handle) CONST
{
	CException::InvokeAction(__FUNCTIONW__, [&]
	{
		RETCODE plm_retcode = SQL_SUCCESS;
		SQLWCHAR plm_szSqlState[MAX_PATH] = L"", plm_szErrorMsg[MAX_PATH] = L"";
		SDWORD plm_pfNativeError = 0L;
		SWORD plm_pcbErrorMsg = 0;
		SQLSMALLINT plm_cRecNmbr = 1;

		while (plm_retcode != SQL_NO_DATA_FOUND)
		{
			plm_retcode = SQLGetDiagRecW(fHandleType, handle, plm_cRecNmbr, plm_szSqlState, &plm_pfNativeError, plm_szErrorMsg, MAX_PATH - 1, &plm_pcbErrorMsg);
			if (plm_retcode != SQL_NO_DATA_FOUND && plm_pfNativeError != 5701 && plm_pfNativeError != 5703)
			{
				LOG_CF_E(L"SqlState=%s", plm_szSqlState);
				LOG_CF_E(L"NativeError=%d", plm_pfNativeError);
				LOG_CF_E(L"ErrorMsg=%s", plm_szErrorMsg);
				LOG_CF_E(L"pcbErrorMsg=%d", plm_pcbErrorMsg);
			}

			plm_cRecNmbr++;   // Increment to next diagnostic record.
		}
	});
}

VOID libTools::CBaseDbManager::FreeMem(_In_ SQLEnvParam& Env) CONST
{
	CException::InvokeAction(__FUNCTIONW__, [&]
	{
		if (Env.hStmt != SQL_NULL_HSTMT)
		{
			::SQLFreeStmt(Env.hStmt, SQL_CLOSE);
			::SQLFreeHandle(SQL_HANDLE_STMT, Env.hStmt);
			Env.hStmt = SQL_NULL_HSTMT;
		}

		if (Env.hDbc != SQL_NULL_HDBC)
		{
			::SQLDisconnect(Env.hDbc);
			::SQLFreeHandle(SQL_HANDLE_DBC, Env.hDbc);
			Env.hDbc = SQL_NULL_HDBC;
		}

		if (Env.hEnv != SQL_NULL_HENV)
		{
			::SQLFreeHandle(SQL_HANDLE_ENV, Env.hEnv);
			Env.hEnv = SQL_NULL_HENV;
		}
	});
}

VOID libTools::CBaseDbManager::_AsyncThread()
{
	while (_bRun)
	{
		this->Lock();
		if (_QueAsyncSQL.empty())
		{
			this->UnLock();
			::Sleep(100);
			continue;
		}


		std::wstring wsSQL = _QueAsyncSQL.front();
		_QueAsyncSQL.pop();
		this->UnLock();

		if (!ExcuteSQL_NoneResult(wsSQL))
		{
			LOG_CF_E(L"ExcuteSQL_NoneResult = FALSE, SQL='%s'", wsSQL.c_str());
		}
	}
}
