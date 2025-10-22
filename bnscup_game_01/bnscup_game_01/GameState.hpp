#pragma once

#include "Problem.hpp"

struct GameState
{
	Array<Problem> problems;  // 現在プレイ中の問題群
	size_t currentIndex = 0;  // 現在の問題番号
	int32 score = 0;		  // 現在セッション内スコア
	int32 correctCount = 0;	  // 正解数
	bool answered = false;	  // 現在の問題を回答済みか

	// 追加: 段位管理（表示用）
	Array<String> ranks{U"特待生",	 U"名人初段", U"名人二段", U"名人三段",
						U"名人四段", U"名人五段", U"名人六段", U"名人七段",
						U"名人八段", U"名人九段", U"名人十段", U"永世名人"};
	size_t rankIndex = 0;  // 0 = 特待生

	void state_reset();
	void state_next();

	// 表示用の現在段位名を返す
	String currentRankName() const
	{
		if (ranks.isEmpty()) return U"";
		return ranks[Min(rankIndex, ranks.size() - 1)];
	}
};
