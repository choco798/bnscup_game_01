#include "stdafx.h"

#include "TitleScene.hpp"

TitleScene::TitleScene(SoundManager& sound, String rankName,
					   const Config& config)
	:
	m_sound{sound}, m_rankName(rankName)
{
	m_sound.playBGM();

	m_sound.setBgmVolume(config.audio().bgmVolume);
	m_sound.setSeVolume(config.audio().seVolume);
}

void TitleScene::update(bool enable)
{
	// 入力は簡易スライダーのみ処理。遷移は Main 側で制御。
	// シーン遷移は外側のコントローラで行う想定。
	if (!enable)
	{
		return;
	}

	const RectF bgmBar{820, 620, 360, 10};
	const RectF seBar{820, 660, 360, 10};
	const Circle bgmKnob{bgmBar.x + bgmBar.w * m_sound.getBgmVolume(),
						 bgmBar.y + bgmBar.h * 0.5, 10};
	const Circle seKnob{seBar.x + seBar.w * m_sound.getSeVolume(),
						seBar.y + seBar.h * 0.5, 10};
	// ドラッグ開始
	if (bgmKnob.leftClicked())
	{
		m_dragBgm = true;
	}
	if (seKnob.leftClicked())
	{
		m_dragSe = true;
	}
	// ドラッグ終了
	if (MouseL.up())
	{
		m_dragBgm = false;
		m_dragSe = false;
	}
	// ドラッグ中はつまみ位置から音量を更新
	if (m_dragBgm)
	{
		const double t =
			Clamp((Cursor::PosF().x - bgmBar.x) / bgmBar.w, 0.0, 1.0);
		m_sound.setBgmVolume(t);
	}
	if (m_dragSe)
	{
		const double t =
			Clamp((Cursor::PosF().x - seBar.x) / seBar.w, 0.0, 1.0);
		m_sound.setSeVolume(t);
	}
}

void TitleScene::draw() const
{
	Rect{Scene::Size()}.draw(ColorF{0.96});

	FontAsset(U"Title")(U"ああ季語や")
		.drawAt(Scene::Center().x, 160, Palette::Black);

	RoundRect{Arg::center = Vec2{Scene::Center().x, 360}, 360, 72, 16}
		.draw(Palette::White)
		.drawFrame(4, 0, Palette::Black);
	FontAsset(U"Title")(U"ゲーム開始").drawAt(Scene::Center().x, 360, Palette::Black);

	RoundRect{Arg::center = Vec2{Scene::Center().x, 460}, 360, 72, 16}
		.draw(Palette::White)
		.drawFrame(4, 0, Palette::Black);
	FontAsset(U"TitleText")(U"遊び方").drawAt(Scene::Center().x, 460, Palette::Black);

	RoundRect{Arg::center = Vec2{Scene::Center().x, 560}, 360, 72, 16}
		.draw(Palette::White)
		.drawFrame(4, 0, Palette::Black);
	FontAsset(U"TitleText")(U"終了").drawAt(Scene::Center().x, 560, Palette::Black);

	// 追加: 現在の段位表示（左中パネル）

	{
		const RectF panel{50, 340, 300, 100};
		panel.rounded(16).draw(ColorF{1.0, 1.0, 1.0, 0.92});
		panel.drawFrame(3, 0, Palette::Black);
		FontAsset(U"TitleText")(U"現在の段位")
			.draw(panel.x + 16, panel.y + 10, Palette::Darkslategray);
		FontAsset(U"TitleText")(m_rankName)
			.draw(panel.x + 16, panel.y + 46, Palette::Black);
	}

	// 追加: BGM / SE 音量スライダー（下部）

	{
		const RectF bgmBar{820, 620, 360, 10};
		const RectF seBar{820, 660, 360, 10};
		FontAsset(U"TitleText")(U"BGM 音量")
			.draw(bgmBar.x, bgmBar.y, Palette::Black);
		FontAsset(U"TitleText")(U"SE 音量")
			.draw(seBar.x, seBar.y, Palette::Black);
		// バー
		bgmBar.draw(ColorF{m_dragBgm ? 0.4 : 0.85});
		seBar.draw(ColorF{m_dragSe ? 0.4 : 0.85});
		// ノブ
		const Circle bgmKnob{bgmBar.x + bgmBar.w * m_sound.getBgmVolume(),
							 bgmBar.y + bgmBar.h * 0.5, 10};
		const Circle seKnob{seBar.x + seBar.w * m_sound.getSeVolume(),
							seBar.y + seBar.h * 0.5, 10};
		bgmKnob.draw(Palette::White).drawFrame(2, 0, Palette::Black);
		seKnob.draw(Palette::White).drawFrame(2, 0, Palette::Black);
	}
}
