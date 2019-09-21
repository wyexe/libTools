#include "CRC32.h"

DWORD libTools::CCRC32::GetCRC32(_In_ LPCSTR pszText, _In_ UINT uSize)
{
	static ULONG ulCrc32Table[256] = { 0 };
	if (ulCrc32Table[0] == 0)
	{
		ULONG Crc;
		for (int i = 0; i < 256; ++i)
		{
			Crc = i;
			for (int j = 0; j < 8; ++j)
			{
				if (Crc & 1)
				{
					Crc = (Crc >> 1) ^ 0xEDB88320;
				}
				else
				{
					Crc >>= 1;
				}
			}
			ulCrc32Table[i] = Crc;
		}
	}

	ULONG crc = 0xFFFFFFFF;
	CONST UCHAR* Buffer = reinterpret_cast<CONST UCHAR*>(pszText);
	for (UINT i = 0; i < uSize; ++i)
	{
		crc = (crc >> 8) ^ ulCrc32Table[(crc & 0xFF) ^ *Buffer++];
	}

	return crc ^ 0xFFFFFFFF;
}