#pragma once

#include "GameState.hpp"
#include "Renderer.hpp"
#include "SoundManager.hpp"
#include "SceneBase.hpp"

class ResultScene : public SceneBase
{
   private:
	const GameState& m_state;
	Renderer& m_renderer;
	SoundManager& m_sound;

   public:
	ResultScene(const GameState& state, Renderer& renderer,
				SoundManager& sound);
	void update() override;
	void draw() const override;
};
