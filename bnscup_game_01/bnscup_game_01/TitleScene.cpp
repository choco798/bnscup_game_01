#include "TitleScene.hpp"

TitleScene::TitleScene(const InitData& init) : IScene{init}
{
	getData().sound.playBGM();

	getData().sound.setBgmVolume(getData().configManager.audio().bgmVolume);
	getData().sound.setSeVolume(getData().configManager.audio().seVolume);

	m_startBtn =
		ui::Button(U"ゲーム開始", U"Title", Vec2{Scene::Center().x, 360});
	m_howToBtn =
		ui::Button(U"遊び方", U"TitleText", Vec2{Scene::Center().x, 480});
	m_exitBtn = ui::Button(U"終了", U"TitleText", Vec2{Scene::Center().x, 560});
}

void TitleScene::update()
{
	// 入力は簡易スライダーのみ処理。遷移は Main 側で制御。
	// シーン遷移は外側のコントローラで行う想定。

	// ボタン
	m_startBtn.update();
	m_howToBtn.update();
	m_exitBtn.update();

	const bool disableInput = getData().showHowToPlay;
	if (disableInput)
	{
		return;
	}

	const RectF bgmBar{820, 620, 360, 10};
	const RectF seBar{820, 660, 360, 10};
	const Circle bgmKnob{bgmBar.x + bgmBar.w * getData().sound.getBgmVolume(),
						 bgmBar.y + bgmBar.h * 0.5, 10};
	const Circle seKnob{seBar.x + seBar.w * getData().sound.getSeVolume(),
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
		getData().sound.setBgmVolume(t);
	}
	if (m_dragSe)
	{
		const double t =
			Clamp((Cursor::PosF().x - seBar.x) / seBar.w, 0.0, 1.0);
		getData().sound.setSeVolume(t);
	}

	if (m_startBtn.isClicked())
	{
		// ゲーム開始
		getData().gameState.state_reset();
		if (getData().gameState.problems.isEmpty())
		{
			System::MessageBoxOK(U"出題できる問題がありません。");
		}
		else
		{
			changeScene(State::Game);
		}
	}
	if (m_howToBtn.isClicked())
	{
		getData().showHowToPlay = true;
	}
	if (m_exitBtn.isClicked())
	{
		System::Exit();
	}
}

void TitleScene::draw() const
{
	Rect{Scene::Size()}.draw(ColorF{0.96});

	FontAsset(U"Title")(U"季語シンクロ！")
		.drawAt(Scene::Center().x, 160, Palette::Black);

	m_startBtn.draw();
	m_howToBtn.draw();
	m_exitBtn.draw();

	// 追加: 現在の段位表示（左中パネル）
	{
		const RectF panel{50, 340, 300, 100};
		panel.rounded(16).draw(ColorF{1.0, 1.0, 1.0, 0.92});
		panel.drawFrame(3, 0, Palette::Black);
		FontAsset(U"TitleText")(U"現在の段位")
			.draw(panel.x + 16, panel.y + 10, Palette::Darkslategray);
		FontAsset(U"TitleText")(getData().gameState.currentRankName())
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
		const Circle bgmKnob{
			bgmBar.x + bgmBar.w * getData().sound.getBgmVolume(),
			bgmBar.y + bgmBar.h * 0.5, 10};
		const Circle seKnob{seBar.x + seBar.w * getData().sound.getSeVolume(),
							seBar.y + seBar.h * 0.5, 10};
		bgmKnob.draw(Palette::White).drawFrame(2, 0, Palette::Black);
		seKnob.draw(Palette::White).drawFrame(2, 0, Palette::Black);
	}

	// 著作権表示
	FontAsset(U"Copyright")(
		U"Version 0.5  2025/10/25\nCopyright\nプログラム・サウンド: Nasatame "
		U"\nプログラム・イラスト: moqueca "
		U"\n音楽：TAM Music Factory")
		.draw(Scene::Rect().leftCenter() + Vec2(50, 200), Palette::Black);
}
