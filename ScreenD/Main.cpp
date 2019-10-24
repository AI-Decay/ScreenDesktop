#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>
#include <fstream>


using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

extern int GetImage(HWND);
extern void SaveSettings();
extern void LoadSettings();


#define ClearMemory() { DeleteObject(hdcMemDC); ReleaseDC(hWnd, hdcWindow); return 0;}

int FileCount = 1;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreatePatternBrush(static_cast<HBITMAP>(LoadImage(NULL, TEXT("..//Background.bmp"), 0, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION)));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		switch (uMsg)
		{
			case WM_CREATE:
			{

				HWND hButton = CreateWindow(L"BUTTON", L"Get Screen", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					120, 0, 100, 50, hWnd, reinterpret_cast<HMENU>(101), nullptr, nullptr);
				CreateDirectory(L"Screenshots", NULL);
			}
			return 0;

			case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
					case 101:
					{	
							GetImage(hWnd);
					}
					break;
				}
			}
			return 0;

			case WM_DESTROY:
			{
				PostQuitMessage(EXIT_SUCCESS);
			}
			return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};
	wc.lpszClassName = L"ScreenD";
	wc.lpszMenuName = L"ToolBar";
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	hwnd = CreateWindow(wc.lpszClassName, L"ScreenD 1.0", WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION,
		300, 0, 360, 300, nullptr, nullptr, wc.hInstance, nullptr);

	if (hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	RegisterHotKey(NULL, 100, NULL, VK_SNAPSHOT);
	LoadSettings();

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		if(msg.message == WM_HOTKEY)
			GetImage(hwnd);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	SaveSettings();
	
	return static_cast<int>(msg.lParam);
}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          
	UINT  size = 0;        

	GetImageEncodersSize(&num, &size);
	if (!size)
		return EXIT_FAILURE;

	ImageCodecInfo* pImageCodecInfo{ static_cast<ImageCodecInfo*>(malloc(size)) };
	if (!pImageCodecInfo)
		return EXIT_FAILURE;

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (!wcscmp(pImageCodecInfo[j].MimeType, format))
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return EXIT_FAILURE;
}

bool is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

void LoadSettings() {

	int dataFile;

	std::ifstream read;
	read.open("..//Settings.txt");


	if (read.is_open()) {
		
		if (is_empty(read))
			FileCount = 0;
		else
			read >> FileCount;
	}

	read.close();
}

void SaveSettings() {

	std::ofstream write;
	write.open("..//Settings.txt");
	write << FileCount;
	write.close();
}

int GetImage(HWND hWnd) {

	HDC hdcWindow{}, hdcMemDC{};
	HBITMAP hbmScreen{};
	BITMAP bmpScreen{};

	hdcWindow = GetDC(NULL);

	hdcMemDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC) {
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		ClearMemory();
	}

	RECT rcClient;
	GetClientRect(GetDesktopWindow(), &rcClient);

	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right, rcClient.bottom);

	if (!hbmScreen) {
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		ClearMemory();
	}

	SelectObject(hdcMemDC, hbmScreen);



	if (!(BitBlt(hdcMemDC, 0, 0, rcClient.right, rcClient.bottom, hdcWindow, 0, 0, SRCCOPY))) {
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		ClearMemory();
	}


	GetObject(hdcWindow, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER bmfHeader{};
	BITMAPINFOHEADER bi{};

	bi.biSize = sizeof(BITMAPFILEHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bmfHeader.bfOffBits = static_cast<DWORD>(sizeof(BITMAPFILEHEADER)) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;

	
	GdiplusStartupInput gdiplusStartupInput{};
	ULONG_PTR gdiplusToken{};
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Bitmap* image = new Bitmap(hbmScreen, NULL);


	CLSID myClsId{};
	int retVal = GetEncoderClsid(L"image/bmp", &myClsId);

	std::wstring name{ L"Screenshots\\Screenshot_" + std::to_wstring(FileCount) + L".bmp"};
	FileCount++;

	const WCHAR* FileName = name.c_str();

	image->Save(FileName, &myClsId, NULL);
	delete image;

	GdiplusShutdown(gdiplusToken);
	
	ClearMemory();
}