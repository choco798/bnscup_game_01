#include "GameState.hpp"
#include "ConfigManager.hpp"
#include "GameConstants.hpp"
#include "SaveDataManager.hpp"


GameState::GameState(SaveDataManager& saveManager, ConfigManager& configManager,
					 ProblemManager& problemManager)
	: m_saveManager(saveManager),
	  m_configManager(configManager),
	  m_problemManager(problemManager)
{
}

bool GameState::initialize()
{
	loadFromSaveData();
	return true;
}

void GameState::loadFromSaveData()
{
	// セーブデータから段位情報を復元
	auto& saveData = m_saveManager.getData();
	saveData.problemStatus.resize(m_problemManager.getProblems().size(), false);
}

void GameState::stateReset()
{
	currentIndex = 0;
	score = GameConstants::INITIAL_SCORE;
	correctCount = GameConstants::INITIAL_CORRECT_COUNT;
	answered = false;

	// 選択された段位の問題を設定
	if (selectedGrade >= 0 && selectedGrade < ProblemGrade::Count)
	{
		// 問題を問題管理から取得
		const Array<Problem>& gradeProblems =
			m_problemManager.getProblemsForGrade(selectedGrade);
		problems = gradeProblems;

		// 問題順をシャッフル
		if (GameConstants::Gameplay::DEFAULT_SHUFFLE_ENABLED &&
			!problems.isEmpty())
		{
			problems.shuffle();
		}
	}
}

void GameState::stateNext()
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

String GameState::currentRankName() const
{
	return getRankName(m_saveManager.getData().rankIndex);
}

String GameState::getRankName(size_t index) const
{
	return GameConstants::RankNames::getRankName(index);
}

size_t GameState::getProgressForGrade(int32 grade) const
{
	return m_saveManager.getData().gradeProgress[grade];
}

void GameState::updateProgress(const Problem& problem, bool correct)
{
	if (correct)
	{
		// 正解した問題の段位の進捗を更新
		auto& saveData = m_saveManager.getData();
		saveData.gradeProgress[problem.grade]++;
		saveData.totalScore += score;

		// 問題のクリア状態を更新
		if (currentIndex < saveData.problemStatus.size())
		{
			saveData.problemStatus[currentIndex] = true;
		}

		// セーブデータを保存
		m_saveManager.save();

		// 昇段判定
		if (updateRank())
		{
			saveData.rankIndex++;
			m_saveManager.save();
		}
	}
}

bool GameState::updateRank()
{
	auto& saveData = m_saveManager.getData();
	if (saveData.rankIndex >= GameConstants::MAX_RANK_INDEX)
	{
		return false;  // すでに最高段位
	}

	return canPromote();
}

bool GameState::canPromote() const
{
	const auto& saveData = m_saveManager.getData();
	const size_t requiredCorrect = GameConstants::PROMOTION_REQUIRED_CORRECT;

	// 段位に応じた昇段条件
	switch (saveData.rankIndex)
	{
		case 0:	 // 特待生から名人へ
			return saveData.gradeProgress[ProblemGrade::Trainee] >=
				   requiredCorrect;

		case 1:	 // 名人から達人へ
			return saveData.gradeProgress[ProblemGrade::Master] >=
				   requiredCorrect;

		case 2:	 // 達人から俳人へ
			return saveData.gradeProgress[ProblemGrade::Expert] >=
				   requiredCorrect;

		default:
			return false;
	}
}

bool GameState::isGradeAvailable(int32 grade) const
{
	// 無効な段位は使用不可
	if (grade < 0 || grade >= ProblemGrade::Count)
	{
		return false;
	}

	const auto& saveData = m_saveManager.getData();

	// プレイヤーの現在の段位より下の段位は常にプレイ可能
	return grade <= static_cast<int32>(saveData.rankIndex);
}

bool GameState::isValidProblems()
{
	return m_problemManager.getProblemsForGrade(selectedGrade).size() > 0;
}

void GameState::setSelectedGrade(int32 grade)
{
	// 無効な段位は選択できない
	if (grade != GameConstants::DEFAULT_SELECTED_GRADE &&
		(grade < 0 || grade >= ProblemGrade::Count))
	{
		return;
	}

	// 利用可能な段位のみ選択可能
	if (grade != GameConstants::DEFAULT_SELECTED_GRADE &&
		!isGradeAvailable(grade))
	{
		return;
	}

	selectedGrade = grade;
}

void GameState::setSelectedHighestGrade()
{
	const auto& saveData = m_saveManager.getData();

	// プレイヤーの現在の段位より下の段位は常にプレイ可能
	selectedGrade = static_cast<int32>(saveData.rankIndex);
}
