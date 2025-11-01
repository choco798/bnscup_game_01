#pragma once

// フリガナ付き文字を表現する構造体
struct RubyChar
{
	String baseText;	   // 漢字部分
	String rubyText;	   // フリガナ部分
	bool hasRuby = false;  // フリガナ有無
};

struct LayoutChar
{
	char32 ch{};			// 文字
	Vec2 pos{};				// 描画位置
	RectF box{};			// 当たり判定矩形
	int32 globalIndex{};	// text内インデックス
	Optional<String> ruby{};// フリガナテキスト
	Vec2 rubyPos{};			// フリガナ描画位置
};

// レイアウト処理の状態を管理する構造体
struct LayoutState
{
	double text_x = 0.0;
	double text_y = 0.0;
	int32 index = 0;
	int32 lineCount = 1;
	int32 headPos = 1;
	String token;
	Array<size_t> breakablePositions;
};

class TextLayouter
{
   private:
	const String m_fontName;
	double m_maxLineWidth;
	double m_lineHeightScale;
	double m_lineWidthScale;
	double m_clientSize;

	// 内部処理メソッド
	double calculateMaxLineWidth(const String& text, const Font& font) const;
	void processWord(const String& word, const Font& font, LayoutState& state,
					 Array<LayoutChar>& out) const;
	bool processSpecialCharacter(char32 ch, const Font& font, double lineWidth,
								 LayoutState& state,
								 Array<LayoutChar>& out) const;
	void processSpace(const Font& font, double lineWidth, LayoutState& state,
					  Array<LayoutChar>& out) const;
	void finalizeLayout(Array<LayoutChar>& out, const Font& font) const;

	// フリガナ関連メソッド
	Array<RubyChar> parseRubyText(const String& rubyText) const;
	void processWordWithRuby(const String& word, const Optional<String>& ruby,
							 const Font& font, LayoutState& state,
							 Array<LayoutChar>& out) const;
	Vec2 calculateRubyPosition(const Vec2& basePos, const String& ruby) const;

   public:
	TextLayouter(const StringView& fontName, double maxWidth,
				 double lineHeightScale, double lineWidthScale,
				 double clientSize);
	Array<LayoutChar> layout(const String& text) const;
	Array<LayoutChar> layoutWithRuby(const String& text,
									 const String& ruby) const;

	void InsertBreakInText(double& text_y, double& text_x, double lineWidth,
						   double localFontY, int32_t& lineCount,
						   int32_t& headPos,
						   s3d::Array<size_t>& breakablePositions) const;
};
