#include "stdafx.h"
#include "globals.h"

Globals::Globals()
{
	debugMode = true;
}

Globals & Globals::var()
{
	// TODO: 於此處插入傳回陳述式
	static Globals g;
	return g;
}
