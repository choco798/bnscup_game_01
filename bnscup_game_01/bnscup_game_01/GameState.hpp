#pragma once
#include "Config.hpp"
#include "Problem.hpp"
#include "SaveDataManager.hpp"

class ConfigManager;

class GameState
{
   private:
	SaveDataManager& m_saveManager;
	ConfigManager& m_configManager;

   public:
	Array<Problem> problems;  // 現在プレイ中の問題群
	size_t currentIndex = 0;  // 現在の問題番号
	int32 score = 0;		  // 現在セッション内スコア
	int32 correctCount = 0;	  // 正解数
	bool answered = false;	  // 現在の問題を回答済みか

	// 段位管理
	const Array<String> ranks{
		U"特待生",	// Grade::Trainee
		U"名人",	// Grade::Master
		U"達人",	// Grade::Expert
		U"俳人"		// 最高段位
	};

	// コンストラクタ
	explicit GameState(SaveDataManager& saveManager,
					   ConfigManager& configManager);

	// 初期化
	bool initialize();

	void loadFromSaveData();
	void state_reset();
	void state_next();
	bool updateRank();	// 段位の更新を試みる
	void updateProgress(const Problem& problem, bool correct);	// 進捗更新

	// 表示用の現在段位名を返す
	String currentRankName() const;

	// 段位の進捗状況を返す
	size_t getProgressForGrade(int32 grade) const;

	// 昇段条件を満たしているか確認
	bool canPromote() const;
};
