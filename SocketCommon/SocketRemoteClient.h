#ifndef __LIBTOOLS_SOCKETCOMMONLIB_SOCKETREMOTECLIENT_H__
#define __LIBTOOLS_SOCKETCOMMONLIB_SOCKETREMOTECLIENT_H__

#include <vector>
#include <string>
#include "SocketTag.h"

namespace libTools
{
	class CSocketBuffer;
	class CSocketRemoteClient : public CSocketTag
	{
	public:
		CSocketRemoteClient(_In_ UINT_PTR Sock);
		virtual ~CSocketRemoteClient() = default;

		//
		VOID    SetTpIo(_In_ TP_IO* pTpIo);

		// 
		TP_IO*  GetTpIo();


		//
		UINT_PTR  GetSocket();

		//      ��ɾ����ʱ������̳߳��е��øú���, ���Կ������ڴ�����ߵ�ʱ��
		virtual VOID Remove();

		//      ����ӵ�ʱ����õĺ���
		virtual VOID Add();

		// �Ƿ�����
		virtual BOOL IsOnLine() CONST;

		// ���socket�Ƿ�ʹ��
		virtual BOOL InExist() CONST;

		// �Ƿ�������ʱ
		virtual BOOL IsKeepALiveTimeout() CONST;

		// ��ճ���Ļ�����˦����
		VOID	SetBuffer(_In_ CONST CHAR* Buffer, _In_ UINT uLen);

		// ���
		VOID    ClearBuffer();

		// ���Buffer�ǵ�ǰSocket���հ�Buffer
		BYTE*	GetRecvBuffer();

		// ճ�������������г���
		UINT	GetBufferLength() CONST;

		// ת��
		VOID	GetBuffer(_Out_ CSocketBuffer& SocketBuffer);

		// �Ͽ�����
		VOID	DisConnect();

		//
		VOID	Release();

		// ����Pachet Head����ȡ������Ĵ�С
		UINT	GetMaxSize() CONST;

		// ����Pakcet����
		VOID    SetMaxSize(_In_ UINT uSize);

		template<typename T>
		T		GetVecBufferValue(_In_ UINT uIndex)
		{
			return uIndex >= _VecBuffer.size() ? T(0) : reinterpret_cast<T*>(_VecBuffer.data())[uIndex];
		}

		UINT	GetRefCount() CONST;

		VOID	AddRefCount();

		VOID	SubRefCount();

		BOOL	ExistPostRecv() CONST;

		VOID	SetExistPostRecv(_In_ BOOL IsExistPostRecv);

		VOID    SetKeepALive();

		CONST std::wstring& GetRemoteClientIp();

		VOID    BeginOnLine();
	public:
		enum { em_Buffer_Len = 1024 };
	private:
		UINT_PTR           _Socket;
		TP_IO*             _pTpIo;
		std::vector<BYTE>  _VecBuffer;
		UINT			   _uBufferSize;
		UINT			   _uBufferMaxSize;
		BYTE               _RecvBuffer[em_Buffer_Len];
		ULONGLONG		   _ulCreateTick;
		UINT			   _uRefCount;
		BOOL			   _ExistPostRecv;
		ULONGLONG		   _ulKeepALiveTick = NULL;
		std::wstring       _wsClientIp;
	protected:
		bool			   _bExist;
		bool			   _bLock;
	};
}

#endif // !__LIBTOOLS_SOCKETCOMMONLIB_SOCKETREMOTECLIENT_H__
