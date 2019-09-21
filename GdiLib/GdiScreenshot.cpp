#include "GdiScreenshot.h"
#include <include/LogLib/Log.h>
#include <include/ProcessLib/Common/ResHandleManager.h>

#ifdef _DEBUG
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"ProcessLib_Debug.lib")
#else
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"ProcessLib.lib")
#endif // _DEBUG


#define _SELF L"GdiScreenshot.cpp"
BOOL libTools::CGdiScreenshot::Screenshot(_In_ HWND hWnd, _In_ CONST std::wstring& wsPath)
{
	std::shared_ptr<BYTE> PicContentPtr;
	UINT uSize = 0;

	if (!GetScreenShotContent(hWnd, PicContentPtr, uSize))
		return FALSE;


	FILE* pFile = nullptr;
	_wfopen_s(&pFile, wsPath.c_str(), L"wb+");
	if (pFile == nullptr)
	{
		LOG_CF_E(L"OpenFile in CLPic.cpp Fiald! Path:%s", wsPath.c_str());
		return FALSE;
	}


	fseek(pFile, 0, SEEK_SET);
	fwrite(PicContentPtr.get(), uSize, 1, pFile);
	fclose(pFile);
	return TRUE;
}

BOOL libTools::CGdiScreenshot::GetScreenShotContent(_In_ HWND hWnd, _Out_ std::shared_ptr<BYTE>& PicContentPtr, _Out_ UINT& uSize)
{
	if (hWnd == NULL)
		hWnd = ::GetDesktopWindow();

	RECT WindowRect;
	if (!::GetWindowRect(hWnd, &WindowRect))
	{
		LOG_CF_E(L"GetWindowRect in CLPic.cpp = FALSE;");
		return FALSE;
	}

	UINT uWidth = WindowRect.right - WindowRect.left;
	UINT uHeight = WindowRect.bottom - WindowRect.top;

	HDC hSrcDC = ::GetWindowDC(hWnd);
	if (hSrcDC == NULL)
	{
		LOG_CF_E(L"GetWindowDC in CLPic.cpp = NULL;");
		return FALSE;
	}

	SetResDeleter(hSrcDC, [hWnd](HDC& hDc) { ::ReleaseDC(hWnd, hDc); });

	HDC hMemDC = ::CreateCompatibleDC(hSrcDC);
	if (hMemDC == NULL)
	{
		LOG_CF_E(L"CreateCompatibleDC in CLPic.cpp = NULL; Err=%d", ::GetLastError());
		return FALSE;
	}
	SetResDeleter(hMemDC, [](HDC& hMemDC) {  ::DeleteDC(hMemDC); });

	HBITMAP hBitmap = ::CreateCompatibleBitmap(hSrcDC, uWidth, uHeight);
	if (hBitmap == NULL)
	{
		LOG_CF_E(L"CreateCompatibleDC in CLPic.cpp = NULL;");
		return FALSE;
	}
	SetResDeleter(hBitmap, [](HBITMAP& hBitmap) { ::DeleteObject(hBitmap); });

	/*HBITMAP hOldBitmap = */(HBITMAP)::SelectObject(hMemDC, hBitmap);
	::BitBlt(hMemDC, 0, 0, uWidth, uHeight, hSrcDC, 0, 0, SRCCOPY);

	BITMAP bitmap = { 0 };
	::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	BITMAPINFOHEADER bi = { 0 };
	BITMAPFILEHEADER bf = { 0 };

	CONST int nBitCount = 24;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = nBitCount;
	bi.biCompression = BI_RGB;
	DWORD dwSize = ((bitmap.bmWidth * nBitCount + 31) / 32) * 4 * bitmap.bmHeight;

	HANDLE hDib = GlobalAlloc(GHND, dwSize + sizeof(BITMAPINFOHEADER));
	if (hDib == NULL)
	{
		LOG_CF_E(L"GlobalAlloc in CLPic.cpp = NULL;");
		return FALSE;
	}
	SetResDeleter(hDib, [](HANDLE& hDib) { ::GlobalFree(hDib); });

	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	if (lpbi == nullptr)
		return FALSE;

	*lpbi = bi;
	::GetDIBits(hMemDC, hBitmap, 0, bitmap.bmHeight, (BYTE*)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	bf.bfType = 0x4d42;
	dwSize += sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bf.bfSize = dwSize;
	bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	GlobalUnlock(hDib);

	uSize = dwSize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	PicContentPtr = std::shared_ptr<BYTE>(new BYTE[uSize], [](BYTE* p) { delete[] p; });
	memcpy(PicContentPtr.get(), &bf, sizeof(BITMAPFILEHEADER));
	memcpy(PicContentPtr.get() + sizeof(BITMAPFILEHEADER), lpbi, uSize - sizeof(BITMAPFILEHEADER));
	return TRUE;
}

