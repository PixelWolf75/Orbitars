// Orbitars with Catch.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Orbitars with Catch.h"
#include "IStars.h"
#include "IScreen.h"
#include "sql/sqlite3.h"
#include <gdiplus.h>

//#include "catch2\catch.hpp"

//#define CONFIG_CATCH_MAIN
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR					gdiplusToken;
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	//game_state_t sGame = std::make_unique<Client_ScreenINFO>("SQL_DB",8080, 0);
	Client_ScreenINFO_t sScreen = std::make_unique<Client_ScreenINFO>("eric", "SQL_DB", 8080, -1);
	CODE::SCREENS::Objects::ScreenController::set_screen("Splash", std::move(sScreen));

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ORBITARSWITHCATCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ORBITARSWITHCATCH));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ORBITARSWITHCATCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ORBITARSWITHCATCH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

VOID CALLBACK TimerProc(HWND hWnd, UINT, UINT_PTR, DWORD)
{
	//g_pStar->Update(hWnd);
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	CODE::SCREENS::Objects::ScreenController::Update(hWnd, ptCursor);

	std::vector<eKeyboard> vecKey_Presses;

	if (GetAsyncKeyState(VK_UP))
	{
		vecKey_Presses.push_back(eKeyboard::Up);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		vecKey_Presses.push_back(eKeyboard::Down);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		vecKey_Presses.push_back(eKeyboard::Left);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		vecKey_Presses.push_back(eKeyboard::Right);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		vecKey_Presses.push_back(eKeyboard::Space);
	}
	if (GetAsyncKeyState(VK_RETURN))
	{
		vecKey_Presses.push_back(eKeyboard::Enter);
	}

	CODE::SCREENS::Objects::ScreenController::Input(vecKey_Presses);

}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   DWORD dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetTimer(hWnd, 0, 1000/60, TimerProc);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::vector<eKeyboard> vecKey_Presses;
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		//g_pStar->Render(hdc, hWnd);

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		//Gets the size of the screen and puts it in the rcClient

		int width = rcClient.right - rcClient.left;
		int height = rcClient.bottom - rcClient.top;
	
		size_t len = width * height;
		

		HDC hdcMem = CreateCompatibleDC(hdc);
		
		uint8_t* lpBitmapBits;

		BITMAPINFO bi;
		ZeroMemory(&bi, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = width;
		bi.bmiHeader.biHeight = -height;  //negative so (0,0) is at top left
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 32;


		HBITMAP hBitmap = ::CreateDIBSection(hdcMem, &bi, DIB_RGB_COLORS, (VOID**)&lpBitmapBits, NULL, 0);
		if (!hBitmap) {
			EndPaint(hWnd, &ps);
			return 0;
		}

		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

		//HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
		Window_Properties wpWinProp = { hdcMem, lpBitmapBits, width, height };
		
	
		//Blank bitmap;
		int pitch = width;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				//int r = 0;
				//int g = y * 255 / height;
				//int b = x * 255 / width;
				((COLORREF*)lpBitmapBits)[x + y * pitch] = 0xff000000;// | RGB(r, g, b);
			}
		}
		


		//Creates an off screen device context to do all the drawing to

		CODE::SCREENS::Objects::ScreenController::Render(hWnd, wpWinProp);

		//for (size_t j = 1; j < rcClient.bottom + 1 - rcClient.top; j++)
		//{

		//	for (size_t i = 0; i < rcClient.right - rcClient.left; i++)
		//	{
		//		crBitmapBacking[i * j] = 0xFF0000FF;
		//	}
		//}

		//HBITMAP hbmMap = CreateBitmap((rcClient.right - rcClient.left), // X dimension
		//	(rcClient.bottom - rcClient.top), // Y dimension
		//	1, // Color planes?
		//	8 * 4, // 8 bits * 4 bytes (R,G,B,A)
		//	(void*)crBitmapBacking); //Memory


		BitBlt(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);
		//Transfers all the drawing we've done onto the main device context
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		
		//Deletes the offscreen device context

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
	{
		eKeyboard eKey_Press = eKeyboard::None;
		
		/*
		if (GetAsyncKeyState(VK_UP))
		{
			vecKey_Presses.push_back(eKeyboard::Up);
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			vecKey_Presses.push_back(eKeyboard::Down);
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			vecKey_Presses.push_back(eKeyboard::Left);
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			vecKey_Presses.push_back(eKeyboard::Right);
		}
		if (GetAsyncKeyState(VK_SPACE))
		{
			vecKey_Presses.push_back(eKeyboard::Space);
		}
		if (GetAsyncKeyState(VK_RETURN))
		{
			vecKey_Presses.push_back(eKeyboard::Enter);
		}


		switch (wParam)
		{
		case VK_DOWN:
			eKey_Press = eKeyboard::Down;
			break;
		case VK_UP:
			eKey_Press = eKeyboard::Up;
			break;
		case VK_LEFT:
			eKey_Press = eKeyboard::Left;
			break;
		case VK_RIGHT:
			eKey_Press = eKeyboard::Right;
			break;
		case VK_SPACE:
			eKey_Press = eKeyboard::Space;
			break;
		case VK_RETURN:
			eKey_Press = eKeyboard::Enter;
			break;
		default:
			break;
		}
		CODE::SCREENS::Objects::ScreenController::Input(vecKey_Presses);
		*/
		break;
	}
	case WM_LBUTTONDOWN:
	{
		eKeyboard eLKey_Press = eKeyboard::Left_Click;
		vecKey_Presses.push_back(eKeyboard::Left_Click);
		CODE::SCREENS::Objects::ScreenController::Input(vecKey_Presses);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		eKeyboard eRKey_Press = eKeyboard::Right_Click;
		vecKey_Presses.push_back(eKeyboard::Right_Click);
		CODE::SCREENS::Objects::ScreenController::Input(vecKey_Presses);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
