#include "CommonCharacter.h"

CHAR libTools::CCommonCharacter::ToHex(_In_ UCHAR uc)
{
	UCHAR i = uc & 0xF;
	return i < 10 ? '0' + i : i - 10 + 'a';
}
