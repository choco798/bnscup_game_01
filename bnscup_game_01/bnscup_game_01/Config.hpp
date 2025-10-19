#pragma once

struct UIConfig
{
	double maxLineWidth = 920.0;
	double lineHeightScale = 1.25;
	double lineWidthScale = 1.25;
	double hitboxPaddingPx = 6.0;
	double hitboxPaddingScale = 0.1;
	int32_t wordWidth = 80;
};

class Config
{
   private:
	UIConfig m_ui;

   public:
	bool load(const FilePath& path);
	const UIConfig& ui() const noexcept;
};
