#pragma once

struct UIConfig
{
	double maxLineWidth = 920.0;
	double lineHeightScale = 1.25;
	double lineWidthScale = 1.25;
	double hitboxPaddingPx = 6.0;
	double hitboxPaddingScale = 0.1;
	int32_t wordWidth = 80;
	int32_t clientSizeX = 1280;
	int32_t clientSizeY = 720;
};

struct AudioConfig
{
	double bgmVolume = 0.6;	 // 0.0 - 1.0
	double seVolume = 0.8;	 // 0.0 - 1.0
};

struct SaveData
{
	size_t rankIndex = 0;				   // 現在の段位
	Array<size_t> gradeProgress{0, 0, 0};  // 各段位問題での進捗
	Array<bool> problemStatus;			   // 問題ごとのクリア状況
	int32 totalScore = 0;				   // 累計スコア

	// JSONシリアライズ用
	void serialize(JSON& json) const;
	void deserialize(const JSON& json);
};
