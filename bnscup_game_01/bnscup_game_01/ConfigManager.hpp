#pragma once
#include "Config.hpp"

class ConfigManager
{
   private:
	UIConfig m_ui;
	AudioConfig m_audio;
	FilePath m_configPath;

   public:
	ConfigManager() = default;
	~ConfigManager() = default;

	// 設定ファイルの読み込み
	bool initialize(const FilePath& path);

	// UIConfig へのアクセス
	const UIConfig& ui() const noexcept
	{
		return m_ui;
	}
	UIConfig& ui() noexcept
	{
		return m_ui;
	}

	// AudioConfig へのアクセス
	const AudioConfig& audio() const noexcept
	{
		return m_audio;
	}
	AudioConfig& audio() noexcept
	{
		return m_audio;
	}

	// 設定の保存
	bool save() const;
};
