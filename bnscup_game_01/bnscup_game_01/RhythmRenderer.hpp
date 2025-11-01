#pragma once

#include <Siv3D.hpp>
#include "GameConstants.hpp"
#include "MoraSystem.hpp"


// ---- 描画レイアウト設定 ----
struct RhythmLayout
{
	double laneY = GameConstants::Rhythm::LANE_Y;  // ベース線Y
	double pxPerBeat =
		GameConstants::Rhythm::PX_PER_BEAT;		 // 1拍の横幅（表示スケール）
	double hitX = GameConstants::Rhythm::HIT_X;	 // "いま"の基準線X
	double moraRadius = GameConstants::Rhythm::MORA_RADIUS;	 // モーラ玉の半径
	double beamY =
		GameConstants::Rhythm::BEAM_Y;	// ビーム（連結線）の縦位置オフセット
};

// ---- 描画ヘルパー関数 ----
namespace RhythmDrawUtils
{
// Cut線描画
void DrawCut(double x, const RhythmLayout& layout, bool strong);

// モーラ間のビーム（語の滑らかさ）
void DrawBeam(double x1, double x2, const RhythmLayout& layout);

// フェルマータ（弧＋点で近似）
void DrawFermata(double x, const RhythmLayout& layout);
}  // namespace RhythmDrawUtils

// ---- モーラレンダラー ----
class MoraRenderer
{
   public:
	MoraRenderer();

	bool initialize();	// フォント初期化（例外なし）
	void finalize();	// クリーンアップ

	void drawGuide(const RhythmLayout& layout, double scrollBeat) const;
	void drawCuts(const ParsedStream& parsedStream, const RhythmLayout& layout,
				  double scrollBeat) const;
	void drawMoras(const ParsedStream& parsedStream, const RhythmLayout& layout,
				   double scrollBeat, double voiceLevel) const;

   private:
	void drawMoraGlyph(const Mora& mora, const RhythmLayout& layout, double x,
					   const ColorF& glow) const;

	bool m_initialized = false;
};

// ---- 声量リアクティブエフェクト ----
struct VoiceState
{
	bool active = false;
	double energy = 0.0;
	double noise = 0.0;
	double onTh = 0.0;
	double offTh = 0.0;
};

class VoiceReactiveFx
{
   public:
	void update(const VoiceState& state, double deltaTime);
	double level() const;

   private:
	double m_level{0.0};
};
