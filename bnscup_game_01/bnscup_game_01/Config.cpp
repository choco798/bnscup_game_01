#include "Config.hpp"
#include "GameConstants.hpp"

void SaveData::serialize(JSON& json) const
{
	json[U"rankIndex"] = static_cast<int64>(rankIndex);
	json[U"totalScore"] = static_cast<int64>(totalScore);

	// 配列をJSONArrayに変換
	Array<JSON> progressArray;
	for (size_t progress : gradeProgress)
	{
		progressArray.push_back(JSON(static_cast<int64>(progress)));
	}
	json[U"gradeProgress"] = JSON(progressArray);

	Array<JSON> statusArray;
	for (bool status : problemStatus)
	{
		statusArray.push_back(JSON(status));
	}
	json[U"problemStatus"] = JSON(statusArray);
}

void SaveData::deserialize(const JSON& json)
{
	rankIndex = static_cast<size_t>(json[U"rankIndex"].getOr<int64>(0));
	totalScore = json[U"totalScore"].getOr<int32>(0);

	// gradeProgress配列の手動読み取り
	gradeProgress.clear();
	if (json.hasElement(U"gradeProgress") && json[U"gradeProgress"].isArray())
	{
		const JSON& progressArray = json[U"gradeProgress"];
		gradeProgress.reserve(progressArray.size());

		for (size_t i = 0; i < progressArray.size(); ++i)
		{
			auto value = progressArray[i].getOr<int64>(0);
			gradeProgress.push_back(static_cast<size_t>(value));
		}
	}

	// デフォルト値で初期化（段位数分）
	if (gradeProgress.size() < GameConstants::RankNames::RANK_COUNT)
	{
		gradeProgress.resize(GameConstants::RankNames::RANK_COUNT, 0);
	}

	// problemStatus配列の手動読み取り
	problemStatus.clear();
	if (json.hasElement(U"problemStatus") && json[U"problemStatus"].isArray())
	{
		const JSON& statusArray = json[U"problemStatus"];
		problemStatus.reserve(statusArray.size());

		for (size_t i = 0; i < statusArray.size(); ++i)
		{
			bool status = statusArray[i].getOr<bool>(false);
			problemStatus.push_back(status);
		}
	}
}
