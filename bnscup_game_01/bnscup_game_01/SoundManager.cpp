#include "SoundManager.hpp"
#include "GameConstants.hpp"

using namespace GameConstants;

void SoundManager::loadAssets()
{
	m_bgm = Audio{U"bgm_se/op01.mp3", Loop::Yes};
	m_seCorrect = Audio{U"bgm_se/correct1.mp3"};
	m_seWrong = Audio{U"bgm_se/wrong1.mp3"};
	m_seResult = Audio{U"bgm_se/result01.mp3"};
	loadRhythmAssets();
}

void SoundManager::playBGM()
{
	if (m_bgm && !m_bgm.isPlaying())
	{
		m_bgm.setVolume(m_bgmVolume);
		m_bgm.play();
	}
}

void SoundManager::stopBGM()
{
	if (m_bgm)
	{
		m_bgm.stop();
	}
}

void SoundManager::playCorrect()
{
	if (m_seCorrect)
	{
		m_seCorrect.playOneShot(m_seVolume);
	}
}

void SoundManager::playWrong()
{
	if (m_seWrong)
	{
		m_seWrong.setVolume(m_seVolume);
		m_seWrong.play();
	}
}

void SoundManager::playResult()
{
	if (m_seResult)
	{
		m_seResult.setVolume(m_seVolume);
		m_seResult.play();
	}
}

void SoundManager::setBgmVolume(double v)
{
	m_bgmVolume = Clamp(v, Game::MIN_VOLUME, Game::MAX_VOLUME);
	if (m_bgm)
	{
		m_bgm.setVolume(m_bgmVolume);
	}
}
void SoundManager::setSeVolume(double v)
{
	m_seVolume = Clamp(v, Game::MIN_VOLUME, Game::MAX_VOLUME);
}
double SoundManager::getBgmVolume() const noexcept
{
	return m_bgmVolume;
}
double SoundManager::getSeVolume() const noexcept
{
	return m_seVolume;
}

void SoundManager::loadRhythmAssets()
{
	// リズム用SE読み込み（音楽的なビート音）
	// 現在は既存のcorrect1.mp3を使用、専用音源があれば差し替え可能
	m_rhythmSe = Audio{U"bgm_se/notes.wav"};
}

void SoundManager::playRhythmBeat()
{
	if (m_rhythmSe)
	{
		m_rhythmSe.playOneShot(m_seVolume * 0.7);  // 少し音量を下げる
	}
}
