#include "stdafx.h"

#include "SoundManager.hpp"

	void SoundManager::loadAssets()
{
	m_bgm = Audio{U"elegant_loop.ogg", Loop::Yes};
	m_seCorrect = Audio{U"correct.wav"};
	m_seWrong = Audio{U"wrong.wav"};
}

void SoundManager::playBGM()
{
	if (m_bgm && !m_bgm.isPlaying())
	{
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
		m_seCorrect.playOneShot();
	}
}

void SoundManager::playWrong()
{
	if (m_seWrong)
	{
		m_seWrong.playOneShot();
	}
}
