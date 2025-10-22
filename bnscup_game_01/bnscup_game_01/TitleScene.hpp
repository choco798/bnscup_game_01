#pragma once

#include "SceneBase.hpp"
#include "SoundManager.hpp"
#include "Config.hpp"

class TitleScene : public SceneBase
{
   public:
	TitleScene(SoundManager& sound, String rankName, const Config& config);
	void update(bool enable) override;
	void draw() const override;

	private:
	SoundManager& m_sound;
	String m_rankName;

	// 簡易スライダー UI 状態
	bool m_dragBgm = false;
	bool m_dragSe = false;
};
