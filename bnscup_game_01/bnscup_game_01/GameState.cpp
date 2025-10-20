#include "stdafx.h"
#include "GameState.hpp"

void GameState::state_reset()
{
	currentIndex = 0;
	score = 0;
	correctCount = 0;
	answered = false;
}

void GameState::state_next()
{
	if (currentIndex + 1 < problems.size())
	{
		++currentIndex;
		answered = false;
	}
	else
	{
		// 末尾を超えた場合の扱いは外側の遷移ロジックに任せる
		currentIndex = problems.size();
		answered = false;
	}
}
