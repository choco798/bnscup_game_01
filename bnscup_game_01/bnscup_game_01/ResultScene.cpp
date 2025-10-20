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

	FontAsset(U"Result")(U"結果発表").drawAt(Scene::Center().x, 140, Palette::Black);

	RoundRect panel{RectF{200, 220, 880, 320}, 20};
	panel.draw(ColorF{1.0, 1.0, 1.0, 0.92}).drawFrame(3, 0, Palette::Black);

	FontAsset(U"Score")(U"正解数: {}"_fmt(m_state.correctCount))
		.draw(240, 260, Palette::Black);
	FontAsset(U"Score")(U"スコア: {}"_fmt(m_state.score))
		.draw(240, 320, Palette::Black);

	Size rect_size(260, 56);

	RoundRect{RectF{780, 470, rect_size.x, rect_size.y}, 16}
		.draw(Palette::White)
		.drawFrame(3, 0, Palette::Black);
	FontAsset(U"Score")(U"タイトルへ")
		.drawAt(780 + rect_size.x / 2, 470 + rect_size.y / 2, Palette::Black);
}
