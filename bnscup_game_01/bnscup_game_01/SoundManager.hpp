#pragma once

class SoundManager
{
   private:
	Audio m_bgm;
	Audio m_seCorrect;
	Audio m_seWrong;

   public:
	void loadAssets();
	void playBGM();
	void stopBGM();
	void playCorrect();
	void playWrong();
};
