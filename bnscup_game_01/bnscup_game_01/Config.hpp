#pragma once

struct UIConfig
{
	double maxLineWidth = 920.0;
	double lineHeightScale = 1.25;
	double lineWidthScale = 1.25;
	double hitboxPaddingPx = 6.0;
	double hitboxPaddingScale = 0.1;
	int32_t wordWidth = 80;
	double clientSize = 1280;
};

// AudioConfig 構造体とアクセサ
struct AudioConfig
{
	double bgmVolume = 0.6;	 // 0.0 - 1.0
	double seVolume = 0.8;	 // 0.0 - 1.0
};

class Config
{
   private:
	UIConfig m_ui;
	AudioConfig m_audio;

   public:
	bool load(const FilePath& path);
	const UIConfig& ui() const noexcept;
	const AudioConfig& audio() const noexcept;
};
