#pragma once

#include "SceneBase.hpp"
#include "UiButton.hpp"

class TitleScene : public KigoGameApp::Scene
{
   public:
	TitleScene(const InitData& init);
	void update() override;
	void draw() const override;

   private:
	// 簡易スライダー UI 状態
	bool m_dragBgm = false;
	bool m_dragSe = false;

	// ボタン
	ui::Button m_startBtn{};
	ui::Button m_howToBtn{};
	ui::Button m_exitBtn{};

	// 段位選択ボタン
	Array<ui::Button> m_gradeButtons{};

	// メソッド
	void initializeGradeButtons();
	void updateGradeButtons();
	void drawGradeButtons() const;

	// update()の分割メソッド
	void updateMainButtons();
	void updateVolumeSliders();
	void handleButtonClicks();
	bool validateGameStart();

	// draw()の分割メソッド
	void drawCurrentRankPanel() const;
	void drawVolumeControls() const;
	void drawCopyrightInfo() const;
};
