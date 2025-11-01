#include "TitleScene.hpp"

TitleScene::TitleScene(const InitData& init) : IScene{init}
{
	getData().sound.playBGM();

	getData().sound.setBgmVolume(getData().configManager.audio().bgmVolume);
	getData().sound.setSeVolume(getData().configManager.audio().seVolume);

	// 段位選択ボタンを初期化
	initializeGradeButtons();

	// メインボタンの位置を下にずらす
	m_startBtn =
		ui::Button(U"ゲーム開始", U"Title", Vec2{Scene::Center().x, 430});
	m_howToBtn =
		ui::Button(U"遊び方", U"TitleText", Vec2{Scene::Center().x, 560});
	m_exitBtn = ui::Button(U"終了", U"TitleText", Vec2{Scene::Center().x, 640});
}

void TitleScene::update()
{
	// 入力は簡易スライダーのみ処理。遷移は Main 側で制御。
	// シーン遷移は外側のコントローラで行う想定。

	// ボタン
	// メインボタンの更新
	m_startBtn.update();
	m_howToBtn.update();
	m_exitBtn.update();

	// 段位選択ボタンの更新
	updateGradeButtons();

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
		// 段位が選択されているか確認
		if (getData().gameState.getSelectedGrade() < 0)
		{
			System::MessageBoxOK(U"段位を選択してください。");
			return;
		}

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

	// 段位選択UIを描画
	drawGradeButtons();

	m_startBtn.draw();
	m_howToBtn.draw();
	m_exitBtn.draw();

	// 追加: 現在の段位表示（左中パネル）
	{
		const RectF panel{50, 250, 300, 100};
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

void TitleScene::initializeGradeButtons()
{
	m_gradeButtons.clear();

	// 段位選択ボタンを作成
	const Vec2 basePos{1050, 400};
	const double yStep = 40;

	// 各段位のボタンを作成
	for (int32 grade = 0; grade < ProblemGrade::Count; ++grade)
	{
		String buttonText = Format(getData().gameState.getRankName(grade), U"段位");
		if (!getData().gameState.isGradeAvailable(grade))
		{
			buttonText += U" (未開放)";
		}

		auto button = ui::Button(buttonText, U"TitleText",
								 Vec2{basePos.x, basePos.y + grade * yStep});

		// 利用不可能な段位のボタンは無効化
		if (!getData().gameState.isGradeAvailable(grade))
		{
			button.setEnabled(false);
		}

		m_gradeButtons << button;
	}
	// 現在の段位に設定
	getData().gameState.setSelectedHighestGrade();
}

void TitleScene::updateGradeButtons()
{
	for (size_t i = 0; i < m_gradeButtons.size(); ++i)
	{
		const int32 grade = static_cast<int32>(i);
		if (getData().gameState.isGradeAvailable(grade))
		{
			auto& button = m_gradeButtons[i];
			button.update();

			if (button.isClicked())
			{
				getData().gameState.setSelectedGrade(grade);
				getData().sound.playCorrect();	// 正解音を使用
			}
		}
	}
}

void TitleScene::drawGradeButtons() const
{
	// 段位選択パネル
	{
		const RectF panel{900, 300, 300, 280};
		panel.rounded(16).draw(ColorF{1.0, 1.0, 1.0, 0.92});
		panel.drawFrame(3, 0, Palette::Black);
		FontAsset(U"TitleText")(U"段位選択")
			.drawAt(panel.x + panel.w / 2, panel.y + 20,
					Palette::Darkslategray);
	}

	// 段位選択ボタンを描画
	for (size_t i = 0; i < m_gradeButtons.size(); ++i)
	{
		const int32 grade = static_cast<int32>(i);
		if (!getData().gameState.isGradeAvailable(grade))
		{
			continue;
		}
		const auto& button = m_gradeButtons[i];

		// 選択状態を反映
		if (getData().gameState.getSelectedGrade() == grade)
		{
			// 選択中の段位は強調表示
			const RoundRect& buttonRect = button.roundRect();
			const RectF rect = buttonRect.rect;	 // RoundRectからRectFを取得
			rect.stretched(4).draw(ColorF{0.3, 0.6, 1.0, 0.6});
		}

		// ボタンを描画
		button.draw();
	}
}
