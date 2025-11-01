#include "HowToPlay.hpp"
#include "GameConstants.hpp"

using namespace GameConstants;

// 簡易ヘルプ表示
void DrawHowToOverlay()
{
	const RectF panel = UI::HELP_OVERLAY_PANEL;
	panel.rounded(UI::EXPLANATION_PANEL_RADIUS).draw(UI::HELP_OVERLAY_COLOR);
	panel.drawFrame(UI::HELP_OVERLAY_FRAME_THICKNESS, 0, Palette::Black);
	FontAsset(Fonts::KEY_GAME)(U"あそびかた")
		.draw(UI::HELP_TITLE_POS, Palette::Black);

	FontAsset(Fonts::KEY_HOW_TO_PLAY)(U"・俳句の中の季語の文字をクリックします。")
		.draw(UI::HELP_TEXT_BASE_POS, Palette::Black);
	FontAsset(Fonts::KEY_HOW_TO_PLAY)(
		U"・俳句にシンクロして、読み上げるとヒントポイントがもらえます。")
		.draw(UI::HELP_TEXT_BASE_POS.movedBy(0, UI::HELP_TEXT_LINE_HEIGHT),
			  Palette::Black);
	FontAsset(Fonts::KEY_HOW_TO_PLAY)(
		U"・季語が無い句は「季語なし」をクリックします。")
		.draw(UI::HELP_TEXT_BASE_POS.movedBy(0, UI::HELP_TEXT_LINE_HEIGHT * 2),
			  Palette::Black);
	FontAsset(Fonts::KEY_HOW_TO_PLAY)(
		U"・正解で点数が追加。解説が表示されます。")
		.draw(UI::HELP_TEXT_BASE_POS.movedBy(0, UI::HELP_TEXT_LINE_HEIGHT * 3),
			  Palette::Black);
	FontAsset(Fonts::KEY_HOW_TO_PLAY)(
		U"・間違えると、スコアが無くなる代わりにヒントポイントがもらえます。")
		.draw(UI::HELP_TEXT_BASE_POS.movedBy(0, UI::HELP_TEXT_LINE_HEIGHT * 4),
			  Palette::Black);
	FontAsset(Fonts::KEY_HOW_TO_PLAY)(
		U"・ヒントポイントが溜まると、ヒントの方向が分かります！")
		.draw(UI::HELP_TEXT_BASE_POS.movedBy(0, UI::HELP_TEXT_LINE_HEIGHT * 5),
			  Palette::Black);
}
