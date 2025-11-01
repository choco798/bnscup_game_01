#include "TitleScene.hpp"
#include "GameConstants.hpp"

using namespace GameConstants;

TitleScene::TitleScene(const InitData& init) : IScene{init}
{
	getData().sound.playBGM();

	getData().sound.setBgmVolume(getData().configManager.audio().bgmVolume);
	getData().sound.setSeVolume(getData().configManager.audio().seVolume);

	// 段位選択ボタンを初期化
	initializeGradeButtons();

	// メインボタンの位置を下にずらす
	m_startBtn = ui::Button(U"ゲーム開始", Fonts::KEY_TITLE,
							Vec2{Scene::Center().x, UI::START_BUTTON_Y_POS});
	m_howToBtn = ui::Button(U"遊び方", Fonts::KEY_TITLE_TEXT,
							Vec2{Scene::Center().x, UI::HOWTO_BUTTON_Y_POS});
	m_exitBtn = ui::Button(U"終了", Fonts::KEY_TITLE_TEXT,
						   Vec2{Scene::Center().x, UI::EXIT_BUTTON_Y_POS});
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

	const RectF bgmBar = UI::BGM_VOLUME_BAR;
	const RectF seBar = UI::SE_VOLUME_BAR;
	const Circle bgmKnob{bgmBar.x + bgmBar.w * getData().sound.getBgmVolume(),
						 bgmBar.y + bgmBar.h * UI::VOLUME_BAR_CENTER_RATIO,
						 UI::VOLUME_KNOB_RADIUS};
	const Circle seKnob{seBar.x + seBar.w * getData().sound.getSeVolume(),
						seBar.y + seBar.h * UI::VOLUME_BAR_CENTER_RATIO,
						UI::VOLUME_KNOB_RADIUS};
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
		const double t = Clamp((Cursor::PosF().x - bgmBar.x) / bgmBar.w,
							   Game::MIN_VOLUME, Game::MAX_VOLUME);
		getData().sound.setBgmVolume(t);
	}
	if (m_dragSe)
	{
		const double t = Clamp((Cursor::PosF().x - seBar.x) / seBar.w,
							   Game::MIN_VOLUME, Game::MAX_VOLUME);
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
	Rect{Scene::Size()}.draw(UI::TITLE_BACKGROUND_COLOR);

	FontAsset(Fonts::KEY_TITLE)(U"季語シンクロ！")
		.drawAt(Scene::Center().x, UI::TITLE_Y_POS, Palette::Black);

	// 段位選択UIを描画
	drawGradeButtons();

	m_startBtn.draw();
	m_howToBtn.draw();
	m_exitBtn.draw();

	// 追加: 現在の段位表示（左中パネル）
	{
		const RectF panel = UI::CURRENT_RANK_PANEL;
		panel.rounded(UI::EXPLANATION_PANEL_RADIUS)
			.draw(UI::EXPLANATION_PANEL_COLOR);
		panel.drawFrame(UI::HELP_OVERLAY_FRAME_THICKNESS, 0, Palette::Black);
		FontAsset(Fonts::KEY_TITLE_TEXT)(U"現在の段位")
			.draw(panel.pos + UI::CURRENT_RANK_TEXT_OFFSET,
				  Palette::Darkslategray);
		FontAsset(Fonts::KEY_TITLE_TEXT)(getData().gameState.currentRankName())
			.draw(panel.pos + UI::CURRENT_RANK_VALUE_OFFSET, Palette::Black);
	}

	// 追加: BGM / SE 音量スライダー（下部）
	{
		const RectF bgmBar = UI::BGM_VOLUME_BAR;
		const RectF seBar = UI::SE_VOLUME_BAR;
		FontAsset(Fonts::KEY_TITLE_TEXT)(U"BGM 音量")
			.draw(bgmBar.pos, Palette::Black);
		FontAsset(Fonts::KEY_TITLE_TEXT)(U"SE 音量")
			.draw(seBar.pos, Palette::Black);
		// バー
		bgmBar.draw(m_dragBgm ? UI::VOLUME_BAR_DRAG_COLOR
							  : UI::VOLUME_BAR_NORMAL_COLOR);
		seBar.draw(m_dragSe ? UI::VOLUME_BAR_DRAG_COLOR
							: UI::VOLUME_BAR_NORMAL_COLOR);
		// ノブ
		const Circle bgmKnob{
			bgmBar.x + bgmBar.w * getData().sound.getBgmVolume(),
			bgmBar.y + bgmBar.h * UI::VOLUME_BAR_CENTER_RATIO,
			UI::VOLUME_KNOB_RADIUS};
		const Circle seKnob{seBar.x + seBar.w * getData().sound.getSeVolume(),
							seBar.y + seBar.h * UI::VOLUME_BAR_CENTER_RATIO,
							UI::VOLUME_KNOB_RADIUS};
		bgmKnob.draw(Palette::White)
			.drawFrame(UI::VOLUME_KNOB_FRAME_THICKNESS, 0, Palette::Black);
		seKnob.draw(Palette::White)
			.drawFrame(UI::VOLUME_KNOB_FRAME_THICKNESS, 0, Palette::Black);
	}

	// 著作権表示
	FontAsset(Fonts::KEY_COPYRIGHT)(
		U"Version 0.5  2025/10/25\nCopyright\nプログラム・サウンド: Nasatame "
		U"\nプログラム・イラスト: moqueca "
		U"\n音楽：TAM Music Factory")
		.draw(Scene::Rect().leftCenter() + UI::COPYRIGHT_OFFSET,
			  Palette::Black);
}

void TitleScene::initializeGradeButtons()
{
	m_gradeButtons.clear();

	// 段位選択ボタンを作成
	const Vec2 basePos = UI::GRADE_BUTTON_BASE_POS;
	const double yStep = UI::GRADE_BUTTON_Y_STEP;

	// 各段位のボタンを作成
	for (int32 grade = 0; grade < ProblemGrade::Count; ++grade)
	{
		String buttonText =
			Format(getData().gameState.getRankName(grade), U"段位");
		if (!getData().gameState.isGradeAvailable(grade))
		{
			buttonText += U" (未開放)";
		}

		auto button = ui::Button(buttonText, Fonts::KEY_TITLE_TEXT,
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
		const RectF panel = UI::GRADE_SELECTION_PANEL;
		panel.rounded(UI::EXPLANATION_PANEL_RADIUS)
			.draw(UI::EXPLANATION_PANEL_COLOR);
		panel.drawFrame(UI::HELP_OVERLAY_FRAME_THICKNESS, 0, Palette::Black);
		FontAsset(Fonts::KEY_TITLE_TEXT)(U"段位選択")
			.drawAt(panel.pos + UI::GRADE_SELECTION_TITLE_OFFSET,
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
			rect.stretched(UI::GRADE_SELECTION_HIGHLIGHT_STRETCH)
				.draw(UI::GRADE_SELECTION_HIGHLIGHT);
		}

		// ボタンを描画
		button.draw();
	}
}
