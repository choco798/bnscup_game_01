#pragma once

#include "SceneBase.hpp"
#include "SoundManager.hpp"
#include "Config.hpp"
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

	ui::Button m_startBtn{};
	ui::Button m_howToBtn{};
	ui::Button m_exitBtn{};
};
