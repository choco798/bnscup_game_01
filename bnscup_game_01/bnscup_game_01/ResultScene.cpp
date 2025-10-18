#include "ResultScene.hpp"
#include "stdafx.h"

ResultScene::ResultScene(const GameState& state, Renderer& renderer,
						 SoundManager& sound)
	: m_state{state}, m_renderer{renderer}, m_sound{sound}
{
}

void ResultScene::update()
{
	// 外側の遷移コントローラでボタン判定を行う想定
}

void ResultScene::draw() const
{
	m_renderer.drawBackground();

	const Font f{56, Typeface::Bold};
	const Font s{32};

	f(U"結果発表").drawAt(Scene::Center().x, 140, Palette::Black);

	RoundRect panel{RectF{200, 220, 880, 320}, 20};
	panel.draw(ColorF{1.0, 1.0, 1.0, 0.92}).drawFrame(3, 0, Palette::Black);

	s(U"正解数: {}"_fmt(m_state.correctCount)).draw(240, 260, Palette::Black);
	s(U"スコア: {}"_fmt(m_state.score)).draw(240, 320, Palette::Black);
	s(U"（スコアは実行中のみ保持・永続化なし）")
		.draw(240, 380, Palette::Dimgray);

	RoundRect{RectF{780, 470, 260, 56}, 16}
		.draw(Palette::White)
		.drawFrame(3, 0, Palette::Black);

	s(U"タイトルへ")
		.draw(780 + 130, 470 + 28, Palette::Black);
}
