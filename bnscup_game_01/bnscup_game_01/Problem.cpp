#include "stdafx.h"

#include "Problem.hpp"

bool Problem::isValid() const
{
	if (hasKigo)
	{
		if (kigoStart < 0 || kigoEnd < kigoStart)
		{
			return false;
		}
		if (kigoEnd >= static_cast<int32>(text.size()))
		{
			return false;
		}
	}
	else
	{
		if (!(kigoStart == -1 && kigoEnd == -1))
		{
			return false;
		}
	}
	return true;
}
