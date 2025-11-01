#include "ResultScene.hpp"
#include "GameConstants.hpp"

using namespace GameConstants;

ResultScene::ResultScene(const InitData& data) : IScene{data}
{
	const Size rect_size = UI::RESULT_BUTTON_SIZE;
	m_backBtn = ui::Button(
		U"タイトルへ", Fonts::KEY_SCORE,
		UI::RESULT_BUTTON_BASE_POS + Vec2{rect_size.x / 2, rect_size.y / 2});
}

void ResultScene::update()
{
	m_backBtn.update();
	if (m_backBtn.isClicked())
	{
		// ゲーム終了時に自動セーブを実行
		auto& saveDataManager = getData().saveDataManager;

		// 昇段判定と処理
		auto& gameState = getData().gameState;
		gameState.updateProgress();
		if (gameState.canPromote())
		{
			String prevRankName = gameState.currentRankName();

			gameState.updateRank();

			// セーブ失敗時はエラーメッセージを表示
			System::MessageBoxOK(U"昇格通知",
								 U" {} から {} へ昇格しました！\nおめでとうございます！！！ "_fmt(
									 gameState.currentRankName(), prevRankName),
								 MessageBoxStyle::Info);
		}

		// セーブを実行（1回のみ）
		if (!saveDataManager.save())
		{
			// セーブ失敗時はエラーメッセージを表示
			System::MessageBoxOK(U"エラー", U"データの保存に失敗しました",
								 MessageBoxStyle::Error);
		}

		changeScene(State::Title);
	}
}

void ResultScene::draw() const
{
	getData().renderer.drawBackground();

	FontAsset(Fonts::KEY_RESULT)(U"結果発表")
		.drawAt(Scene::Center().x, UI::RESULT_TITLE_Y_POS, Palette::Black);

	RoundRect panel{UI::RESULT_PANEL, UI::RESULT_PANEL_RADIUS};
	panel.draw(UI::EXPLANATION_PANEL_COLOR)
		.drawFrame(UI::HELP_OVERLAY_FRAME_THICKNESS, 0, Palette::Black);

	FontAsset(Fonts::KEY_SCORE)(
		U"正解数: {}"_fmt(getData().gameState.correctCount))
		.draw(UI::RESULT_CORRECT_COUNT_POS, Palette::Black);
	FontAsset(Fonts::KEY_SCORE)(U"スコア: {}"_fmt(getData().gameState.score))
		.draw(UI::RESULT_SCORE_POS, Palette::Black);

	m_backBtn.draw();
}
