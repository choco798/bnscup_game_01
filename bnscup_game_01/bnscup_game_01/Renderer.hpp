#pragma once

#include "GameState.hpp"
#include "TextLayout.hpp"
#include "Config.hpp"

class Renderer
{
   private:
	Texture m_bg;
	Texture m_teacherNormal;
	Texture m_teacherHappy;
	Texture m_teacherAngry;
	String m_fontName;

   public:
	void initRenderer(const String fontName);
	void drawBackground() const;
	void drawHaiku(const Array<LayoutChar>& chars) const;
	void drawTeacherNormal() const;
	void drawTeacherHappy() const;
	void drawTeacherAngry() const;
	void drawExplanation(const String& text) const;
	void drawTutorial(const String& text) const;
};
