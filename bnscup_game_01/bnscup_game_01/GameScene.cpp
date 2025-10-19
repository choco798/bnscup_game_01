#include "stdafx.h"

#include "GameScene.hpp"

GameScene::GameScene(GameState& state, Renderer& renderer, SoundManager& sound,
					 Config& config, Font& font)
	: m_state{state},
	  m_renderer{renderer},
	  m_sound{sound},
	  m_config{config},
	  m_font(font)
{
	startProblem();
}

void GameScene::startProblem()
{
	m_showExplanation = false;
	m_result = false;

	if (m_state.currentIndex >= m_state.problems.size())
	{
		return;
	}

	const auto& ui = m_config.ui();
	TextLayouter layouter{m_font, ui.maxLineWidth, ui.lineHeightScale, ui.lineWidthScale};
	m_chars = layouter.layout(m_state.problems[m_state.currentIndex].text);

	// 俳句表示の開始位置（左上）にオフセットを与える
	const Vec2 base{80, 120};
	for (auto& c : m_chars)
	{
		c.pos += base;
		c.box.moveBy(base);
	}
}

void GameScene::update()
{
	handleClick();
}

void GameScene::draw() const
{
	m_renderer.drawBackground();

	if (m_state.currentIndex >= m_state.problems.size())
	{
		m_font(U"問題がありません").drawAt(Scene::Center(), Palette::Black);
		return;
	}

	// 俳句本文
	m_renderer.drawHaiku(m_chars);

	// 季語なしボタン（簡易）
	{
		const RoundRect btn{RectF{960, 140, 240, 60}, 16};
		btn.draw(Palette::White).drawFrame(3, 0, Palette::Black);
		m_font(U"季語なし").drawAt(btn.rect.center(), Palette::Black);
	}

	// 先生リアクション
	if (!m_state.answered)
	{
		m_renderer.drawTeacherNormal();
	}
	else
	{
		if (m_result)
		{
			m_renderer.drawTeacherHappy();
		}
		else
		{
			m_renderer.drawTeacherAngry();
		}
	}

	// 解説
	if (m_showExplanation)
	{
		m_renderer.drawExplanation(
			m_state.problems[m_state.currentIndex].explanation);
	}
}

RectF GameScene::Inflate(const RectF& r, double padPx, double padScale)
{
	const double pad = padPx + (r.w * padScale);
	return RectF{r.x - pad, r.y - pad, r.w + pad * 2.0, r.h + pad * 2.0};
}

bool GameScene::isHitKigo() const
{
	const auto& prob = m_state.problems[m_state.currentIndex];
	if (!prob.hasKigo)
	{
		return false;
	}

	const auto& ui = m_config.ui();

	for (int32 i = prob.kigoStart; i <= prob.kigoEnd; ++i)
	{
		const RectF inflated =
			Inflate(m_chars[i].box, ui.hitboxPaddingPx, ui.hitboxPaddingScale);
		if (inflated.mouseOver() && MouseL.down())
		{
			return true;
		}
	}
	return false;
}

void GameScene::handleClick()
{
	if (m_state.currentIndex >= m_state.problems.size())
	{
		return;
	}

	if (!MouseL.down())
	{
		return;
	}

	auto& prob = m_state.problems[m_state.currentIndex];

	// 季語ヒット
	if (isHitKigo())
	{
		m_state.answered = true;
		m_result = prob.hasKigo;  // hasKigo=true のときだけここに来る設計
		if (m_result)
		{
			m_sound.playCorrect();
			m_state.score += 10;
			m_state.correctCount += 1;
			m_showExplanation = true;
		}
		else
		{
			m_sound.playWrong();
		}
		return;
	}

	// 季語なしボタンの簡易チェック
	{
		const RectF btn{960, 140, 240, 60};
		if (btn.mouseOver())
		{
			m_state.answered = true;
			m_result = (!prob.hasKigo);
			if (m_result)
			{
				m_sound.playCorrect();
				m_state.score += 10;
				m_state.correctCount += 1;
				m_showExplanation = true;
			}
			else
			{
				m_sound.playWrong();
			}
			return;
		}
	}

	// 解説表示中にクリックで次の問題へ
	if (m_state.answered && m_showExplanation)
	{
		m_state.state_next();
		startProblem();
	}
}
