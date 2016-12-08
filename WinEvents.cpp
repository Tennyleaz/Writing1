#include "stdafx.h"
#include <stdio.h>
#include <string>
#include "Windowsx.h"
#include "WinEvents.h"
#include "Resource.h"
#include "Globals.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// These variables are required by BitBlt.
static HDC hdcScreen;        // DC for entire screen
static BITMAP bmp;           // bitmap data structure
static BOOL fBlt;            // TRUE if BitBlt occurred
static BOOL fScroll;         // TRUE if scrolling occurred
static BOOL fSize;           // TRUE if fBlt & WM_SIZE
static HBITMAP hBmp;         //bitmap for memory DC
static RECT rect;

static int mouseX, mouseY;
static bool mouseHasDown;
static bool isInit;
static int strokeCount;

static HMODULE dllHandler;
static DWORD serviceID;
static json J;
static list<stroke> strokes;
static stroke newStroke;
static WCHAR candidates[6];
static HPEN hpen;

//the writing function pointers
static CreateServiceFunc MyCreateService;
static DestroyServiceFunc MyDestroyService;
static SetReturnHandle MySetReturnHandle;
static AddStrokes MyAddStrokes;
static StartRecog MyStartRecog;
static GetRecogResultJSON MyGetResultJ;
static ClearStrokes MyClearStrokes;
static GetStrokeCount MyGetStrokeCount;

LRESULT WM_CloseEvent(Parameter& param)
{
	DeleteObject(hpen);  //delete pen
	Clean(param.hWnd_);
	MyClearStrokes(serviceID);
	MyDestroyService(serviceID);
	FreeLibrary(dllHandler);
	DestroyWindow(param.hWnd_);
	return 0;
}

LRESULT WM_CreateEvent(Parameter& param)
{
	//getting the dll and the function in dll
	dllHandler = LoadLibrary(TEXT("PPRecognizer.dll"));
	if (!dllHandler) {
		MessageBox(NULL, TEXT("Cannot load PPRecognizer.dll"), NULL, MB_OK);
		return -1;
	}

	MyCreateService = (CreateServiceFunc)GetProcAddress(dllHandler, "CreateService");
	if (!MyCreateService) {
		MessageBox(NULL, TEXT("Cannot load function CreateService"), NULL, MB_OK);
		return -1;
	}

	MyDestroyService = (DestroyServiceFunc)GetProcAddress(dllHandler, "DestroyService");
	if (!MyDestroyService) {
		MessageBox(NULL, TEXT("Cannot load function DestroyService"), NULL, MB_OK);
		return -1;
	}

	MySetReturnHandle = (SetReturnHandle)GetProcAddress(dllHandler, "SetReturnHandle");
	if (!MySetReturnHandle) {
		MessageBox(NULL, TEXT("Cannot load function SetReturnHandle"), NULL, MB_OK);
		return -1;
	}

	MyAddStrokes = (AddStrokes)GetProcAddress(dllHandler, "AddStrokes");
	if (!MyAddStrokes) {
		MessageBox(NULL, TEXT("Cannot load function AddStrokes"), NULL, MB_OK);
		return -1;
	}

	MyStartRecog = (StartRecog)GetProcAddress(dllHandler, "StartRecog");
	if (!MyStartRecog) {
		MessageBox(NULL, TEXT("Cannot load function StartRecog"), NULL, MB_OK);
		return -1;
	}

	MyGetResultJ = (GetRecogResultJSON)GetProcAddress(dllHandler, "GetRecogResultJSON");
	if (!MyGetResultJ) {
		MessageBox(NULL, TEXT("Cannot load function GetRecogResultJSON"), NULL, MB_OK);
		return -1;
	}

	MyClearStrokes = (ClearStrokes)GetProcAddress(dllHandler, "ClearStrokes");
	if (!MyClearStrokes) {
		MessageBox(NULL, TEXT("Cannot load function ClearStrokes"), NULL, MB_OK);
		return -1;
	}

	MyGetStrokeCount = (GetStrokeCount)GetProcAddress(dllHandler, "GetStrokeCount");
	if (!MyGetStrokeCount) {
		MessageBox(NULL, TEXT("Cannot load function GetStrokeCount"), NULL, MB_OK);
		return -1;
	}


	//init
	serviceID = MyCreateService();
	MySetReturnHandle(serviceID, param.hWnd_);

	//set button text to \0
	for (int i = 0; i < 6; i++)
		candidates[i] = '\0';

	//create my pen
	hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));	

	// Create a normal DC and a memory DC for the entire 
	// screen. The normal DC provides a snapshot of the 
	// screen contents. The memory DC keeps a copy of this 
	// snapshot in the associated bitmap. 
	hdcScreen = CreateDC(L"DISPLAY", (PCTSTR)NULL, (PCTSTR)NULL, (CONST DEVMODE *) NULL);

	// Retrieve the metrics for the bitmap associated with the 
	// regular device context.  bmp size=1920*1080
	bmp.bmBitsPixel = (BYTE)GetDeviceCaps(hdcScreen, BITSPIXEL);
	bmp.bmPlanes = (BYTE)GetDeviceCaps(hdcScreen, PLANES);
	bmp.bmWidth = GetDeviceCaps(hdcScreen, HORZRES);
	bmp.bmHeight = GetDeviceCaps(hdcScreen, VERTRES);

	// The width must be byte-aligned. 
	bmp.bmWidthBytes = ((bmp.bmWidth + 15) &~15) / 8;
	
	DeleteDC(hdcScreen);
	return 0;
}

LRESULT WM_CommandEvent(Parameter & param)
{
	int wmId = LOWORD(param.wParam_);
	// 剖析功能表選取項目: 
	switch (wmId)
	{
	case IDM_ABOUT:  //「關於」視窗
		DialogBox(Globals::var().hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), param.hWnd_, About);
		break;
	case IDM_EXIT:  //bye bye
		SendMessage(param.hWnd_, WM_CLOSE, NULL, NULL);
		break;
		DestroyWindow(param.hWnd_);
		break;
	case 120:  //clear button
		Clean(param.hWnd_);
		break;
	case 121:  //1st candidate word
		SendWstring(0, param.hWnd_);
		break;
	case 122:  //2nd candidate word
		SendWstring(1, param.hWnd_);
		break;
	case 123:  //3rd candidate word
		SendWstring(2, param.hWnd_);
		break;
	case 124:  //4th candidate word
		SendWstring(3, param.hWnd_);
		break;
	case 125:  //5th candidate word
		SendWstring(4, param.hWnd_);
		break;
	case 126:  //6th candidate word
		SendWstring(5, param.hWnd_);
		break;
	case ID_DEBUG:
		Globals::var().debugMode = !Globals::var().debugMode;
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	}
	return 0;
}

LRESULT WM_PaintEvent(Parameter& param)
{
	PAINTSTRUCT ps;
	RECT clientRec = rect;
	HDC hdc = BeginPaint(param.hWnd_, &ps);  //this will return display device id

	GetClientRect(param.hWnd_, &clientRec);
	HDC memoryDC = CreateCompatibleDC(hdc);
	hBmp = CreateCompatibleBitmap(hdc, clientRec.right, clientRec.bottom);  // Create a bitmap big enough for our client rectangle.

	// Select the bitmap into the off-screen DC.
	SelectObject(memoryDC, hBmp);

	// Erase the background.
	HBRUSH hbrBkGnd;
	hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(memoryDC, &clientRec, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	//do only once at window start
	if (!isInit)
	{
		isInit = true;

		//make my window that cannot be activated
		//this help when I click on buttons
		int exstyle = GetWindowLong(param.hWnd_, GWL_EXSTYLE);
		exstyle |= WS_EX_NOACTIVATE;
		SetWindowLong(param.hWnd_, GWL_EXSTYLE, exstyle);
	}


	// TODO: 在此加入任何使用 hdc 的繪圖程式碼...
	//---------------------------------------------------------------

	//select my custom pen
	SelectObject(memoryDC, hpen);

	//draw the newStroke
	if (newStroke.points.size() > 0)
	{
		for (int i = 0; i < newStroke.points.size() - 1; i++) //do not draw the last point (-1,-1)
		{
			if (!mouseHasDown && i == newStroke.points.size() - 2)
				break;

			MoveToEx(memoryDC, newStroke.points[i].x, newStroke.points[i].y, NULL);
			LineTo(memoryDC, newStroke.points[i + 1].x, newStroke.points[i + 1].y);
		}
	}

	//draw the resting strokes
	for (auto it : strokes)
	{
		if (it.points.size() > 0)
		{
			for (int i = 0; i < it.points.size() - 1; i++)  
			{
				if (i == it.points.size() - 2)
					break;

				MoveToEx(memoryDC, it.points[i].x, it.points[i].y, NULL);
				LineTo(memoryDC, it.points[i + 1].x, it.points[i + 1].y);
			}
		}
	}

	if (Globals::var().debugMode)  //print debugging word candidate
	{
		for (int i = 0; i < 6; i++)
		{
			if(candidates[i] == '\0')
				continue;

			WCHAR * warray = new WCHAR[2];
			warray[0] = candidates[i];
			warray[1] = '\0';
			TextOut(memoryDC, 100 + i*25, 100, warray, 1);
			TextOut(memoryDC, 55, 100, L"候選:", 3);
		}
	}

	string s = "stroke count=" + to_string(strokeCount);
	TextOutA(memoryDC, 10, 390, s.c_str(), s.size());


	//---------------------------------------------------------------
	// Blt the changes to the screen DC.
	BitBlt(hdc,
		clientRec.left, clientRec.top,
		clientRec.right - clientRec.left, clientRec.bottom - clientRec.top,
		memoryDC,
		0, 0,
		SRCCOPY);

	DeleteDC(memoryDC);  //release a memory DC
	DeleteObject(hBmp);
	EndPaint(param.hWnd_, &ps);

	return 0;
}

LRESULT WM_MouseMoveEvent(Parameter & param)
{
	mouseX = GET_X_LPARAM(param.lParam_);
	mouseY = GET_Y_LPARAM(param.lParam_);

	if (mouseHasDown)  //if mouse L is down
	{
		PTTYPE p(mouseX, mouseY);  //add a new point to newStroke
		newStroke.points.push_back(p);
	}

	InvalidateRect(param.hWnd_, NULL, FALSE);
	return 0;
}

LRESULT WM_LButtonDownEvent(Parameter & param)
{
	if (param.wParam_ && !mouseHasDown)  //if mouse L is down
	{
		SetCapture(param.hWnd_);  //capture mouse even outside window

		newStroke.points.clear();
		mouseHasDown = true;

		mouseX = GET_X_LPARAM(param.lParam_);
		mouseY = GET_Y_LPARAM(param.lParam_);
		PTTYPE p(mouseX, mouseY);  //this is the first point in newStroke
		newStroke.points.push_back(p);
	}
	return 0;
}

LRESULT WM_LButtonUpEvent(Parameter & param)
{
	if (!param.wParam_)
	{
		ReleaseCapture();  //stop capture mouse

		InvalidateRect(param.hWnd_, NULL, FALSE);
		mouseHasDown = false;

		PTTYPE p(-1, -1);  //add a (-1,-1) to the end of newStroke
		newStroke.points.push_back(p);

		strokes.push_back(newStroke);

		vector<PTTYPE> temp;
		for (auto it : strokes)  //doing for every stroke
		{
			temp.insert(temp.end(), it.points.begin(), it.points.end());  //dump all iterators into temp
			PTTYPE * pArray = &it.points[0];
			MyAddStrokes(serviceID, pArray, it.points.size());
		}

		strokeCount = MyGetStrokeCount(serviceID);
		MyStartRecog(serviceID, NULL, 0);
	}
	return 0;
}

LRESULT WM_RecogComplete(Parameter & param)
{
	if (param.lParam_ == NULL)
		return false;

	char * result;
	unsigned int length;
	length = MyGetResultJ(serviceID, param.lParam_, NULL, 0);  //get result length first
	result = new char[length];
	MyGetResultJ(serviceID, param.lParam_, result, length);  //then get result into a buffer

	J = json::parse(result);

	MyClearStrokes(serviceID);

	json J2 = J["RecogResults"];
	
	if (J2.size() > 0)
	{
		std::string s2 = J2.dump();
		s2 = s2.substr(1, s2.size() - 2);  //去頭尾的 [ ] 符號
		json J3 = json::parse(s2.c_str());  //get the "RecogResults" value string

		if (J3.size() <= 1)
			return false;

		int taskType = J3["taskType"];
		if (taskType != 1)  //don't continue if type != 1
			return false;


		json J4 = J3["word"];
		s2 = J4.dump();
		int size = J4.size();
		string s = "\nJ4=" + s2;
		OutputDebugStringA(s.c_str());

		for (int i = 0; i < size; i++)
		{
			WCHAR * warray = new WCHAR[2];
			warray[0] = static_cast<WCHAR>(J4[i]);
			candidates[i] = warray[0];
			warray[1] = '\0';
			wstring ws(warray);
			SendMessage(Globals::var().myButton[i], WM_SETTEXT, 0, (LPARAM)ws.c_str());  //set the button text
		}

		if (size < 6)  //set the rest button to empty
		{
			for (int i = 6; i > size; i--)
			{
				candidates[i-1] = '\0';
				SendMessage(Globals::var().myButton[i-1], WM_SETTEXT, 0, (LPARAM)"");
			}
		}
	}

	return true;
}

LRESULT WM_DestroyEvent(Parameter& param)
{
	PostQuitMessage(0);
	return 0;
}


// [關於] 方塊的訊息處理常式。
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


void Clean(HWND hwnd)
{
	newStroke.points.clear();
	for (auto a : strokes)
	{
		a.points.clear();
	}
	strokes.clear();
	J.clear();
	for (int i = 0; i < 6; i++)  //set each button to empty
	{
		SendMessage(Globals::var().myButton[i], WM_SETTEXT, 0, (LPARAM)L"");
		candidates[i] = '\0';
	}
	strokeCount = 0;
	InvalidateRect(hwnd, NULL, FALSE);
}

//send a wstring to given button
UINT SendWstring(int position, HWND hwnd)
{
	if (candidates[position] == '\0')
		return 0;

	HWND handle = GetForegroundWindow();
	bool success = SetForegroundWindow(handle);
	if (success)
	{
		//wstring ws = L"hello, こんにちは";
		WCHAR * warray = new WCHAR[2];
		warray[0] = candidates[position];
		warray[1] = '\0';
		wstring ws(warray);

		// Construct list of inputs in order to send them through a single SendInput call at the end.
		vector<INPUT> inputs;

		// Loop through each Unicode character in the string.
		for (auto c : ws)
		{
			// First send a key down (keyUp=false), then a key up (keyUp=true).
			bool bools[2] = { false, true };
			for (bool keyUp : bools)
			{
				// INPUT is a multi-purpose structure which can be used 
				// for synthesizing keystrokes, mouse motions, and button clicks.
				INPUT input;
				input.type = INPUT_KEYBOARD;
				input.ki.wVk = 0;
				input.ki.wScan = c;  //set the input char
				input.ki.dwFlags = KEYEVENTF_UNICODE | (keyUp ? KEYEVENTF_KEYUP : 0);
				input.ki.dwExtraInfo = GetMessageExtraInfo();

				// Add to the list (to be sent later).
				inputs.push_back(input);
			}
		}

		//set the button text to ""
		Clean(hwnd);

		// Send all inputs together using a Windows API call.
		INPUT * ptr = &inputs[0];
		return SendInput((UINT)inputs.size(), ptr, sizeof(INPUT));
	}

	return 0;
}
