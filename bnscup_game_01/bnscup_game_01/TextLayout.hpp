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
	double m_clientSize;

   public:
	TextLayouter(const String fontName, double maxWidth, double lineHeightScale,
				 double lineWidthScale, double clientSize);
	Array<LayoutChar> layout(const String &text) const;

	void InsertBreakInText(double &text_y, double &text_x, double lineWidth,
						   double localFontY, int32_t &lineCount,
						   int32_t &headPos,
						   s3d::Array<size_t> &breakablePositions) const;
};
