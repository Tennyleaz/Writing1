#include "stdafx.h"
#include "globals.h"

Globals::Globals()
{
	debugMode = false;
}

Globals & Globals::var()
{
	// TODO: �󦹳B���J�Ǧ^���z��
	static Globals g;
	return g;
}
