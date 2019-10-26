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

BOOL isBmp{};
BOOL isJpeg{};
BOOL isPng{};
BOOL isTiff{};
HICON hIcon;

#define ClearMemory() { DeleteObject(hdcMemDC); ReleaseDC(hWnd, hdcWindow); return 0;}
#define APPWM_ICONNOTIFY (WM_APP + 1)

int FileCount = 1;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nCmdShow)
{
	static HICON hIcon = static_cast<HICON>(LoadImage(NULL, L"..//Tray.ico", IMAGE_ICON, 0, 0,
		LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE | LR_LOADFROMFILE));

	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreatePatternBrush(static_cast<HBITMAP>(LoadImage(NULL, TEXT("..//Background.bmp"), 0, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION)));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = hIcon;
	wc.hIconSm = hIcon;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		
		switch (uMsg)
		{
			case WM_CREATE:
			{
				CreateDirectory(L"Screenshots", NULL);

				HWND hButton = CreateWindow(L"BUTTON", L"Get Screen", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					120, 0, 100, 50, hWnd, reinterpret_cast<HMENU>(101), nullptr, nullptr);
				
				HWND hCheckBmp = CreateWindow(L"BUTTON", L"BMP", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
					24, 100, 60, 20, hWnd, reinterpret_cast<HMENU>(11), nullptr, nullptr);

				HWND hCheckJpg = CreateWindow(L"BUTTON", L"JPEG", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
					108, 100, 60, 20, hWnd, reinterpret_cast<HMENU>(12), nullptr, nullptr);

				HWND hCheckPng = CreateWindow(L"BUTTON", L"PNG", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
					192, 100, 60, 20, hWnd, reinterpret_cast<HMENU>(13), nullptr, nullptr);

				HWND hCheckTiff = CreateWindow(L"BUTTON", L"TIFF", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
					276, 100, 60, 20, hWnd, reinterpret_cast<HMENU>(14), nullptr, nullptr);
				
				LoadSettings();

				if (isJpeg)
					CheckDlgButton(hWnd, 12, BST_CHECKED);
				else if (isPng)
					CheckDlgButton(hWnd, 13, BST_CHECKED);
				else if (isTiff)
					CheckDlgButton(hWnd, 14, BST_CHECKED);
				else
					CheckDlgButton(hWnd, 11, BST_CHECKED);

			}
			return 0;

			case APPWM_ICONNOTIFY:
			{
				switch (lParam)
				{
					case WM_LBUTTONDBLCLK:
					{
						NOTIFYICONDATA nid{};
						nid.cbSize = sizeof(nid);
						nid.hWnd = hWnd;
						nid.uID = 1;

						Shell_NotifyIcon(NIM_DELETE, &nid);
						ShowWindow(hWnd, SW_SHOW);
						ShowWindow(hWnd, SW_RESTORE);
					}
						break;
				}
			}
			return 0;

			case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
					
					case 11:
					{
						isBmp = IsDlgButtonChecked(hWnd, 11);

						if (isBmp) {
							CheckDlgButton(hWnd, 11, BST_UNCHECKED);
							isBmp = FALSE;
						}
						else {
							CheckDlgButton(hWnd, 11, BST_CHECKED);
							CheckDlgButton(hWnd, 12, BST_UNCHECKED);
							CheckDlgButton(hWnd, 13, BST_UNCHECKED);
							CheckDlgButton(hWnd, 14, BST_UNCHECKED);
							isBmp = TRUE;
							isPng = FALSE;
							isTiff = FALSE;
							isJpeg = FALSE;
						}
					}
					break;

					case 12:
					{
						isJpeg = IsDlgButtonChecked(hWnd, 12);

						if (isJpeg) {
							CheckDlgButton(hWnd, 12, BST_UNCHECKED);
							isJpeg = FALSE;
						}
						else {
							CheckDlgButton(hWnd, 12, BST_CHECKED);
							CheckDlgButton(hWnd, 11, BST_UNCHECKED);
							CheckDlgButton(hWnd, 13, BST_UNCHECKED);
							CheckDlgButton(hWnd, 14, BST_UNCHECKED);
							isJpeg = TRUE;
							isPng = FALSE;
							isTiff = FALSE;
							isBmp = FALSE;
						}
					}
					break;

					case 13:
					{
						isPng = IsDlgButtonChecked(hWnd, 13);

						if (isPng) {
							CheckDlgButton(hWnd, 13, BST_UNCHECKED);
							isPng = FALSE;

						}
						else {
							CheckDlgButton(hWnd, 13, BST_CHECKED);
							CheckDlgButton(hWnd, 12, BST_UNCHECKED);
							CheckDlgButton(hWnd, 11, BST_UNCHECKED);
							CheckDlgButton(hWnd, 14, BST_UNCHECKED);
							isPng = TRUE;
							isJpeg = FALSE;
							isTiff = FALSE;
							isBmp = FALSE;
						}
					}
					break;

					case 14:
					{
						isTiff = IsDlgButtonChecked(hWnd, 14);

						if (isTiff) {
							CheckDlgButton(hWnd, 14, BST_UNCHECKED);
							isTiff = FALSE;
						}
						else {
							CheckDlgButton(hWnd, 14, BST_CHECKED);
							CheckDlgButton(hWnd, 12, BST_UNCHECKED);
							CheckDlgButton(hWnd, 13, BST_UNCHECKED);
							CheckDlgButton(hWnd, 11, BST_UNCHECKED);
							isTiff = TRUE;
							isPng = FALSE;
							isJpeg = FALSE;
							isBmp = FALSE;
						}
					}
					break;

					case 101:
					{
							GetImage(hWnd);		
					}
					break;
				}
				
			}
			return 0;

			case WM_SYSCOMMAND:  
			{
				switch (wParam)
				{
				case SC_MINIMIZE:

					NOTIFYICONDATA nid{};
					nid.cbSize = sizeof(nid);
					nid.hWnd = hWnd;
					nid.uID = 1;
					nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
					nid.uCallbackMessage = APPWM_ICONNOTIFY;
					nid.hIcon = hIcon;
					Shell_NotifyIcon(NIM_ADD, &nid);

					ShowWindow(hWnd, SW_MINIMIZE);
					ShowWindow(hWnd, SW_HIDE);
				}
				break;
			}
			return 0;

			case WM_DESTROY:
			{
				DestroyIcon(hIcon);
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

	hwnd = CreateWindow(wc.lpszClassName, L"ScreenD 1.2", WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION,
		300, 0, 360, 300, nullptr, nullptr, wc.hInstance, nullptr);

	if (hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	RegisterHotKey(NULL, 100, NULL, VK_SNAPSHOT);

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
		return -1;

	ImageCodecInfo* pImageCodecInfo{ static_cast<ImageCodecInfo*>(malloc(size)) };
	if (!pImageCodecInfo)
		return -1;

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
	return -1;
}

bool is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

void LoadSettings() {

	std::ifstream read;
	read.open("..//Settings.txt");

	int FormatSet = 0;

	if (read.is_open()) {
		
		if (is_empty(read))
			FileCount = 0;
		else
			read >> FileCount;
			read >> FormatSet;
			switch (FormatSet)
			{
			case 1:
				isBmp = TRUE;
				break;
			case 2:
				isJpeg = TRUE;
				break;
			case 3:
				isPng = TRUE;
				break;
			case 4:
				isTiff = TRUE;
				break;
			default:
				isBmp = TRUE;
				break;
			}

	}

	read.close();
}

void SaveSettings() {

	std::ofstream write;
	write.open("..//Settings.txt");
	write << FileCount << std::endl;

	if (isBmp)
		write << 1;
	else if(isJpeg)
		write << 2;
	else if (isPng)
		write << 3;
	else 
		write << 4;
	
	write.close();
}

int GetImage(HWND hWnd) {

	if (!(isJpeg || isPng || isBmp || isTiff)) {
		MessageBox(hWnd, L"Select a file format", L"Failed", MB_OK);
		return 0;
	}


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
	std::wstring format_{};
	
	if (isPng)
		format_ = L"png";
	else if(isJpeg)
		format_ = L"jpeg";
	else if(isBmp)
		format_ = L"bmp";
	else
		format_ = L"tiff";

	std::wstring format{ L"image/" + format_};

	if(GetEncoderClsid(format.c_str(), &myClsId) == -1)  {
		MessageBox(hWnd, L"Encoding has failed", L"Failed", MB_OK);
		ClearMemory();
	}

	std::wstring name{ L"Screenshots\\Screenshot_" + std::to_wstring(FileCount) + L"." + format_};
	

	FileCount++;

	const WCHAR* FileName = name.c_str();

	image->Save(FileName, &myClsId, NULL);
	delete image;

	GdiplusShutdown(gdiplusToken);
	
	ClearMemory();
}