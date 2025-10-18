#include "stdafx.h"

#include "TextLayout.hpp"

TextLayouter::TextLayouter(const Font& font, double maxWidth,
						   double lineHeightScale)
	: m_font{font}, m_maxLineWidth{maxWidth}, m_lineHeightScale{lineHeightScale}
{
}

Array<LayoutChar> TextLayouter::layout(const String& text) const
{
	Array<LayoutChar> out;

	const double lineHeight = m_font.height() * m_lineHeightScale;

	double x = 0.0;
	double y = 0.0;
	int32 index = 0;

	String token;

	// 折り返し候補（スペース）の位置を保持（必要時にのみ使用）
	Array<size_t> breakablePositions;

	auto flushWord = [&](const String& word)
	{
		for (const char32 ch : word)
		{
			const auto gi = m_font.getGlyph(ch);
			RectF box{Arg::topLeft = Vec2{x, y}, gi.xAdvance,
					  static_cast<double>(m_font.height())};
			out << LayoutChar{ch, Vec2{x, y}, box, index++};
			x += gi.xAdvance;
		}
	};

	for (size_t i = 0; i < text.size(); ++i)
	{
		const char32 ch = text[i];

		// 強制改行
		if (ch == U'\n')
		{
			flushWord(token);
			token.clear();
			x = 0.0;
			y += lineHeight;
			breakablePositions.clear();
			continue;
		}

		const bool isSpace = (ch == U' ' || ch == U'　');

		// スペースでトークンを区切る
		if (!isSpace)
		{
			token.push_back(ch);
			continue;
		}

		// 次の単語＋このスペースを載せられるか？（見積り）
		const double wordW = m_font(token).region().w;
		const double spaceW = m_font(U" ").region().w;

		if ((x + wordW + spaceW) > m_maxLineWidth &&
			!breakablePositions.isEmpty())
		{
			// 直前のスペースで改行
			x = 0.0;
			y += lineHeight;
			breakablePositions.clear();
		}

		// 単語出力
		flushWord(token);
		token.clear();

		// スペースを 1 文字として位置進行（可視描画しないが矩形は持つ）
		{
			const auto gi = m_font.getGlyph(U' ');
			RectF box{Arg::topLeft = Vec2{x, y}, gi.xAdvance,
					  static_cast<double>(m_font.height())};
			out << LayoutChar{U' ', Vec2{x, y}, box, index++};
			x += gi.xAdvance;
		}

		// 改行候補に登録
		breakablePositions << out.size();
	}

	// 残りの単語を吐き出し
	if (!token.isEmpty())
	{
		const double wordW = m_font(token).region().w;
		if ((x + wordW) > m_maxLineWidth && !breakablePositions.isEmpty())
		{
			x = 0.0;
			y += lineHeight;
			breakablePositions.clear();
		}
		flushWord(token);
		token.clear();
	}

	// 最終的に各 glyph の advance から box を更新（必要なら）
	for (auto& lc : out)
	{
		const auto gi = m_font.getGlyph(lc.ch);
		lc.box = RectF{Arg::topLeft = lc.pos, gi.xAdvance,
					   static_cast<double>(m_font.height())};
	}

	return out;
}
