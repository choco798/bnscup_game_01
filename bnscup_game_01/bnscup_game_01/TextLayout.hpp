#pragma once


struct LayoutChar
{
	char32 ch;			// 文字
	Vec2 pos;			// 描画位置
	RectF box;			// 当たり判定矩形
	int32 globalIndex;	// text内インデックス
};

class TextLayouter
{
   private:
	const Font& m_font;
	double m_maxLineWidth;
	double m_lineHeightScale;
	double m_lineWidthScale;

   public:
	TextLayouter(const Font& font, double maxWidth, double lineHeightScale,
				 double lineWidthScale);
	Array<LayoutChar> layout(const String& text) const;
};
