#pragma once

#include "SceneBase.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "SoundManager.hpp"
#include "Config.hpp"

class GameScene : public SceneBase
{
   private:
	GameState& m_state;
	Renderer& m_renderer;
	SoundManager& m_sound;
	Config& m_config;

	Array<LayoutChar> m_chars;
	bool m_showExplanation = false;
	bool m_result = false;
	double m_flowTime = 0.0f;
	Vec2 m_flowStartPos{};

	void handleClick();
	void ExecWrong();
	void ExecCorrect();
	bool isHitKigo() const;

   public:
	GameScene(GameState& state, Renderer& renderer, SoundManager& sound,
			  Config& config);
	void startProblem();
	void update(bool enable) override;
	void draw() const override;
	void drawKigoRect() const;
	void drawWordRect(s3d::int32 i, const UIConfig& ui) const;
	void drawHiakuRect() const;
	Vec2 getKigoRectCenter() const;
	static RectF Inflate(const RectF& r, double padPx, double padScale);
};
