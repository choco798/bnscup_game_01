#pragma once

#include "TextLayout.hpp"

class Renderer
{
   private:
	Texture m_bg;
	Texture m_teacherNormal;
	Texture m_teacherHappy;
	Texture m_teacherAngry;
	String m_fontName;

   public:
	void initRenderer(const StringView& fontName);
	void drawBackground() const;
	void drawHaiku(const Array<LayoutChar>& chars) const;
	void drawHaikuWithRuby(const Array<LayoutChar>& chars) const;
	void drawTeacherNormal() const;
	void drawTeacherHappy() const;
	void drawTeacherAngry() const;
	void drawExplanation(const String& kigo, const String& season,
						 const String& text) const;
	void drawTutorial(const String& text) const;

   private:
	void drawRubyText(const String& ruby, const Vec2& pos) const;
};
