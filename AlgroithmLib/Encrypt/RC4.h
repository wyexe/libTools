#ifndef __LIBTOOLS_ALGORITHMLIB_ENCRYP_RC4_H__
#define __LIBTOOLS_ALGORITHMLIB_ENCRYP_RC4_H__

#include <Windows.h>
#include <vector>
#include <string>
#include <memory>

namespace libTools
{
	/*
	ʹ�÷�����
	RC4 r("Key")
	r.GetEncrypText("����",����)

	����(���ĺ����ĵĳ�����һëһ����)
	RC4 r("key");
	r.GetKeyStream(������|���ĵĳ��ȡ�,��Կ��)
	RC4_Decryption::DecrypText(����,��Կ��,���ص�����)

	���ܷ�ʽ:
	��ž��ǵ���keyStream(Len) ����һ�������ĳ���һ������Կ��
	Ȼ���������Կ�� ^ ���� = ����

	���ܷ���
	ҲҪ����keyStream(Len) ����Կ����һ����Կ��(���ĺ����ĵĳ�����һ����)
	��Կ�� ^ ���� = ����
	*/
	class CRC4 
	{
	public:
		CRC4(_In_ CONST CHAR* pszKeyText, _In_ UINT uKeyLength);

		// ��ȡ��Կ��(���ĳ���,������), ���ǵ��ý��ܺ���֮ǰ���õ�
		std::shared_ptr<CHAR> GetKeyStream(_In_ UINT uEnctypTextLength);

		// ��ȡ�����ı�(����,���ĳ���)
		std::shared_ptr<CHAR> GetEncryptText(_In_ CONST CHAR* pszPlanText, _In_ UINT uLength);
	private:
		// ��ʼ��״̬����S����ʱ����T����keyStream��������
		void Initial();

		// ��ʼ����״̬����S����keyStream��������
		void rangeS();
		
		// ������Կ�� len:����Ϊlen���ֽ�
		void keyStream(_In_ UINT uLen);
	private:
		// ״̬��������256�ֽ�
		UCHAR S[256]; 
		// ��ʱ��������256�ֽ�
		UCHAR T[256]; 
		// ��Կ���ȣ�keylen���ֽڣ�ȡֵ��ΧΪ1-256
		UINT _uKeyLen;
		// �ɱ䳤����Կ
		std::vector<CHAR> K;
		// ��Կ��
		std::vector<CHAR> k;
	};


	class CRC4_Decryption 
	{
	public:
		CRC4_Decryption() = default;
		~CRC4_Decryption() = default;

		// �����ı�,���ܵ���Կ��, ���ܳ���(�����ı�����Կ���ĳ�����һ����), ���ؽ��ܵ��ı�
		static std::shared_ptr<CHAR> DecryptText(_In_ CONST CHAR* pszEnctryText, _In_ UINT uEncryptLength, _In_ CONST CHAR* pszKeyStream);
	};
}




#endif // !__LIBTOOLS_ALGORITHMLIB_ENCRYP_RC4_H__
