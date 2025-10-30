#include "GameState.hpp"
#include "ConfigManager.hpp"
#include "SaveDataManager.hpp"

GameState::GameState(SaveDataManager& saveManager, ConfigManager& configManager)
	: m_saveManager(saveManager), m_configManager(configManager)
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
	saveData.problemStatus.resize(problems.size(), false);
}

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

String GameState::currentRankName() const
{
	if (ranks.isEmpty()) return U"";
	return ranks[Min(m_saveManager.getData().rankIndex, ranks.size() - 1)];
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
	if (saveData.rankIndex >= ranks.size() - 1)
	{
		return false;  // すでに最高段位
	}

	return canPromote();
}

bool GameState::canPromote() const
{
	const auto& saveData = m_saveManager.getData();
	const size_t requiredCorrect = 3;  // 昇段に必要な正解数

	// 段位に応じた昇段条件
	switch (saveData.rankIndex)
	{
		case 0:	 // 特待生から名人へ
			return saveData.gradeProgress[Grade::Trainee] >= requiredCorrect;

		case 1:	 // 名人から達人へ
			return saveData.gradeProgress[Grade::Master] >= requiredCorrect;

		case 2:	 // 達人から俳人へ
			return saveData.gradeProgress[Grade::Expert] >= requiredCorrect;

		default:
			return false;
	}
}
