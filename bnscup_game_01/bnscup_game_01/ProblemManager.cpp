#include "ProblemManager.hpp"

bool ProblemManager::loadFromJSON(const FilePath& path)
{
	m_problems.clear();

	const JSON json = JSON::Load(path);
	if (!json)
	{
		return false;
	}

	if (!json.hasElement(U"problems"))
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
		m_problems << std::move(p);
	}
	return (!m_problems.isEmpty());
}

const Array<Problem>& ProblemManager::getProblems() const noexcept
{
	return m_problems;
}

const Problem& ProblemManager::getProblem(size_t index) const
{
	return m_problems.at(index);
}

size_t ProblemManager::size() const noexcept
{
	return m_problems.size();
}
