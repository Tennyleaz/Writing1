#pragma once
#include "stdafx.h"
#include "Listener.h"

LRESULT WM_CloseEvent(Parameter& param);
LRESULT WM_CreateEvent(Parameter& param);
LRESULT WM_CommandEvent(Parameter& param);
LRESULT WM_PaintEvent(Parameter& param);
LRESULT WM_MouseMoveEvent(Parameter& param);
LRESULT WM_LButtonDownEvent(Parameter& param);
LRESULT WM_LButtonUpEvent(Parameter& param);
LRESULT WM_RecogComplete(Parameter& param);
LRESULT WM_DestroyEvent(Parameter& param);

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void Clean(HWND hwnd);
UINT SendWstring(int position, HWND hwnd);
//void SetRecogConfig1();

#define STDCALL __cdecl
typedef DWORD (WINAPI *CreateServiceFunc)();
typedef void (WINAPI *DestroyServiceFunc)(unsigned int dwServiceID);
typedef void (WINAPI  *SetReturnHandle)(DWORD dwServiceID, HWND hWnd);
typedef void (WINAPI *AddStrokes)(unsigned int dwServiceID, PTTYPE* pPts, int iCount);
typedef void (WINAPI *StartRecog)(unsigned int dwServiceID, PTTYPE* pPts, int iCount);
typedef unsigned int (WINAPI *GetRecogResultJSON)(unsigned int dwServiceID, unsigned int dwResultID, char* szJSON, unsigned int l);
typedef void (WINAPI *ClearStrokes)(unsigned int dwServiceID);
typedef int (WINAPI * GetStrokeCount)(unsigned int dwServiceID);
//typedef bool (WINAPI * SetRecogConfig)(unsigned int dwServiceID, RECOG_CONFIG recogConfig);