#pragma once

#include "Problem.hpp"

struct GameState
{
	Array<Problem> problems;  // 現在プレイ中の問題群
	size_t currentIndex = 0;  // 現在の問題番号
	int32 score = 0;		  // 現在セッション内スコア
	int32 correctCount = 0;	  // 正解数
	bool answered = false;	  // 現在の問題を回答済みか

	void state_reset();
	void state_next();
};
