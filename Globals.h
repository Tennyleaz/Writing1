#pragma once
#define MAX_LOADSTRING 100
#include <iostream>

// �����ܼ�: 
class Globals
{
public:
	HINSTANCE hInst;                                // �ثe�������
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
