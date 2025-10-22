#pragma once

class SoundManager
{
   private:
	Audio m_bgm;
	Audio m_seCorrect;
	Audio m_seWrong;
	double m_bgmVolume = 0.6;
	double m_seVolume = 0.8;

   public:
	void loadAssets();
	void playBGM();
	void stopBGM();
	void playCorrect();
	void playWrong();

	// 追加: 音量制御（0.0 - 1.0）
	void setBgmVolume(double v);
	void setSeVolume(double v);
	double getBgmVolume() const noexcept;
	double getSeVolume() const noexcept;
};
