#include "Renderer.hpp"

void Renderer::initRenderer(const String fontName)
{
	m_fontName = fontName;
	m_bg = {Texture{U"Assets/images/bg_paper.png"}};
	m_teacherNormal = {Texture{U"Assets/images/teacher_normal.png"}};
	m_teacherHappy = {Texture{U"Assets/images/teacher_happy.png"}};
	m_teacherAngry = Texture{U"Assets/images/teacher_angry.png"};
}

void Renderer::drawBackground() const
{
	if (m_bg)
	{
		m_bg.scaled(Scene::Width() / m_bg.width(),
					Scene::Height() / m_bg.height())
			.draw();
	}
	else
	{
		Rect{Scene::Size()}.draw(ColorF{0.97, 0.97, 0.94});
	}
}

void Renderer::drawHaiku(const Array<LayoutChar>& chars) const
{
	for (const auto& c : chars)
	{
		FontAsset(m_fontName)(String{1, c.ch}).draw(c.pos, Palette::Black);
	}
}

void Renderer::drawTeacherNormal() const
{
	if (m_teacherNormal)
	{
		m_teacherNormal.draw(1000, 420);
	}
}

void Renderer::drawTeacherHappy() const
{
	if (m_teacherHappy)
	{
		m_teacherHappy.draw(1000, 420);
	}
}

void Renderer::drawTeacherAngry() const
{
	if (m_teacherAngry)
	{
		m_teacherAngry.draw(1000, 420);
	}
}

void Renderer::drawExplanation(const String& text) const
{
	const RectF panel{60, 540, 860, 140};
	panel.rounded(16).draw(ColorF{1.0, 1.0, 1.0, 0.9});
	panel.drawFrame(2, 0, Palette::Gray);

	String show_text = text;
	show_text.replace(U"*", U"\n");

	FontAsset(U"Explanation")(show_text).draw(panel.pos.movedBy(30, 16),
											  Palette::Black);
}

void Renderer::drawTutorial(const String& text) const
{
	const RectF panel{60, 540, 860, 140};
	panel.rounded(16).draw(ColorF{1.0, 1.0, 1.0, 0.9});
	panel.drawFrame(2, 0, Palette::Gray);

	String show_text = text;
	show_text.replace(U"*", U"\n");

	FontAsset(U"Explanation")(show_text).draw(panel.pos.movedBy(30, 16),
											  Palette::Black);
}
