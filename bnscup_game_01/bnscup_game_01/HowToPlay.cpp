#include "stdafx.h"
#include "HowToPlay.hpp"

// 簡易ヘルプ表示
void DrawHowToOverlay()
{
	const RectF panel{140, 140, 1000, 440};
	panel.rounded(16).draw(ColorF{1.0, 1.0, 1.0, 0.96});
	panel.drawFrame(3, 0, Palette::Black);
	FontAsset(U"Game")(U"あそびかた").draw(160, 160, Palette::Black);

	FontAsset(U"HowToPlay")(U"・俳句の中の季語の文字をクリックします。")
		.draw(160, 220, Palette::Black);
	FontAsset(U"HowToPlay")(U"・季語が無い句は「季語なし」をクリックします。")
		.draw(160, 260, Palette::Black);
	FontAsset(U"HowToPlay")(U"・正解で点数が追加。解説が表示されます。")
		.draw(160, 300, Palette::Black);
}
