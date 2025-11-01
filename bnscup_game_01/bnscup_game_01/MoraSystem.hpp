#pragma once

#include <Siv3D.hpp>
#include "GameConstants.hpp"

// ---- ユーティリティ関数 ----
namespace MoraUtils
{
double Frac(double x);
bool IsYouonSmall(char32 ch);
}  // namespace MoraUtils

// ---- モーラ要素 ----
struct Mora
{
	String text;			// 表示（1モーラ）
	double startBeat{};		// このモーラの開始拍（スクロール計算用）
	bool beamNext = false;	// 次モーラと連結ビーム
	bool fermata = false;	// このモーラの直後に無音休止（余韻）
};

// 句切れ（どのモーラの"直後"か）
struct Cut
{
	size_t afterIndex{};  // mora配列の何番目の直後か（0-based）
	int32 beats{};		  // 無音休止拍数
	bool strong = false;  // 表示スタイル用
};

// ---- 休止の設定 ----
struct RestPreset
{
	int32 lightCutBeats = GameConstants::Rhythm::LIGHT_CUT_BEATS;	 // |
	int32 strongCutBeats = GameConstants::Rhythm::STRONG_CUT_BEATS;	 // ||
	int32 fermataBeats =
		GameConstants::Rhythm::FERMATA_BEATS;  // ~（無音は音後）
	int32 tailEndBeats = GameConstants::Rhythm::TAIL_END_BEATS;	 // 末尾の余韻
};

// ---- パース結果 ----
struct ParsedStream
{
	Array<Mora> moras;
	Array<Cut> cuts;
	double totalBeats = 0.0;  // 最終積算拍（休止含む）
};

// ---- カナストリームパーサー ----
class KanaStreamParser
{
   public:
	static ParsedStream Parse(const String& stream, const RestPreset& rest);

   private:
	static bool IsYouonSmall(char32 ch);
};

// ---- ビートトランスポート ----
class BeatTransport
{
   public:
	explicit BeatTransport(double bpm);

	void start();
	void pause();
	void resume();
	void reset();
	bool isRunning() const;

	double nowBeat() const;
	void setPreRoll(double sec);

	double secPerBeat() const;
	double bpm() const;

   private:
	double m_bpm{};
	double m_preRollSec{1.0};  // 既定で -1秒の余裕を設けるなら 1.0
	Stopwatch m_sw{StartImmediately::No};
};

// ---- ビートヒット検出 ----
class BeatHitDetector
{
   public:
	void reset();

	// スクロール更新のたびに呼ぶ。渡したコールバックでSEや任意処理を発火
	template <class Fn>
	void process(const ParsedStream& parsedStream, double nowBeat, Fn onHit);

   private:
	double m_prev{0.0};
	size_t m_nextIndex{0};
};

// ---- テンプレート実装 ----
template <class Fn>
void BeatHitDetector::process(const ParsedStream& parsedStream, double nowBeat,
							  Fn onHit)
{
	constexpr double eps = 1e-2;
	while (m_nextIndex < parsedStream.moras.size() &&
		   parsedStream.moras[m_nextIndex].startBeat <= nowBeat + eps)
	{
		if (parsedStream.moras[m_nextIndex].startBeat > m_prev + eps)
		{
			onHit(m_nextIndex);	 // ここで se.playOneShot() 等
		}
		++m_nextIndex;
	}
	m_prev = nowBeat;
}
