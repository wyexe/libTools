#include "RSA.h"
#include <algorithm>

void libTools::CRSA::GenerateKey(_Out_ PublicKeyClass* PubKey, _Out_ PrivateKeyClass* PriKey)
{
	LONGLONG p = GenerateRandPrime();
	LONGLONG q = GenerateRandPrime();
	LONGLONG e = static_cast<LONGLONG>(pow(static_cast<double>(2), static_cast<double>(8)) + 1);
	LONGLONG MaxValue = p * q;
	LONGLONG MaxPhi = (p - 1) * (q - 1); // 欧拉数


	LONGLONG d = ExtEuclid(MaxPhi, e);
	while (d < 0)
	{
		d = d + MaxPhi;
	}


	PubKey->modulus = MaxValue;
	PriKey->modulus = MaxValue;

	
	PubKey->exponent = e;
	PriKey->exponent = d;
}

VOID libTools::CRSA::Encrypt(_In_ CONST PublicKeyClass& PubKey, _In_ CONST std::vector<CHAR>& VecText, _Out_ std::vector<DWORD>& VecBinary) CONST
{
	VecBinary.reserve(VecText.size());
	for (auto c : VecText)
	{
		VecBinary.push_back(static_cast<DWORD>(Rsa_ModExp(c, PubKey.exponent, PubKey.modulus)));
	}
}

VOID libTools::CRSA::Encrypt(_In_ CONST PublicKeyClass& PubKey, _In_ CONST std::wstring& wsText, _Out_ std::vector<DWORD>& VecBinary) CONST
{
	VecBinary.reserve(wsText.length());
	for (auto c : wsText)
	{
		VecBinary.push_back(static_cast<DWORD>(Rsa_ModExp(c, PubKey.exponent, PubKey.modulus)));
	}
}

VOID libTools::CRSA::Decrypt(_In_ CONST PrivateKeyClass& PrivKey, _In_ CONST std::vector<DWORD>& VecEncrypText, _Out_ std::vector<CHAR>& VecBinary) CONST
{
	VecBinary.reserve(VecEncrypText.size());
	for (auto c : VecEncrypText)
	{
		VecBinary.push_back(static_cast<CHAR>(Rsa_ModExp(c, PrivKey.exponent, PrivKey.modulus)));
	}
}

VOID libTools::CRSA::Decrypt(_In_ CONST PrivateKeyClass& PrivKey, _In_ CONST std::vector<DWORD>& VecEncrypText, _Out_ std::wstring& wsText) CONST
{
	wsText.reserve(VecEncrypText.size());
	for (auto c : VecEncrypText)
	{
		wsText.push_back(static_cast<WCHAR>(Rsa_ModExp(c, PrivKey.exponent, PrivKey.modulus)));
	}
}

LONGLONG libTools::CRSA::Gcd(_In_ LONGLONG a, _In_ LONGLONG b) CONST
{
	for (LONGLONG c = 0; a != 0; b = c)
	{
		c = a;
		a = b % a;
	}

	return b;
}

LONGLONG libTools::CRSA::ExtEuclid(_In_ LONGLONG a, _In_ LONGLONG b) CONST
{
	LONGLONG y = 1;
	for (LONGLONG x = 0, u = 1, v = 0, gcd = b, m = 0, n = 0, q = 0, r = 0; a != 0 ;)
	{
		q = gcd / a;
		r = gcd % a;
		m = x - u * q;
		n = y - v * q;
		gcd = a;
		a = r;
		x = u;
		y = v;
		u = m;
		v = n;
	}

	return y;
}

LONGLONG libTools::CRSA::Rsa_ModExp(_In_ LONGLONG b, _In_ LONGLONG e, _In_ LONGLONG m) CONST
{
	b = b % m;
	if (e == 0)
	{
		return 1;
	}
	else if (e == 1)
	{
		return b;
	}
	else if (e % 2 == 0)
	{
		return Rsa_ModExp(b * b % m, e / 2, m) % m;
	}
	else if (e % 2 == 1)
	{
		return b * Rsa_ModExp(b, e - 1, m) % m;
	}


	return 0;
}

LONGLONG libTools::CRSA::GenerateRandPrime()
{
	// 素数表 
	CONST static LONG aPrimeList[] =
	{
		1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41,
		43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 113,
		193, 241, 257, 337, 353, 401, 433, 449, 577, 593, 641,
		673, 769, 881, 929, 977, 1009, 1153, 1201, 1217, 1249,
		1297,1361, 1409, 1489, 1553, 1601, 1697, 1777, 1873,
		1889, 2017, 2081, 2113, 2129, 2161, 2273, 2417, 2593,
		2609, 2657, 2689, 2753, 2801, 2833, 2897, 3041, 3089,
		3121, 3137, 3169, 3217, 3313, 3329, 3361, 3457, 3617,
		3697, 3761, 3793, 3889, 4001, 4049, 4129, 4177, 4241,
		4273, 4289, 4337, 4481, 4513, 4561, 4657, 4673, 4721,
		4801, 4817, 4993, 5009, 5153, 5233, 5281, 5297, 5393,
		5441, 5521, 5569, 5857, 5953, 6113, 6257, 6337, 6353,
		6449, 6481, 6529, 6577, 6673, 6689, 6737, 6833, 6961,
		6977, 7057, 7121, 7297, 7393, 7457, 7489, 7537, 7649,
		7681, 7793, 7841, 7873, 7937, 8017, 8081, 8161, 8209,
		8273, 8353, 8369, 8513, 8609, 8641, 8689, 8737, 8753,
		8849, 8929, 9041, 9137, 9281, 9377, 9473, 9521, 9601,
		9649, 9697, 9857
	};


	static LONG ulTick = static_cast<LONG>(::GetTickCount64());
	srand(ulTick);

	return ulTick = aPrimeList[rand() % _countof(aPrimeList)];
}
