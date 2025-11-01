#include "SoundManager.hpp"
#include "GameConstants.hpp"

using namespace GameConstants;

void SoundManager::loadAssets()
{
	m_bgm = Audio{U"bgm_se/op01.mp3", Loop::Yes};
	m_seCorrect = Audio{U"bgm_se/correct1.mp3"};
	m_seWrong = Audio{U"bgm_se/wrong1.mp3"};
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
		m_seWrong.playOneShot(m_seVolume);
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
