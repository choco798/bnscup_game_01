#pragma once
#include "Problem.hpp"

class ProblemManager
{
   private:
	Array<Problem> m_problems;					   // 全問題
	Array<Problem> m_gradeProblems[Grade::Count];  // 段位別問題配列

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
};
