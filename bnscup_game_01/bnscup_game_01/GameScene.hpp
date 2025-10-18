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
	Font m_font;

	Array<LayoutChar> m_chars;
	bool m_showExplanation = false;
	bool m_result = false;

	void handleClick();
	bool isHitKigo() const;

   public:
	GameScene(GameState& state, Renderer& renderer, SoundManager& sound,
			  Config& config);
	void startProblem();
	void update() override;
	void draw() const override;
	static RectF Inflate(const RectF& r, double padPx, double padScale);
};
