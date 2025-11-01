#pragma once
#include "Problem.hpp"
#include "ProblemManager.hpp"
#include "SaveDataManager.hpp"

class ConfigManager;

class GameState
{
   private:
	SaveDataManager& m_saveManager;
	ConfigManager& m_configManager;
	ProblemManager& m_problemManager;

   public:
	Array<Problem> problems;   // 現在プレイ中の問題群
	size_t currentIndex = 0;   // 現在の問題番号
	int32 score = 0;		   // 現在セッション内スコア
	int32 correctCount = 0;	   // 正解数
	bool answered = false;	   // 現在の問題を回答済みか
	int32 selectedGrade = -1;  // 選択された段位

	// コンストラクタ
	explicit GameState(SaveDataManager& saveManager,
					   ConfigManager& configManager,
					   ProblemManager& problemManager);

	// 初期化
	bool initialize();

	void loadFromSaveData();
	void stateReset();
	void stateNext();

	// 進捗・段位関連
	bool updateRank();		// 段位の更新を試みる
	void updateProgress();	// 進捗更新まわりをすべて対応する

	// 表示用の現在段位名を返す
	String currentRankName() const;
	String getRankName(size_t index) const;

	// 段位の進捗状況を返す
	size_t getProgressForGrade(int32 grade) const;

	// 段位選択機能
	void setSelectedGrade(int32 grade);
	void setSelectedHighestGrade();
	int32 getSelectedGrade() const noexcept
	{
		return selectedGrade;
	}
	bool isGradeAvailable(int32 grade) const;
	bool isValidProblems();

	// 昇段条件を満たしているか確認
	bool canPromote() const;
};
