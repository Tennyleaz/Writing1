	// Writing1.cpp : 定義應用程式的進入點。
//

#include "stdafx.h"
#include "Writing1.h"
#include "Listener.h"
#include "WinEvents.h"
#include "Globals.h"

#define MAX_LOADSTRING 100

// 全域變數: 
HINSTANCE hInst;                                // 目前執行個體
WCHAR szTitle[MAX_LOADSTRING];                  // 標題列文字
WCHAR szWindowClass[MAX_LOADSTRING];            // 主視窗類別名稱



// 這個程式碼模組中所包含之函式的向前宣告: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此置入程式碼。

	//register the widsows message NOTIFY_SINGLE_RECOG_COMPLETE
	UINT NSRC = RegisterWindowMessage(L"NOTIFY_SINGLE_RECOG_COMPLETE");

	Listener::WinProcMsgListener().hInst = hInstance;
	Listener::WinProcMsgListener().AddEvent(WM_CLOSE, WM_CloseEvent);
	Listener::WinProcMsgListener().AddEvent(WM_COMMAND, WM_CommandEvent);
	Listener::WinProcMsgListener().AddEvent(WM_CREATE, WM_CreateEvent);
	Listener::WinProcMsgListener().AddEvent(WM_PAINT, WM_PaintEvent);
	Listener::WinProcMsgListener().AddEvent(WM_MOUSEMOVE, WM_MouseMoveEvent);
	Listener::WinProcMsgListener().AddEvent(WM_LBUTTONDOWN, WM_LButtonDownEvent);
	Listener::WinProcMsgListener().AddEvent(WM_LBUTTONUP, WM_LButtonUpEvent);
	Listener::WinProcMsgListener().AddEvent(NSRC, WM_RecogComplete);
	Listener::WinProcMsgListener().AddEvent(WM_DESTROY, WM_DestroyEvent);

    // 初始化全域字串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WRITING1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 執行應用程式初始設定: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	//create control buttons
	Globals::var().clearButton = CreateWindow(L"BUTTON", L"Clean", WS_VISIBLE | WS_CHILD | BS_BOTTOM | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 5, 0, 80, 25, Globals::var().hWndFather, (HMENU)120, Globals::var().hInst, NULL);

	Globals::var().myButton[0] = CreateWindow(L"BUTTON", L" ", WS_VISIBLE | WS_CHILD | BS_BOTTOM | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 95, 0, 50, 25, Globals::var().hWndFather, (HMENU)121, Globals::var().hInst, NULL);
	Globals::var().myButton[1] = CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_BOTTOM | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 150, 0, 50, 25, Globals::var().hWndFather, (HMENU)122, Globals::var().hInst, NULL);
	Globals::var().myButton[2] = CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_BOTTOM | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 205, 0, 50, 25, Globals::var().hWndFather, (HMENU)123, Globals::var().hInst, NULL);
	Globals::var().myButton[3] = CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_BOTTOM | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 260, 0, 50, 25, Globals::var().hWndFather, (HMENU)124, Globals::var().hInst, NULL);
	Globals::var().myButton[4] = CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_BOTTOM | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 315, 0, 50, 25, Globals::var().hWndFather, (HMENU)125, Globals::var().hInst, NULL);
	Globals::var().myButton[5] = CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_BOTTOM | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 370, 0, 50, 25, Globals::var().hWndFather, (HMENU)126, Globals::var().hInst, NULL);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WRITING1));

    MSG msg;

    // 主訊息迴圈: 
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
//  函式: MyRegisterClass()
//
//  用途: 註冊視窗類別。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WRITING1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WRITING1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函式: InitInstance(HINSTANCE, int)
//
//   用途: 儲存執行個體控制代碼並且建立主視窗
//
//   註解: 
//
//        在這個函式中，我們會將執行個體控制代碼儲存在全域變數中，
//        並且建立和顯示主程式視窗。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 將執行個體控制代碼儲存在全域變數中

   Globals::var().hWndFather = CreateWindowEx(WS_EX_TOPMOST, szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
      CW_USEDEFAULT, CW_USEDEFAULT, 450, 450, nullptr, nullptr, hInstance, nullptr);

   if (!Globals::var().hWndFather)
   {
      return FALSE;
   }

   ShowWindow(Globals::var().hWndFather, nCmdShow);
   UpdateWindow(Globals::var().hWndFather);

   return TRUE;
}

//
//  函式: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  用途:     處理主視窗的訊息。
//
//  WM_COMMAND  - 處理應用程式功能表
//  WM_PAINT    - 繪製主視窗
//  WM_DESTROY  - 顯示結束訊息然後返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Listener::WinProcMsgListener().Trig(message, Parameter(hWnd, message, wParam, lParam));
}

