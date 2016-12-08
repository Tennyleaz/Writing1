#pragma once
#define MAX_LOADSTRING 100
#include <iostream>

// 全域變數: 
class Globals
{
public:
	HINSTANCE hInst;                                // 目前執行個體
	HWND myButton[6];
	HWND clearButton;
	HWND hWndFather;
	HMODULE dllHandler;

	std::string events;

	//----------------------------------------------------------------------------------------------
	bool debugMode;
	//----------------------------------------------------------------------------------------------

	Globals();
	static Globals& var();
};
