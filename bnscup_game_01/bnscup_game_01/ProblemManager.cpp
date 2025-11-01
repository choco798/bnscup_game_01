#include "ProblemManager.hpp"
#include "GameConstants.hpp"

bool ProblemManager::loadFromJSON(const StringView& path)
{
	// 全配列をクリア
	m_problems.clear();
	for (auto& gradeArray : m_gradeProblems)
	{
		gradeArray.clear();
	}

	const JSON json = JSON::Load(path);
	if (!json || !json.hasElement(U"problems"))
	{
		return false;
	}

	for (const auto& pj : json[U"problems"].arrayView())
	{
		Problem p;
		p.id = pj[U"id"].getOr<String>(U"");
		p.author = pj[U"author"].getOr<String>(U"");
		p.source = pj[U"source"].getOr<String>(U"");
		p.text = pj[U"text"].getOr<String>(U"");
		p.hasKigo = pj[U"hasKigo"].getOr<bool>(false);
		p.kigo = pj[U"kigo"].getOr<String>(U"");
		p.kigoStart = pj[U"kigoStart"].getOr<int32>(-1);
		p.kigoEnd = pj[U"kigoEnd"].getOr<int32>(-1);
		p.explanation = pj[U"explanation"].getOr<String>(U"");
		p.grade = pj[U"grade"].getOr<int32>(
			ProblemGrade::Trainee);					  // デフォルトは特待生
		p.ruby = pj[U"ruby"].getOr<String>(U"");	  // フリガナ情報
		p.rhythm = pj[U"rhythm"].getOr<String>(U"");  // リズム情報
		p.displayRuby = pj[U"displayRuby"].getOr<bool>(false);	// リズム情報

		// タグ情報の読み込み
		if (const auto tags = pj[U"tags"]; tags && tags.isArray())
		{
			for (const auto& t : tags.arrayView())
			{
				p.tags << t.getString();
			}
		}

		if (!p.isValid())
		{
			Console << U"[ProblemManager] invalid problem: {}"_fmt(p.id);
			continue;  // 不正なものはスキップ
		}
		p.index = static_cast<int32>(m_problems.size());
		m_problems << p;  // 全問題配列に追加
	}

	// 問題を段位別に振り分け
	filterProblemsByGrade();

	return (!m_problems.isEmpty());
}

void ProblemManager::filterProblemsByGrade()
{
	// 各段位の配列をクリア
	for (auto& gradeArray : m_gradeProblems)
	{
		gradeArray.clear();
	}

	// 問題を段位別に振り分け
	for (const auto& problem : m_problems)
	{
		if (problem.grade >= 0 && problem.grade < ProblemGrade::Count)
		{
			m_gradeProblems[problem.grade] << problem;
		}
	}
}

const Array<Problem>& ProblemManager::getProblemsForGrade(int32 grade) const
{
	return m_gradeProblems[grade];
}

void ProblemManager::shuffleProblemsForGrade(int32 grade)
{
	if (grade >= 0 && grade < ProblemGrade::Count)
	{
		m_gradeProblems[grade].shuffle();
	}
}

size_t ProblemManager::getCompletedCountForGrade(int32 grade) const
{
	if (grade >= 0 && grade < ProblemGrade::Count)
	{
		return m_gradeProblems[grade].count_if([](const Problem& p)
											   { return p.completed; });
	}
	return 0;
}

bool ProblemManager::isGradeAvailable(int32 grade, size_t playerRank) const
{
	// 無効な段位は使用不可
	if (grade < 0 || grade >= ProblemGrade::Count)
	{
		return false;
	}

	// プレイヤーの現在の段位より下の段位は常にプレイ可能
	if (grade <= static_cast<int32>(playerRank))
	{
		return true;
	}

	// それ以外の段位は使用不可
	return false;
}

void ProblemManager::setSelectedGrade(int32 grade)
{
	// 無効な段位は選択できない
	if (grade != GameConstants::DEFAULT_SELECTED_GRADE &&
		(grade < 0 || grade >= ProblemGrade::Count))
	{
		return;
	}

	m_selectedGrade = grade;
}
