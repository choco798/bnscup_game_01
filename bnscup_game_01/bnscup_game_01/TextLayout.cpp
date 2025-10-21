#include "stdafx.h"

#include "TextLayout.hpp"

TextLayouter::TextLayouter(const String fontName, double maxWidth,
						   double lineHeightScale, double lineWidthScale)
	: m_fontName{fontName},
	  m_maxLineWidth{maxWidth},
	  m_lineHeightScale{lineHeightScale},
	  m_lineWidthScale{lineWidthScale}
{
}

Array<LayoutChar> TextLayouter::layout(const String& text) const
{
	Array<LayoutChar> out;

	double lineWidth = 0;

	const Font& localFont = FontAsset(m_fontName);

	for (size_t i = 0; i < text.size(); ++i)
	{
		lineWidth = Max(
			lineWidth, localFont.getGlyph(text[i]).xAdvance *
							   m_lineWidthScale);
	}

	double text_x = 0.0;
	double text_y = 0.0;
	int32 index = 0;

	String token;

	// 折り返し候補（スペース）の位置を保持（必要時にのみ使用）
	Array<size_t> breakablePositions;

	auto flushWord = [&](const String& word)
	{
		for (const char32 ch : word)
		{
			const auto gi = localFont.getGlyph(ch);
			RectF box{Arg::topLeft = Vec2{text_x, text_y}, gi.xAdvance,
					  static_cast<double>(localFont.height())};
			out << LayoutChar{ch, Vec2{text_x, text_y}, box, index++};
			text_y += static_cast<double>(localFont.height());
		}
	};

	for (size_t i = 0; i < text.size(); ++i)
	{
		const char32 ch = text[i];

		// 強制改行
		if (ch == U'*')
		{
			flushWord(token);
			token.clear();
			text_y = 0.0;
			text_x += lineWidth;
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
		//const double wordW = m_font(token).region().w;
		//const double spaceW = m_font(U" ").region().w;

		const bool needBreak = true;

		// 長さで改行をするか決める場合はこちらを利用する
		// (x + wordW + spaceW) > m_maxLineWidth && !breakablePositions.isEmpty();

		if (needBreak)
		{
			// 直前のスペースで改行
			text_y = 0.0;
			text_x += lineWidth;
			breakablePositions.clear();
		}

		// 単語出力
		flushWord(token);
		token.clear();

		// スペースを 1 文字として位置進行（可視描画しないが矩形は持つ）
		{
			const auto gi = localFont.getGlyph(U' ');
			RectF box{Arg::topLeft = Vec2{text_x, text_y}, gi.xAdvance,
					  static_cast<double>(localFont.height())};
			out << LayoutChar{U' ', Vec2{text_x, text_y}, box, index++};
			text_y += static_cast<double>(localFont.height());
		}

		// 改行候補に登録
		breakablePositions << out.size();
	}

	// 残りの単語を吐き出し
	if (!token.isEmpty())
	{

		//const double wordW = m_font(token).region().w;
		const bool needBreak = true;
		// 長さで改行をするか決める場合はこちらを利用する
		// (x + wordW) > m_maxLineWidth && !breakablePositions.isEmpty()
		if (needBreak)
		{
			text_y = 0.0;
			text_x += lineWidth;
			breakablePositions.clear();
		}
		flushWord(token);
		token.clear();
	}

	// 最終的に各 glyph の advance から box を更新（必要なら）
	for (auto& lc : out)
	{
		const auto gi = localFont.getGlyph(lc.ch);
		lc.box = RectF{Arg::topLeft = lc.pos, gi.xAdvance,
					   static_cast<double>(localFont.height())};
	}

	return out;
}
