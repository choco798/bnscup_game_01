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
	const String m_fontName;
	double m_maxLineWidth;
	double m_lineHeightScale;
	double m_lineWidthScale;

   public:
	TextLayouter(const String fontName, double maxWidth, double lineHeightScale,
				 double lineWidthScale);
	Array<LayoutChar> layout(const String& text) const;
};
