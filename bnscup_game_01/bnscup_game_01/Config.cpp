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

void RhythmConfig::serialize(JSON& json) const
{
	json[U"bpm"] = bpm;
	json[U"micSensitivity"] = micSensitivity;
	json[U"rhythmModeEnabled"] = rhythmModeEnabled;

	// VADパラメータ
	json[U"vadAlpha"] = vadAlpha;
	json[U"vadKOn"] = vadKOn;
	json[U"vadKOff"] = vadKOff;
	json[U"vadAbsOn"] = vadAbsOn;
	json[U"vadAbsOff"] = vadAbsOff;
	json[U"vadBandLowHz"] = vadBandLowHz;
	json[U"vadBandHighHz"] = vadBandHighHz;
	json[U"vadMinOnMs"] = static_cast<int64>(vadMinOnMs);
	json[U"vadMinOffMs"] = static_cast<int64>(vadMinOffMs);
}

void RhythmConfig::deserialize(const JSON& json)
{
	bpm = json[U"bpm"].getOr<double>(120.0);
	micSensitivity = json[U"micSensitivity"].getOr<double>(0.5);
	rhythmModeEnabled = json[U"rhythmModeEnabled"].getOr<bool>(true);

	// VADパラメータ（デフォルト値はGameConstants::Rhythmから）
	vadAlpha =
		json[U"vadAlpha"].getOr<double>(GameConstants::Rhythm::VAD_ALPHA);
	vadKOn = json[U"vadKOn"].getOr<double>(GameConstants::Rhythm::VAD_K_ON);
	vadKOff = json[U"vadKOff"].getOr<double>(GameConstants::Rhythm::VAD_K_OFF);
	vadAbsOn =
		json[U"vadAbsOn"].getOr<double>(GameConstants::Rhythm::VAD_ABS_ON);
	vadAbsOff =
		json[U"vadAbsOff"].getOr<double>(GameConstants::Rhythm::VAD_ABS_OFF);
	vadBandLowHz = json[U"vadBandLowHz"].getOr<double>(
		GameConstants::Rhythm::VAD_BAND_LOW_HZ);
	vadBandHighHz = json[U"vadBandHighHz"].getOr<double>(
		GameConstants::Rhythm::VAD_BAND_HIGH_HZ);
	vadMinOnMs = static_cast<int32>(
		json[U"vadMinOnMs"].getOr<int64>(GameConstants::Rhythm::VAD_MIN_ON_MS));
	vadMinOffMs = static_cast<int32>(json[U"vadMinOffMs"].getOr<int64>(
		GameConstants::Rhythm::VAD_MIN_OFF_MS));
}
