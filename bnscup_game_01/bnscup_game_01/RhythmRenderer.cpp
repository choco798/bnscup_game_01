#include "RhythmRenderer.hpp"
#include "MoraSystem.hpp"

// ---- 描画ヘルパー関数実装 ----
namespace RhythmDrawUtils
{
void DrawCut(double x, const RhythmLayout& layout, bool strong)
{
	const double len = strong ? 90.0 : 70.0;
	const double thick = strong ? 6.0 : 3.0;
	Line{x, layout.laneY - len * 0.5, x, layout.laneY + len * 0.5}.draw(
		thick, Color(0, 0, 0, 160));
}

void DrawBeam(double x1, double x2, const RhythmLayout& layout)
{
	if (x2 <= x1) return;
	const double y = layout.laneY + layout.beamY;
	const double h = 4.0;
	RectF{x1 + 6, y - h * 0.5, (x2 - x1) - 12, h}.draw(ColorF{0, 0, 0, 0.75});
}

void DrawFermata(double x, const RhythmLayout& layout)
{
	const double r = layout.moraRadius + 8;
	const Circle c{x, layout.laneY - (layout.moraRadius + 12), r};
	c.drawArc(-90.0_deg, 180.0_deg, 2.5, 0.0, Palette::Black);
	Circle{x, c.y, 4.5}.draw(Palette::Black);
}
}  // namespace RhythmDrawUtils

// ---- MoraRenderer実装 ----
MoraRenderer::MoraRenderer()
{
}

bool MoraRenderer::initialize()
{
	if (m_initialized)
	{
		return true;
	}

	// GameConstantsの定数を使ってフォント登録
	using namespace GameConstants::Rhythm;

	FontAsset::Register(KEY_KANA_BIG, FontMethod::MSDF, SIZE_KANA_BIG,
						Typeface::Regular);
	FontAsset::Register(KEY_KANA_SMALL, FontMethod::MSDF, SIZE_KANA_SMALL,
						Typeface::Regular);
	FontAsset::Register(KEY_UI_SMALL, FontMethod::MSDF, SIZE_UI_SMALL,
						Typeface::Regular);

	// フォント読み込み待機
	FontAsset::Wait(KEY_KANA_BIG);
	FontAsset::Wait(KEY_KANA_SMALL);
	FontAsset::Wait(KEY_UI_SMALL);

	m_initialized = true;
	return true;
}

void MoraRenderer::finalize()
{
	using namespace GameConstants::Rhythm;

	FontAsset::Unregister(KEY_KANA_BIG);
	FontAsset::Unregister(KEY_KANA_SMALL);
	FontAsset::Unregister(KEY_UI_SMALL);

	m_initialized = false;
}

void MoraRenderer::drawGuide(const RhythmLayout& layout,
							 double scrollBeat) const
{
	if (!m_initialized) return;

	// いまのライン
	Line{layout.hitX, 40, layout.hitX, Scene::Height() - 40}.draw(
		3, Palette::Orange);

	// ベースライン
	Line{0, layout.laneY, Scene::Width(), layout.laneY}.draw(2, Palette::Black);

	// 拍グリッド
	for (int i = -8; i <= 24; ++i)
	{
		const double gx =
			layout.hitX + (i - MoraUtils::Frac(scrollBeat)) * layout.pxPerBeat;
		const Color c =
			(i == 0 ? Palette::Red : Color(0, 0, 0, (i % 4 == 0 ? 64 : 28)));
		Line{gx, layout.laneY - 60, gx, layout.laneY + 60}.draw(1, c);
	}
}

void MoraRenderer::drawCuts(const ParsedStream& parsedStream,
							const RhythmLayout& layout, double scrollBeat) const
{
	if (!m_initialized) return;

	for (const auto& c : parsedStream.cuts)
	{
		const double cutBeat = parsedStream.moras[c.afterIndex].startBeat + 1.0;
		const double x =
			layout.hitX + (cutBeat - scrollBeat) * layout.pxPerBeat;
		if (-20 <= x && x <= Scene::Width() + 20)
		{
			RhythmDrawUtils::DrawCut(x, layout, c.strong);
		}
	}
}

void MoraRenderer::drawMoras(const ParsedStream& parsedStream,
							 const RhythmLayout& layout, double scrollBeat,
							 double voiceLevel) const
{
	if (!m_initialized) return;

	const ColorF glowColor = GameConstants::Rhythm::GLOW_COLOR_BASE *
							 ColorF{1, 1, 1, voiceLevel * 10.0};

	for (size_t i = 0; i < parsedStream.moras.size(); ++i)
	{
		const auto& m = parsedStream.moras[i];
		const double x =
			layout.hitX + (m.startBeat - scrollBeat) * layout.pxPerBeat;
		if (x < -120 || x > Scene::Width() + 120) continue;

		// ビーム（次モーラ連結）
		if (m.beamNext && (i + 1 < parsedStream.moras.size()))
		{
			const double nx =
				layout.hitX +
				(parsedStream.moras[i + 1].startBeat - scrollBeat) *
					layout.pxPerBeat;
			if (nx > x + 24) RhythmDrawUtils::DrawBeam(x, nx, layout);
		}

		drawMoraGlyph(m, layout, x, glowColor);
	}
}

void MoraRenderer::drawMoraGlyph(const Mora& mora, const RhythmLayout& layout,
								 double x, const ColorF& glow) const
{
	using namespace GameConstants::Rhythm;

	// 声量エフェクト：背景グロー
	if (glow.a > 0.01)
	{
		Circle{x, layout.laneY, layout.moraRadius * (1.0 + 0.8 * glow.a)}
			.drawShadow(Vec2{0, 0}, 8, 4, glow);
	}

	// モーラ玉
	Circle{x, layout.laneY, layout.moraRadius}.draw(Palette::Black);

	// 読み仮名（拗音など2文字以上は小フォント）
	const Font& f = (mora.text.size() >= 2 ? FontAsset(KEY_KANA_SMALL)
										   : FontAsset(KEY_KANA_BIG));
	f(mora.text).drawAt(Vec2{x, layout.laneY + 26}, Palette::Black);

	// フェルマータ
	if (mora.fermata) RhythmDrawUtils::DrawFermata(x, layout);
}

// ---- VoiceReactiveFx実装 ----
void VoiceReactiveFx::update(const VoiceState& state, double deltaTime)
{
	(void)deltaTime;
	// 簡易：エネルギー正規化 → 平滑 → 0..1
	double target =
		Saturate((state.energy - state.noise + (state.active ? 0.5 : 0)) /
				 (state.noise * 4.0 + 0.1 + 0.5));
	m_level = Math::Lerp(m_level, target, 0.15);  // スムージング
}

double VoiceReactiveFx::level() const
{
	return m_level;
}
