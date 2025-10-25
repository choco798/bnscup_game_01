#include "ResultScene.hpp"
#include "stdafx.h"

ResultScene::ResultScene(const InitData& data) : IScene{data}
{
	Size rect_size(260, 56);
	m_backBtn = ui::Button(U"タイトルへ", U"Score",
						   Vec2{780 + rect_size.x / 2, 470 + rect_size.y / 2});
}

void ResultScene::update()
{
	m_backBtn.update();
	if (m_backBtn.isClicked())
	{
		changeScene(State::Title);
	}

}

void ResultScene::draw() const
{
	getData().renderer.drawBackground();

	FontAsset(U"Result")(U"結果発表").drawAt(Scene::Center().x, 140, Palette::Black);

	RoundRect panel{RectF{200, 220, 880, 320}, 20};
	panel.draw(ColorF{1.0, 1.0, 1.0, 0.92}).drawFrame(3, 0, Palette::Black);

	FontAsset(U"Score")(U"正解数: {}"_fmt(getData().gameState.correctCount))
		.draw(240, 260, Palette::Black);
	FontAsset(U"Score")(U"スコア: {}"_fmt(getData().gameState.score))
		.draw(240, 320, Palette::Black);

	m_backBtn.draw();
}
