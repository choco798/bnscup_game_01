#pragma once

#include "SceneBase.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "SoundManager.hpp"
#include "Config.hpp"
#include "UiButton.hpp"

class GameScene : public KigoGameApp::Scene
{
   private:
	Array<LayoutChar> m_chars;
	bool m_showExplanation = false;
	bool m_result = false;
	double m_flowTime = 0.0f;
	Vec2 m_flowStartPos{};
	ui::Button m_noKigoBtn;

	void handleClick();
	void ExecWrong();
	void ExecCorrect();
	bool isHitKigo() const;

   public:
	GameScene(const InitData& init);
	void startProblem();
	void update() override;
	void draw() const override;
	void drawKigoRect() const;
	void drawWordRect(s3d::int32 i, const UIConfig& ui) const;
	void drawHiakuRect() const;
	Vec2 getKigoRectCenter() const;
	static RectF Inflate(const RectF& r, double padPx, double padScale);
};
