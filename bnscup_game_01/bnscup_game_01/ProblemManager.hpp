#pragma once

#include "Problem.hpp"

class ProblemManager
{
   private:
	Array<Problem> m_problems;

   public:
	bool loadFromJSON(const FilePath& path);
	const Array<Problem>& getProblems() const noexcept;
	const Problem& getProblem(size_t index) const;
	size_t size() const noexcept;
};
