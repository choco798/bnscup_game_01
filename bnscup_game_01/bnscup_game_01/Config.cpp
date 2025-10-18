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
	return true;
}

const UIConfig& Config::ui() const noexcept
{
	return m_ui;
}
