#pragma once

#include "SceneBase.hpp"
#include "SoundManager.hpp"

class TitleScene : public SceneBase
{
   public:
	TitleScene(SoundManager& sound);
	void update(bool enable) override;
	void draw() const override;

	private:
	SoundManager& m_sound;
	Font normal_font;
	Font small_font;
};
