#include "Problem.hpp"

bool Problem::isValid() const
{
	// 基本データの検証
	if (id.isEmpty() || text.isEmpty())
	{
		return false;
	}

	// 季語関連の検証
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
		if (kigo.isEmpty())
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

	// 段位の検証
	if (grade < 0 || grade >= Grade::Count)
	{
		return false;
	}

	// フリガナとリズム情報の検証
	if (!ruby.isEmpty() && text.isEmpty())
	{
		return false;  // テキストが空なのにフリガナがある
	}

	if (!rhythm.isEmpty())
	{
		// リズム情報は "5-7-5" のような形式
		const auto parts = rhythm.split(U'-');
		if (parts.size() != 3)
		{
			return false;
		}
	}

	return true;
}
