#include "RC4.h"
#include <algorithm>

libTools::CRC4::CRC4(_In_ CONST CHAR* pszKeyText, _In_ UINT uKeyLength)
{
	_uKeyLen = uKeyLength;

	K.reserve(_uKeyLen);
	for (UINT i = 0; i < _uKeyLen; ++i)
		K.push_back(pszKeyText[i]);
}

std::shared_ptr<CHAR> libTools::CRC4::GetKeyStream(_In_ UINT uEnctypTextLength)
{
	keyStream(uEnctypTextLength);

	std::shared_ptr<CHAR> pszKeyStream(new CHAR[uEnctypTextLength], [](CHAR * p) {delete[] p; });
	for (UINT i = 0;i < k.size(); ++i)
	{
		pszKeyStream.get()[i] = k[i];
	}
	return pszKeyStream;
}

std::shared_ptr<CHAR> libTools::CRC4::GetEncryptText(_In_ CONST CHAR* pszPlanText, _In_ UINT uLength)
{
	keyStream(uLength);


	std::shared_ptr<CHAR> pszEncryptText(new CHAR[uLength], [](CHAR * p) {delete[] p; });
	for (UINT i = 0; i < uLength; ++i)
	{
		pszEncryptText.get()[i] = pszPlanText[i] ^ k.at(i);
	}
	return pszEncryptText;
}

void libTools::CRC4::Initial()
{
	for (int i = 0; i < 256; ++i)
	{
		S[i] = static_cast<UCHAR>(i);
		T[i] = K[i % _uKeyLen];
	}
}

void libTools::CRC4::rangeS()
{
	int j = 0;
	for (int i = 0; i < 256; ++i)
	{
		j = (j + S[i] + T[i]) % 256;
		S[i] = S[i] + S[j];
		S[j] = S[i] - S[j];
		S[i] = S[i] - S[j];
	}
}

void libTools::CRC4::keyStream(_In_ UINT uLen)
{
	Initial();
	rangeS();

	int m = 0, j = 0, t = 0;
	for(UINT i = 0;i < uLen; ++i)
	{
		m = (m + 1) % 256;
		j = (j + S[m]) % 256;

		S[m] = S[m] + S[j];
		S[j] = S[m] - S[j];
		S[m] = S[m] - S[j];

		t = (S[m] + S[j]) % 256;
		k.push_back(S[t]);
	}
}

std::shared_ptr<CHAR> libTools::CRC4_Decryption::DecryptText(_In_ CONST CHAR* pszEnctryText, _In_ UINT uEncryptLength, _In_ CONST CHAR* pszKeyStream)
{
	std::shared_ptr<CHAR> szPlainKeyText(new CHAR[uEncryptLength], [](CHAR * p) {delete[] p; });
	for (UINT i = 0;i < uEncryptLength; ++i)
	{
		szPlainKeyText.get()[i] = pszEnctryText[i] ^ pszKeyStream[i];
	}
	return szPlainKeyText;
}
