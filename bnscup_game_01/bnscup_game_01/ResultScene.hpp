#pragma once

#include "SceneBase.hpp"
#include "UiButton.hpp"

class ResultScene : public KigoGameApp::Scene
{
   private:
	ui::Button m_backBtn{};

   public:
	ResultScene(const InitData& data);
	void update() override;
	void draw() const override;
};
