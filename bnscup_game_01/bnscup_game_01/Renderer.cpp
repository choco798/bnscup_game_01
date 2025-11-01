#include "Renderer.hpp"
#include "GameConstants.hpp"

using namespace GameConstants;

void Renderer::initRenderer(const StringView& fontName)
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
		Rect{Scene::Size()}.draw(UI::FALLBACK_BACKGROUND_COLOR);
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
		m_teacherNormal.draw(UI::TEACHER_POSITION);
	}
}

void Renderer::drawTeacherHappy() const
{
	if (m_teacherHappy)
	{
		m_teacherHappy.draw(UI::TEACHER_POSITION);
	}
}

void Renderer::drawTeacherAngry() const
{
	if (m_teacherAngry)
	{
		m_teacherAngry.draw(UI::TEACHER_POSITION);
	}
}

void Renderer::drawExplanation(const String& kigo, const String& season,
							   const String& text) const
{
	String show_text = text;
	show_text.replace(U"*", U"\n");

	String kigo_show_text = U"季語 : " + kigo;

	const RectF panelSeason = UI::EXPLANATION_SEASON_PANEL;
	panelSeason.rounded(UI::EXPLANATION_PANEL_RADIUS)
		.draw(UI::EXPLANATION_PANEL_COLOR);
	panelSeason.drawFrame(UI::EXPLANATION_FRAME_THICKNESS, 0, Palette::Gray);

	const RectF panelKigo{
		UI::EXPLANATION_KIGO_BASE_POS.x, UI::EXPLANATION_KIGO_BASE_POS.y,
		UI::EXPLANATION_KIGO_BASE_SIZE.x +
			FontAsset(Fonts::KEY_EXPLANATION)(kigo_show_text).region().w,
		UI::EXPLANATION_KIGO_BASE_SIZE.y};
	panelKigo.rounded(UI::EXPLANATION_PANEL_RADIUS)
		.draw(UI::EXPLANATION_PANEL_COLOR);
	panelKigo.drawFrame(UI::EXPLANATION_FRAME_THICKNESS, 0, Palette::Gray);

	const RectF panelMain = UI::EXPLANATION_MAIN_PANEL;
	panelMain.rounded(UI::EXPLANATION_PANEL_RADIUS)
		.draw(UI::EXPLANATION_PANEL_COLOR);
	panelMain.drawFrame(UI::EXPLANATION_FRAME_THICKNESS, 0, Palette::Gray);

	FontAsset(Fonts::KEY_EXPLANATION)(season).draw(
		panelSeason.pos.movedBy(UI::EXPLANATION_SMALL_OFFSET), Palette::Black);
	FontAsset(Fonts::KEY_EXPLANATION)(kigo_show_text)
		.draw(panelKigo.pos.movedBy(UI::EXPLANATION_SMALL_OFFSET),
			  Palette::Black);

	FontAsset(Fonts::KEY_EXPLANATION)(show_text).draw(
		panelMain.pos.movedBy(UI::EXPLANATION_TEXT_OFFSET), Palette::Black);
}

void Renderer::drawTutorial(const String& text) const
{
	const RectF panel = UI::EXPLANATION_MAIN_PANEL;
	panel.rounded(UI::EXPLANATION_PANEL_RADIUS)
		.draw(UI::EXPLANATION_PANEL_COLOR);
	panel.drawFrame(UI::EXPLANATION_FRAME_THICKNESS, 0, Palette::Gray);

	String show_text = text;
	show_text.replace(U"*", U"\n");

	FontAsset(Fonts::KEY_EXPLANATION)(show_text).draw(
		panel.pos.movedBy(UI::EXPLANATION_TEXT_OFFSET), Palette::Black);
}
