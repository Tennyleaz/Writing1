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