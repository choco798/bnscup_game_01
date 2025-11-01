#pragma once
#include "Problem.hpp"

class ProblemManager
{
   private:
	Array<Problem> m_problems;					   // 全問題
	Array<Problem> m_gradeProblems[ProblemGrade::Count];  // 段位別問題配列
	int32 m_selectedGrade = -1;					   // 現在選択されている段位

   public:
	ProblemManager() = default;

	// 基本操作
	bool loadFromJSON(const FilePath& path);
	const Array<Problem>& getProblems() const noexcept
	{
		return m_problems;
	}
	const Problem& getProblem(size_t index) const
	{
		return m_problems.at(index);
	}
	size_t size() const noexcept
	{
		return m_problems.size();
	}

	// 段位システム機能
	const Array<Problem>& getProblemsForGrade(int32 grade) const;
	void shuffleProblemsForGrade(int32 grade);
	size_t getCompletedCountForGrade(int32 grade) const;
	void filterProblemsByGrade();  // 問題を段位別に振り分け

	// 段位選択機能
	bool isGradeAvailable(int32 grade, size_t playerRank) const;
	void setSelectedGrade(int32 grade);
	int32 getSelectedGrade() const noexcept
	{
		return m_selectedGrade;
	}
};
