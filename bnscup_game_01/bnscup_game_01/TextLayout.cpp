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
	const Font& localFont = FontAsset(m_fontName);

	// 最大行幅を計算
	const double lineWidth = calculateMaxLineWidth(text, localFont);

	// レイアウト状態を初期化
	LayoutState state;
	state.lineCount = UI::LINE_COUNT_START;
	state.headPos = UI::HEAD_POSITION_START;

	// 各文字を処理
	for (size_t i = 0; i < text.size(); ++i)
	{
		const char32 ch = text[i];

		// 特殊文字処理（改行、位置調整など）
		if (processSpecialCharacter(ch, localFont, lineWidth, state, out))
		{
			continue;
		}

		const bool isSpace = (ch == U' ' || ch == U'　');

		// スペースでトークンを区切る
		if (!isSpace)
		{
			state.token.push_back(ch);
			continue;
		}

		// 単語を処理してスペース処理
		processWord(state.token, localFont, state, out);
		state.token.clear();
		processSpace(localFont, lineWidth, state, out);
	}

	// 残りの単語を処理
	if (!state.token.isEmpty())
	{
		processWord(state.token, localFont, state, out);
	}

	// 最終調整
	finalizeLayout(out, localFont);

	return out;
}

double TextLayouter::calculateMaxLineWidth(const String& text,
										   const Font& font) const
{
	double lineWidth = 0;
	for (size_t i = 0; i < text.size(); ++i)
	{
		lineWidth =
			Max(lineWidth, font.getGlyph(text[i]).xAdvance * m_lineWidthScale);
	}
	return lineWidth;
}

bool TextLayouter::processSpecialCharacter(char32 ch, const Font& font,
										   double lineWidth, LayoutState& state,
										   Array<LayoutChar>& out) const
{
	// 強制改行
	if (ch == U'*')
	{
		processWord(state.token, font, state, out);
		state.token.clear();
		InsertBreakInText(state.text_y, state.text_x, lineWidth,
						  static_cast<double>(font.height()), state.lineCount,
						  state.headPos, state.breakablePositions);
		return true;
	}

	// 強制y位置を戻す
	if (ch == U'^')
	{
		state.text_y = static_cast<double>(font.height()) *
					   (state.headPos - UI::BACK_POSITION_OFFSET - 1);
		state.headPos -= UI::BACK_POSITION_OFFSET;
		return true;
	}

	return false;
}

void TextLayouter::processSpace(const Font& font, double lineWidth,
								LayoutState& state,
								Array<LayoutChar>& out) const
{
	// 改行判定（現在は常に改行）
	const bool needBreak = true;
	if (needBreak)
	{
		InsertBreakInText(state.text_y, state.text_x, lineWidth,
						  static_cast<double>(font.height()), state.lineCount,
						  state.headPos, state.breakablePositions);
	}

	// スペースボックスの表示（設定による）
	if (UI::DISPLAY_SPACE_BOX)
	{
		const auto gi = font.getGlyph(U' ');
		RectF box{Arg::topLeft = Vec2{state.text_x, state.text_y}, gi.xAdvance,
				  static_cast<double>(font.height())};
		out << LayoutChar{U' ', Vec2{state.text_x, state.text_y}, box,
						  state.index++};
		state.text_y += static_cast<double>(font.height());
	}

	// 改行候補に登録
	state.breakablePositions << out.size();
}

void TextLayouter::processWord(const String& word, const Font& font,
							   LayoutState& state, Array<LayoutChar>& out) const
{
	for (const char32 ch : word)
	{
		const auto gi = font.getGlyph(ch);
		RectF box{Arg::topLeft = Vec2{state.text_x, state.text_y}, gi.xAdvance,
				  static_cast<double>(font.height())};
		out << LayoutChar{ch, Vec2{state.text_x, state.text_y}, box,
						  state.index++};
		state.text_y += static_cast<double>(font.height());
	}
}

void TextLayouter::finalizeLayout(Array<LayoutChar>& out,
								  const Font& font) const
{
	// 各glyphのadvanceからboxを更新
	for (auto& lc : out)
	{
		const auto gi = font.getGlyph(lc.ch);
		lc.box = RectF{Arg::topLeft = lc.pos, gi.xAdvance,
					   static_cast<double>(font.height())};
	}
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
