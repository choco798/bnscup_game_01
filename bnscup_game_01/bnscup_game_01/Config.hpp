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

struct RhythmConfig
{
	double bpm = 120.0;				// BPM設定
	double micSensitivity = 0.5;	// マイク感度（0.0-1.0）
	bool rhythmModeEnabled = true;	// リズムモード有効フラグ

	// VAD（Voice Activity Detection）パラメータ
	double vadAlpha = 0.02;			// ノイズEMAの追従度
	double vadKOn = 1.0;			// ON閾値倍率
	double vadKOff = 0.4;			// OFF閾値倍率
	double vadAbsOn = 0.005;		// ON閾値絶対値
	double vadAbsOff = 0.01;		// OFF閾値絶対値
	double vadBandLowHz = 80.0;		// 人声帯域下限[Hz]
	double vadBandHighHz = 6000.0;	// 人声帯域上限[Hz]
	int32 vadMinOnMs = 80;			// 声あり最小継続時間[ms]
	int32 vadMinOffMs = 200;		// 無音最小継続時間[ms]

	// JSONシリアライズ用
	void serialize(JSON& json) const;
	void deserialize(const JSON& json);
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
