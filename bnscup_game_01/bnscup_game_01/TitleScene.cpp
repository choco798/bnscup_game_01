#include "stdafx.h"

#include "TitleScene.hpp"

TitleScene::TitleScene(SoundManager& sound) : m_sound{sound}
{
	m_sound.playBGM();

	// 最低限のプレースホルダ
	normal_font = Font{64, Typeface::Bold};
	small_font = Font{32};
}

void TitleScene::update()
{
	// ここでは UI のヒットテストは行わず、描画専任。
	// シーン遷移は外側のコントローラで行う想定。
}

void TitleScene::draw() const
{

	Rect{Scene::Size()}.draw(ColorF{0.96});

	normal_font(U"ああ季語や")
		.drawAt(Scene::Center().x, 160, Palette::Black);

	RoundRect{Arg::center = Vec2{Scene::Center().x, 360}, 360, 72, 16}
		.draw(Palette::White)
		.drawFrame(4, 0, Palette::Black);
	normal_font(U"ゲーム開始").drawAt(Scene::Center().x, 360, Palette::Black);

	RoundRect{Arg::center = Vec2{Scene::Center().x, 460}, 360, 72, 16}
		.draw(Palette::White)
		.drawFrame(4, 0, Palette::Black);
	small_font(U"遊び方").drawAt(Scene::Center().x, 460, Palette::Black);

	RoundRect{Arg::center = Vec2{Scene::Center().x, 560}, 360, 72, 16}
		.draw(Palette::White)
		.drawFrame(4, 0, Palette::Black);
	small_font(U"終了").drawAt(Scene::Center().x, 560, Palette::Black);
}
