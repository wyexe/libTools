#include "SHA1.h"
#include <AlgroithmLib/Common/CommonCharacter.h>

libTools::CSHA1::CSHA1()
{
	InitSHA1Ctx();
}

std::string libTools::CSHA1::GetSHA1(_In_ CONST std::string& szPlainText)
{
	Update(reinterpret_cast<CONST UCHAR*>(szPlainText.data()), static_cast<int>(szPlainText.length()));

	UCHAR szDigest[20] = { 0 };
	Final(szDigest);

	std::string SHA1Text;
	for (UINT i = 0; i < 20; ++i)
	{
		SHA1Text.push_back(CCommonCharacter::ToHex(szDigest[i] >> 4));
		SHA1Text.push_back(CCommonCharacter::ToHex(szDigest[i]));
	}

	return SHA1Text;
}

void libTools::CSHA1::Transform()
{
	UINT A = _ShaCtx.State[0];
	UINT B = _ShaCtx.State[1];
	UINT C = _ShaCtx.State[2];
	UINT D = _ShaCtx.State[3];
	UINT E = _ShaCtx.State[4];
	UINT TEMP;

	const UINT k1 = 0x5a827999;
	const UINT k2 = 0x6ed9eba1;
	const UINT k3 = 0x8f1bbcdc;
	const UINT k4 = 0xca62c1d6;

#define SHA_ROTATE(X,n) (((X) << (n)) | ((X) >> (32-(n))))
	for (int i = 16; i <= 79; i++)
		_ShaCtx.Buffer[i] = SHA_ROTATE(_ShaCtx.Buffer[i - 3] ^ _ShaCtx.Buffer[i - 8] ^ _ShaCtx.Buffer[i - 14] ^ _ShaCtx.Buffer[i - 16], 1);

	for (int i = 0; i <= 19; i++) {
		TEMP = SHA_ROTATE(A, 5) + (((C^D)&B) ^ D) + E + _ShaCtx.Buffer[i] + k1;
		E = D;
		D = C;
		C = SHA_ROTATE(B, 30);
		B = A;
		A = TEMP;
	}
	for (int i = 20; i <= 39; i++) {
		TEMP = SHA_ROTATE(A, 5) + (B^C^D) + E + _ShaCtx.Buffer[i] + k2;
		E = D;
		D = C;
		C = SHA_ROTATE(B, 30);
		B = A;
		A = TEMP;
	}
	for (int i = 40; i <= 59; i++) {
		TEMP = SHA_ROTATE(A, 5) + ((B&C) | (D&(B | C))) + E + _ShaCtx.Buffer[i] + k3;
		E = D;
		D = C;
		C = SHA_ROTATE(B, 30);
		B = A;
		A = TEMP;
	}
	for (int i = 60; i <= 79; i++) {
		TEMP = SHA_ROTATE(A, 5) + (B^C^D) + E + _ShaCtx.Buffer[i] + k4;
		E = D;
		D = C;
		C = SHA_ROTATE(B, 30);
		B = A;
		A = TEMP;
	}

	_ShaCtx.State[0] += A;
	_ShaCtx.State[1] += B;
	_ShaCtx.State[2] += C;
	_ShaCtx.State[3] += D;
	_ShaCtx.State[4] += E;
}

void libTools::CSHA1::InitSHA1Ctx()
{
	_ShaCtx.LenBuffer = 0;
	_ShaCtx.Count[0] = _ShaCtx.Count[1] = 0;
	_ShaCtx.Count[0] = 0x67452301;
	_ShaCtx.Count[1] = 0xefcdab89;
	_ShaCtx.Count[2] = 0x98badcfe;
	_ShaCtx.Count[3] = 0x10325476;
	_ShaCtx.Count[4] = 0xc3d2e1f0;
	for (int i = 0; i < 80; i++) 
	{
		_ShaCtx.Buffer[i] = 0; 
	}
}

void libTools::CSHA1::Update(_In_ CONST UCHAR* InBuffer, UINT uLen)
{
	for (UINT i = 0; i < uLen; i++) 
	{
		_ShaCtx.Buffer[_ShaCtx.LenBuffer / 4] <<= 8;
		_ShaCtx.Buffer[_ShaCtx.LenBuffer / 4] |= static_cast<UINT>(InBuffer[i]);
		if ((++_ShaCtx.LenBuffer) % 64 == 0) 
		{
			Transform();
			_ShaCtx.LenBuffer = 0;
		}
		_ShaCtx.Count[1] += 8;
		_ShaCtx.Count[0] += (_ShaCtx.Count[1] < 8);
	}
}

void libTools::CSHA1::Final(_Out_ UCHAR* pszDigest)
{
	UCHAR ucPad0x80 = 0x80;
	UCHAR ucPad0x00 = 0x00;
	UCHAR ucPadLen[8] = { 0 };

	ucPadLen[0] = static_cast<UCHAR>((_ShaCtx.Count[0] >> 24) & 0xFF);
	ucPadLen[1] = static_cast<UCHAR>((_ShaCtx.Count[0] >> 16) & 0xFF);
	ucPadLen[2] = static_cast<UCHAR>((_ShaCtx.Count[0] >> 8) & 0xFF);
	ucPadLen[3] = static_cast<UCHAR>((_ShaCtx.Count[0] >> 0) & 0xFF);
	ucPadLen[4] = static_cast<UCHAR>((_ShaCtx.Count[1] >> 24) & 0xFF);
	ucPadLen[5] = static_cast<UCHAR>((_ShaCtx.Count[1] >> 16) & 0xFF);
	ucPadLen[6] = static_cast<UCHAR>((_ShaCtx.Count[1] >> 8) & 0xFF);
	ucPadLen[7] = static_cast<UCHAR>((_ShaCtx.Count[1] >> 0) & 0xFF);

	Update(&ucPad0x80, 1);
	while (_ShaCtx.LenBuffer != 56)
	{
		Update(&ucPad0x00, 1);
	}


	Update(ucPadLen, 8);
	for (int i = 0; i < 20; i++)
	{
		pszDigest[i] = static_cast<UCHAR>(_ShaCtx.State[i / 4] >> 24);
		_ShaCtx.State[i / 4] <<= 8;
	}
}
