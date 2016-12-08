#pragma once
#include <vector>

struct PTTYPE
{
	short x;
	short y;
	PTTYPE(double x, double y)
	{
		this->x = (short)x;
		this->y = (short)y;
	}
};

struct stroke
{
	std::vector<PTTYPE> points;
};

struct RECOG_CONFIG
{
	//Data Path
	char AI_Path[260];
	char AI_Multi_Path[260];
	char Learn_Str_Path[260];
	char User_Supplement_Path[260];
	int ResolutionWidth;
	int ResolutionHeight;

	//RecogEngine
	int recogEngine;
	BOOL MultiRecog;
	int CandidateNum;
	WCHAR PrevChar;
	int MyaTable;

	//RecogType
	BOOL ChineseCommon;
	BOOL ChineseRare;
	BOOL EnglishUpperAlpha;
	BOOL EnglishLowerAlpha;
	BOOL JapaneseKatakana;
	BOOL JapaneseHiragana;
	BOOL Numerics;
	BOOL Symbols;
	BOOL Gestures;
	BOOL KoreaCommon;
	BOOL KoreaRare;
	int recogType;

	//ShapeMode
	BOOL UseHalfShape;
	BOOL TurnOnHK;
	BOOL TurnOnFullUnicode;
	BOOL TurnOnBig5Only;
	BOOL TurnOnGBOnly;
	BOOL TurnOnGBBOnly;
	BOOL TurnOnRunning;
};