#include "SocketBuffer.h"
#include <include/CharacterLib/Character.h>
#include <include/AlgroithmLib/Encrypt/CRC32.h>

#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"AlgroithmLib.lib")
#endif // _DEBUG

libTools::CSocketBuffer::CSocketBuffer() : _dwMsgHead(NULL)
{

}

VOID libTools::CSocketBuffer::SetValue_By_Buffer(_Out_ DWORD& dwValue, _In_ CONST BYTE* Buffer)
{
	BYTE* ValuePtr = reinterpret_cast<BYTE*>(&dwValue);
	for (int i = 0; i < sizeof(dwValue); ++i)
		*ValuePtr++ = Buffer[i];

	//*ValuePtr++ = Buffer[0];
	//*ValuePtr++ = Buffer[1];
	//*ValuePtr++ = Buffer[2];
	//*ValuePtr++ = Buffer[3];
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(ULONGLONG ulValue)
{
	for (int i = 0;i < 8; ++i)
	{
		Set(static_cast<BYTE>(ulValue >> (i * 0x8) & 0xFF));
	}
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(int nValue)
{
	return *this << static_cast<DWORD>(nValue);
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(DWORD dwValue)
{
	Set(static_cast<BYTE>(dwValue & 0xFF));
	Set(static_cast<BYTE>(dwValue >> 0x8 & 0xFF));
	Set(static_cast<BYTE>(dwValue >> 16 & 0xFF));
	Set(static_cast<BYTE>(dwValue >> 24 & 0xFF));
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(bool bValue)
{
	Set(bValue ? 1 : 0);
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(WORD wValue)
{
	Set(static_cast<BYTE>(wValue & 0xFF));
	Set(static_cast<BYTE>(wValue >> 0x8 & 0xFF));
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(BYTE bValue)
{
	Set(bValue);
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(float fVersion)
{
	DWORD dwValue = *reinterpret_cast<DWORD*>(&fVersion);
	return *this << dwValue;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(UINT uValue)
{
	return *this << static_cast<DWORD>(uValue);
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(CONST std::wstring& wsText)
{
	// Unicode to UTF-8
	std::string str = CCharacter::UnicodeToUTF8(wsText);
	for (CONST auto& itm : str)
		Set(static_cast<BYTE>(itm));

	Set(0);
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(CONST SYSTEMTIME& Tick)
{
	return *this << Tick.wYear << Tick.wMonth << Tick.wDay << Tick.wDayOfWeek << Tick.wHour << Tick.wMinute << Tick.wSecond << Tick.wMilliseconds;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator<<(LPCWSTR lpwszText)
{
	return *this << std::wstring(lpwszText);
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(DWORD& dwValue)
{
	BYTE* ValuePtr = reinterpret_cast<BYTE*>(&dwValue);
	*ValuePtr++ = Get();
	*ValuePtr++ = Get();
	*ValuePtr++ = Get();
	*ValuePtr++ = Get();
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(ULONGLONG& ulValue)
{
	BYTE* ValuePtr = reinterpret_cast<BYTE*>(&ulValue);
	for (int i = 0; i < 8; ++i)
	{
		*ValuePtr++ = Get();
	}
	return *this;
}
libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(WORD& wValue)
{
	BYTE* ValuePtr = reinterpret_cast<BYTE*>(&wValue);
	*ValuePtr++ = Get();
	*ValuePtr++ = Get();
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(BYTE& bValue)
{
	bValue = Get();
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(bool& bValue)
{
	bValue = Get() == 1 ? true : false;
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(std::wstring& wsText)
{
	BYTE bData = 0;
	std::string sText;
	while ((bData = Get()) != 0)
		sText.push_back(static_cast<CHAR>(bData));

	wsText = CCharacter::UTF8ToUnicode(sText);
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(float& fValue)
{
	DWORD dwValue = 0;
	*this >> dwValue;
	fValue = *reinterpret_cast<float*>(&dwValue);
	return *this;
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(UINT& uValue)
{
	return *this >> reinterpret_cast<DWORD&>(uValue);
}


libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(int& nValue)
{
	return *this >> reinterpret_cast<DWORD&>(nValue);
}

libTools::CSocketBuffer& libTools::CSocketBuffer::operator>>(SYSTEMTIME& Tick)
{
	return *this >> Tick.wYear >> Tick.wMonth >> Tick.wDay >> Tick.wDayOfWeek >> Tick.wHour >> Tick.wMinute >> Tick.wSecond >> Tick.wMilliseconds;
}

std::shared_ptr<CHAR> libTools::CSocketBuffer::GetBuffer(_Out_ UINT& uSize)
{
	uSize = static_cast<UINT>(_Data.size());
	std::shared_ptr<CHAR> DataPtr(new CHAR[uSize], [](CHAR* lpAddr) { delete[] lpAddr; });
	memcpy(DataPtr.get(), _Data.data(), uSize);
	_Data.clear();
	return DataPtr;
}

std::shared_ptr<CHAR> libTools::CSocketBuffer::GetSizeBuffer(_In_ UINT uSize)
{
	std::shared_ptr<CHAR> DataPtr(new CHAR[uSize], [](CHAR* lpAddr) { delete[] lpAddr; });
	if (_Data.size() < uSize)
	{
		MessageBoxW(NULL, libTools::CCharacter::MakeFormatText(L"_Data.size=[%d] < uSize[%d]", _Data.size(), uSize).c_str(), L"libTools::CSocketBuffer::GetSizeBuffer", NULL);
		return DataPtr;
	}
	
	memcpy(DataPtr.get(), _Data.data(), uSize);
	_Data.erase(_Data.begin(), _Data.begin() + uSize);
	return DataPtr;
}

UINT libTools::CSocketBuffer::CopyToBuffer(_In_ CHAR* OutBuffer, _In_ UINT uMaxOutBufferSize)
{
	if (_Data.size() >= uMaxOutBufferSize)
		return 0;

	memcpy(OutBuffer, _Data.data(), _Data.size());
	return _Data.size();
}

DWORD libTools::CSocketBuffer::GetCRC() CONST
{
	return CCRC32::GetCRC32(reinterpret_cast<CONST CHAR*>(_Data.data()), _Data.size());
}

std::shared_ptr<CHAR> libTools::CSocketBuffer::GetDataPtr(_Out_ UINT& uSize)
{
	uSize = static_cast<UINT>(_Data.size()) + 4;
	std::shared_ptr<CHAR> DataPtr(new CHAR[uSize], [](CHAR* lpAddr) { delete[] lpAddr; });

	// Write Buffer Size
	DataPtr.get()[0] = static_cast<CHAR>(uSize & 0xFF);
	DataPtr.get()[1] = static_cast<CHAR>((uSize >> 8) & 0xFF);
	DataPtr.get()[2] = static_cast<CHAR>((uSize >> 16) & 0xFF);
	DataPtr.get()[3] = static_cast<CHAR>((uSize >> 24) & 0xFF);

	// 
	if (uSize > 4)
	{
		//_Data.resize(_Data.size() + uSize - 4);
		memcpy(DataPtr.get() + 4, _Data.data(), uSize - 4);
	}

	return DataPtr;
}

VOID libTools::CSocketBuffer::SetDataPtr(_In_ CONST std::shared_ptr<CHAR>& DataPtr, _In_ UINT uSize)
{
	SetDataPtr(DataPtr.get(), uSize);
}


VOID libTools::CSocketBuffer::SetDataPtr(_In_ CONST CHAR* Buffer, _In_ UINT uSize)
{
	if (_Data.capacity() - _Data.size() < uSize)
	{
		_Data.reserve(_Data.size() + uSize);
	}
	auto Index = _Data.size();
	_Data.resize(_Data.size() + uSize);
	memcpy(_Data.data() + Index, Buffer, uSize);
}

VOID libTools::CSocketBuffer::clear()
{
	_dwMsgHead = NULL;
	_Data = {};
}

VOID libTools::CSocketBuffer::AddVerData()
{
	*this << static_cast<DWORD>(0x434C536F);
	*this << static_cast<DWORD>(0x636B6574);
	*this << static_cast<DWORD>(_dwMsgHead);
}

VOID libTools::CSocketBuffer::Set(BYTE Data)
{
	_Data.push_back(Data);
}

BYTE libTools::CSocketBuffer::Get()
{
	BYTE Data = 0;
	if (!_Data.empty())
	{
		Data = _Data.front();
		_Data.erase(_Data.begin());
	}
	return Data;
}