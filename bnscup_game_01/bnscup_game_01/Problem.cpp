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
	return grade >= 0 && grade < Grade::Count;
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

	// リズム情報の形式: "44444!64444444!644444"
	// 数字(音の長さ)と!(休符)の組み合わせ
	bool hasDigit = false;
	bool wasExclamation = false;

	for (const auto ch : rhythm)
	{
		if (InRange(ch, U'0', U'9'))
		{
			hasDigit = true;
			wasExclamation = false;
		}
		else if (ch == U'!')
		{
			if (wasExclamation || !hasDigit)
			{
				// 連続した!または数字の前の!は無効
				return false;
			}
			wasExclamation = true;
		}
		else
		{
			return false;  // 数字と!以外は無効
		}
	}

	// 最後が!で終わっているのは無効
	if (wasExclamation)
	{
		return false;
	}

	// 数字が1つも無いのは無効
	return hasDigit;
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
