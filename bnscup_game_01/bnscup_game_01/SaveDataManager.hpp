#pragma once
#include "Config.hpp"
#include "GameConstants.hpp"

class SaveDataManager
{
   private:
	SaveData m_saveData;
	FilePath m_saveDataPath;

   public:
	SaveDataManager() = default;
	~SaveDataManager() = default;

	// 初期化（ファイル読み込み）
	bool initialize(const StringView& path);

	// セーブデータの保存
	bool save() const;

	// セーブデータへのアクセス
	const SaveData& getData() const noexcept
	{
		return m_saveData;
	}
	SaveData& getData() noexcept
	{
		return m_saveData;
	}

	// 段位関連
	size_t getCurrentRank() const noexcept
	{
		return m_saveData.rankIndex;
	}
	void setCurrentRank(size_t rank)
	{
		if (rank > GameConstants::MAX_RANK_INDEX)
		{
			Console << U"[SaveDataManager] Invalid rank: " << rank
					<< U", max is: " << GameConstants::MAX_RANK_INDEX;
			return;
		}
		m_saveData.rankIndex = rank;
	}

	// 進捗関連
	size_t getGradeProgress(size_t grade) const
	{
		return (grade < m_saveData.gradeProgress.size())
				   ? m_saveData.gradeProgress[grade]
				   : 0;
	}

	void incrementGradeProgress(size_t grade)
	{
		if (grade >= m_saveData.gradeProgress.size())
		{
			Console << U"[SaveDataManager] Invalid grade index for increment: "
					<< grade;
			return;
		}
		m_saveData.gradeProgress[grade]++;
	}

	// スコア関連
	int32 getTotalScore() const noexcept
	{
		return m_saveData.totalScore;
	}
	void addScore(int32 score)
	{
		m_saveData.totalScore += score;
	}

	// 問題クリア状況
	bool isProblemCleared(size_t index) const
	{
		if (index >= m_saveData.problemStatus.size())
		{
			Console << U"[SaveDataManager] Invalid problem index: " << index;
			return false;
		}
		return m_saveData.problemStatus[index];
	}

	void setProblemCleared(size_t index, bool cleared = true)
	{
		if (index >= m_saveData.problemStatus.size())
		{
			Console << U"[SaveDataManager] Invalid problem index for set: "
					<< index;
			return;
		}
		m_saveData.problemStatus[index] = cleared;
	}
};
