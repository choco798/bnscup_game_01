#include "UiButton.hpp"

ui::Button::Button(const StringView& text, const StringView& fontAssetKey,
						  const Vec2& pos)
	: m_text(text), m_fontKey(fontAssetKey), m_pos(pos)
{
	update();
}

// 配置・テキスト変更
ui::Button& ui::Button::setPos(const Vec2& pos)
{
	m_pos = pos;
	return *this;
}

ui::Button& ui::Button::setText(const String& text)
{
	m_text = text;
	return *this;
}

ui::Button& ui::Button::setEnabled(bool enabled)
{
	m_enabled = enabled;
	return *this;
}

ui::Button& ui::Button::setStyle(const ButtonStyle& s)
{
	m_style = s;
	return *this;
}

// 今フレームの入力を処理し、クリックしたら true を返す
bool ui::Button::update()
{
	m_state = m_enabled ? ButtonState::Idle : ButtonState::Disabled;

	// レイアウト（毎フレームでも軽いです。必要なら dirty フラグ化）
	const RectF label = FontAsset(m_fontKey)(m_text).regionAt(m_pos);
	m_rect = label.stretched(m_style.padding).rounded(m_style.cornerRadius);

	if (!m_enabled)
	{
		return false;
	}

	const bool over = m_rect.mouseOver();
	const bool pressedNow = MouseL.pressed();
	const bool wentDown = MouseL.down();
	const bool wentUp = MouseL.up();

	if (over)
	{
		if (pressedNow)
		{
			m_state = ButtonState::Pressed;
		}
		else
		{
			m_state = ButtonState::Hover;
		}
	}

	// 押下開始がボタン内だったかを記録（外で押して入ってきた誤クリックを防止）
	if (wentDown)
	{
		m_pressedOriginInside = over;
	}

	// 「内で押し始め、内で離した」時のみ click
	const bool clicked = (wentUp && over && m_pressedOriginInside);
	if (wentUp)
	{
		m_pressedOriginInside = false;
	}
	return m_preFrameClicked = clicked;
}

// 描画だけ分離
void ui::Button::draw() const
{
	const ColorF fill = [&]
	{
		switch (m_state)
		{
			case ButtonState::Hover:
				return m_style.colorHover;
			case ButtonState::Pressed:
				return m_style.colorPress;
			case ButtonState::Disabled:
				return m_style.colorDisabled;
			default:
				return m_style.colorIdle;
		}
	}();

	m_rect.draw(fill).drawFrame(m_style.frameThickness, 0, m_style.colorFrame);

	FontAsset(m_fontKey)(m_text).drawAt(m_pos, (m_state == ButtonState::Disabled)
												 ? m_style.textDisabled
												 : m_style.textColor);
}

// ユースケースによっては状態や領域を外から参照したいことも
const RoundRect& ui::Button::roundRect() const
{
	return m_rect;
}

ui::ButtonState ui::Button::state() const
{
	return m_state;
}

bool ui::Button::isClicked() const
{
	return m_preFrameClicked;
}
