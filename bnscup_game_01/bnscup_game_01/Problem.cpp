#include "Problem.hpp"

bool Problem::isBasicDataValid() const
{
	return !id.isEmpty() && !text.isEmpty();
}

bool Problem::isKigoValid() const
{
	if (hasKigo)
	{
		if (kigoStart < 0 || kigoEnd < kigoStart)
		{
			return false;
		}
		if (kigoEnd > static_cast<int32>(text.size()))
		{
			return false;
		}
		return !kigo.isEmpty();
	}
	else
	{
		return kigoStart == -1 && kigoEnd == -1;
	}
}

bool Problem::isGradeValid() const
{
	return grade >= 0 && grade < ProblemGrade::Count;
}

bool Problem::isRubyValid() const
{
	// テキストが空なのにフリガナがあるのは無効
	return ruby.isEmpty() || !text.isEmpty();
}

bool Problem::isRhythmValid() const
{
	if (rhythm.isEmpty())
	{
		return true;
	}

	return true;
}

String Problem::getSeason() const
{
	if (tags.contains(U"春"))
	{
		return U"春";
	}
	if (tags.contains(U"夏"))
	{
		return U"夏";
	}
	if (tags.contains(U"秋"))
	{
		return U"秋";
	}
	if (tags.contains(U"冬"))
	{
		return U"冬";
	}

	return String();
}

bool Problem::isValid() const
{
	bool is_valid = true;
	is_valid &= isBasicDataValid();
	is_valid &= isKigoValid();
	is_valid &= isGradeValid();
	is_valid &= isRubyValid();
	is_valid &= isRhythmValid();
	return is_valid;
}
