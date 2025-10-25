// UiButton.hpp
#pragma once
#include <Siv3D.hpp>

namespace ui
{
enum class ButtonState
{
	Idle,
	Hover,
	Pressed,
	Disabled,
};

struct ButtonStyle
{
	// 見た目
	double cornerRadius = 16.0;
	int32 frameThickness = 3;
	// 余白（フォント region を膨らませる）
	Vec2 padding = {12, 6};

	// 色
	ColorF colorIdle = Palette::White;
	ColorF colorHover = Palette::Silver;
	ColorF colorPress = ColorF{0.85};
	ColorF colorFrame = Palette::Black;
	ColorF textColor = Palette::Black;

	// 無効時
	ColorF colorDisabled = ColorF{0.9};
	ColorF textDisabled = Palette::Gray;
};

class Button
{
   public:
	Button(const String& text = U"", const String& fontAssetKey = U"",
		   const Vec2& pos = Vec2{0, 0});

	// 配置・テキスト変更
	Button& setPos(const Vec2& pos);
	Button& setText(const String& text);
	Button& setEnabled(bool enabled);
	Button& setStyle(const ButtonStyle& s);

	// 今フレームの入力を処理し、クリックしたら true を返す
	bool update();

	// 描画だけ分離
	void draw() const;

	// ユースケースによっては状態や領域を外から参照したいことも
	const s3d::RoundRect& roundRect() const;
	ButtonState state() const;

	bool isClicked() const;

   private:
	String m_text;
	String m_fontKey;
	Vec2 m_pos;
	ButtonStyle m_style{};
	bool m_enabled = true;

	mutable s3d::RoundRect m_rect{RectF{0, 0, 0, 0}, m_style.cornerRadius};
	ButtonState m_state = ButtonState::Idle;

	bool m_pressedOriginInside = false;
	bool m_preFrameClicked = false;
};
}  // namespace ui
