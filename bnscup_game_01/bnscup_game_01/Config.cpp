#include "stdafx.h"

#include "Config.hpp"

bool Config::load(const FilePath& path)
{
	const JSON json = JSON::Load(path);
	if (!json)
	{
		// 既定値のまま false
		return false;
	}

	if (const auto ui = json[U"ui"])
	{
		m_ui.maxLineWidth =
			ui[U"maxLineWidth"].getOr<double>(m_ui.maxLineWidth);
		m_ui.lineHeightScale =
			ui[U"lineHeightScale"].getOr<double>(m_ui.lineHeightScale);
		m_ui.hitboxPaddingPx =
			ui[U"hitboxPaddingPx"].getOr<double>(m_ui.hitboxPaddingPx);
		m_ui.hitboxPaddingScale =
			ui[U"hitboxPaddingScale"].getOr<double>(m_ui.hitboxPaddingScale);
	}

	if (const auto au = json[U"audio"])
	{
		m_audio.bgmVolume = au[U"bgmVolume"].getOr<double>(m_audio.bgmVolume);
		m_audio.seVolume = au[U"seVolume"].getOr<double>(m_audio.seVolume);
	}
	return true;
}

const UIConfig& Config::ui() const noexcept
{
	return m_ui;
}

const AudioConfig& Config::audio() const noexcept
{
	return m_audio;
}
