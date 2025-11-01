#include "TextLayout.hpp"
#include "GameConstants.hpp"

using namespace GameConstants;

TextLayouter::TextLayouter(const StringView& fontName, double maxWidth,
						   double lineHeightScale, double lineWidthScale,
						   double clientSize)
	: m_fontName{fontName},
	  m_maxLineWidth{maxWidth},
	  m_lineHeightScale{lineHeightScale},
	  m_lineWidthScale{lineWidthScale},
	  m_clientSize{clientSize}
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
			lineWidth, localFont.getGlyph(text[i]).xAdvance * m_lineWidthScale);
	}

	double text_x = 0.0;
	double text_y = 0.0;
	int32 index = 0;
	int32 lineCount = UI::LINE_COUNT_START;
	int32 headPos = UI::HEAD_POSITION_START;

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

			InsertBreakInText(text_y, text_x, lineWidth,
							  static_cast<double>(localFont.height()),
							  lineCount, headPos, breakablePositions);
			continue;
		}

		// 強制y位置を3つ戻す
		if (ch == U'^')
		{
			text_y = static_cast<double>(localFont.height()) *
					 (headPos - UI::BACK_POSITION_OFFSET - 1);
			headPos -= UI::BACK_POSITION_OFFSET;
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
		// const double wordW = m_font(token).region().w;
		// const double spaceW = m_font(U" ").region().w;

		// 単語出力
		flushWord(token);
		token.clear();

		const bool needBreak = true;
		// 長さで改行をするか決める場合はこちらを利用する
		// (x + wordW + spaceW) > m_maxLineWidth &&
		// !breakablePositions.isEmpty();
		if (needBreak)
		{
			// 直前のスペースで改行
			InsertBreakInText(text_y, text_x, lineWidth,
							  static_cast<double>(localFont.height()),
							  lineCount, headPos, breakablePositions);
		}

		const bool disp_space_box = UI::DISPLAY_SPACE_BOX;
		// スペースを 1 文字として位置進行（可視描画しないが矩形は持つ）
		if (disp_space_box)
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

void TextLayouter::InsertBreakInText(
	double& text_y, double& text_x, double lineWidth, double localFontY,
	int32_t& lineCount, int32_t& headPos,
	s3d::Array<size_t>& breakablePositions) const
{
	// 改行処理
	text_y = localFontY * headPos;
	text_x -= lineWidth;
	breakablePositions.clear();
	++lineCount;
	++headPos;
}
