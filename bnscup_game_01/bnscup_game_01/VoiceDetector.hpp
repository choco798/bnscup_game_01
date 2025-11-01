#pragma once

#include <Siv3D.hpp>
#include "GameConstants.hpp"

// ---- Voice Activity Detection パラメータ ----
struct VoiceActivityDetectorParams
{
	// ノイズ推定
	double alpha =
		GameConstants::Rhythm::VAD_ALPHA;  // ノイズEMAの追従度 (0..1)

	// ヒステリシス閾値（noiseEMA に対する倍率）＋ 絶対足しこみ
	double kOn = GameConstants::Rhythm::VAD_K_ON;
	double kOff = GameConstants::Rhythm::VAD_K_OFF;
	double absOn = GameConstants::Rhythm::VAD_ABS_ON;
	double absOff = GameConstants::Rhythm::VAD_ABS_OFF;

	// 人声帯域 [Hz]
	double bandLowHz = GameConstants::Rhythm::VAD_BAND_LOW_HZ;
	double bandHighHz = GameConstants::Rhythm::VAD_BAND_HIGH_HZ;

	// 最小継続時間 [ms]
	int32 minOnMs = GameConstants::Rhythm::
		VAD_MIN_ON_MS;	// 「声あり」確定までに必要な最小継続
	int32 minOffMs = GameConstants::Rhythm::
		VAD_MIN_OFF_MS;	 // 「無音」確定までに必要な最小継続
};

// ---- デバッグログ設定 ----
struct VoiceActivityDetectorDebugLogSettings
{
	bool enabled = false;				// true で記録
	FilePath logPath = U"vad_log.csv";	// 出力先
	bool append = false;				// 追記（falseで新規作成）
	bool writeHeader = true;			// ヘッダ行の出力
	int32 flushEveryN = 30;				// Nフレームごとに flush
};

// ---- Voice State ----
struct VoiceState
{
	bool active = false;
	double energy = 0.0;
	double noise = 0.0;
	double onTh = 0.0;
	double offTh = 0.0;
};

// ---- Voice Activity Detector インターフェース ----
class IVoiceActivityDetector
{
   public:
	virtual ~IVoiceActivityDetector() = default;
	virtual bool initialize(
		const Optional<uint32>& deviceIndex = unspecified) = 0;	 // 初期化
	virtual void finalize() = 0;
	virtual bool update() = 0;			   // 状態変化が起きたら true
	virtual VoiceState state() const = 0;  // 現在値
	virtual void setParams(
		const VoiceActivityDetectorParams& params) = 0;	 // パラメータ設定
};

// ---- Voice Activity Detector 実装 ----
class VoiceActivityDetector : public IVoiceActivityDetector
{
   public:
	using Params = VoiceActivityDetectorParams;
	using DebugLog = VoiceActivityDetectorDebugLogSettings;

	VoiceActivityDetector(const Params& params = Params{},
						  const DebugLog& dbg = DebugLog{});
	~VoiceActivityDetector();

	// IVoiceActivityDetector実装
	bool initialize(const Optional<uint32>& deviceIndex = unspecified) override;
	void finalize() override;
	bool update() override;	 // 1フレーム更新。戻り値は「確定状態が変化したか」
	VoiceState state() const override;
	void setParams(const VoiceActivityDetectorParams& params) override;

	// デバッグ用
	void drawUI() const;
	bool isVoice() const;
	double energy() const;
	double noiseEMA() const;
	double onThreshold() const;
	double offThreshold() const;
	const Params& params() const;

   private:
	void writeLog(bool rawVoice, double onTh, double offTh, int32 i0, int32 i1,
				  double hzPerBin);
	bool initializeLog();

   private:
	Params m_params;
	DebugLog m_dbgOpt;

	Microphone m_mic;
	FFTResult m_fft;

	double m_noiseEMA = 0.0;
	double m_lastEnergy = 0.0;

	TextWriter m_writer;
	int32 m_logFrameCount = 0;

	bool m_rawVoicePrev = false;	// 生状態（最小継続前の"候補"）
	bool m_confirmedVoice = false;	// 最小継続を満たした確定状態
	Stopwatch m_stageTimer{StartImmediately::No};

	bool m_initialized = false;
};

// ---- ダミー実装（マイクが使用できない場合用） ----
class DummyVoiceActivityDetector : public IVoiceActivityDetector
{
   public:
	bool initialize(const Optional<uint32>& deviceIndex = unspecified) override;
	void finalize() override;
	bool update() override;
	VoiceState state() const override;
	void setParams(const VoiceActivityDetectorParams& params) override;

   private:
	VoiceState m_state;
	VoiceActivityDetectorParams m_params;
};