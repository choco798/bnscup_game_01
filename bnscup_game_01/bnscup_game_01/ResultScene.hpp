#pragma once

#include "GameState.hpp"
#include "Renderer.hpp"
#include "SoundManager.hpp"
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
