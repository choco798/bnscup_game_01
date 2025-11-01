#include "GameScene.hpp"

// ローカル関数定義

ColorF GeneratePastelColor()
{
	// HSV空間でランダムに生成（パステル調にする）
	double hue = Random(0.0, 360.0);	   // 色相：全範囲
	double saturation = Random(0.2, 0.3);  // 彩度：低め（0〜1）
	double value = Random(0.9, 1.0);	   // 明度：高め（明るい色）

	return HSV{hue, saturation, value};
}

void DrawRadialFadeCircle(const Circle& c, const ColorF& base, int steps = 16)
{
	double prevR = 0.0;
	for (int i = 1; i <= steps; ++i)
	{
		double t = static_cast<double>(i) / steps;	// 0 → 1（中心→外周）
		double r = c.r * t;							// 現在の半径
		double thickness = r - prevR;				// リング幅（重ならない）
		// 線形フェード：中心1.0 → 外周0.0
		double a = 1.0 - t;

		// ちょっと柔らかく（スムースステップ）
		a = a * a * (3.0 - 2.0 * a);

		Circle(c.center, r).drawFrame(thickness, 0.0, ColorF(base, base.a * a));
		prevR = r;
	}
}

// 0〜1に丸める補助
static inline double frac(double x)
{
	return x - std::floor(x);
}

// 方向ヒント（風・粒子の流れ）：t01 ∈ [0,1] を進行度として受け取り描画する
// area: 描画エリア / dir: 流れの方向ベクトル / t01: 0〜1の時間 / count: 粒子数
void DrawFlowHintPastel(const RectF& area, Vec2 dir, double t01,
						size_t count = 300)
{
	// デフォルト方向（未指定なら右向き）
	if (dir.isZero())
	{
		dir = Vec2{1, 0};
	}
	const Vec2 nDir = dir.normalized();

	// 流れの見た目チューニング
	const double baseSpeed = 180.0;	   // 基本スピード（px / 周期）
	const double swirlScale = 10.0;	   // 横揺れの大きさ
	const double swirlFreq = 6.28318;  // 横揺れの周波数（2π）
	const double trailLength = 26.0;   // 軌跡の長さ
	const double alphaBase =
		(t01 < 0.1) ? t01 * 10 * 0.26
					: ((0.9 < t01) ? (1 - t01) * 10 * 0.26
								   : 0.26);	 // 粒子の基本アルファ（やわらかく）
	const double satMin = 0.28, satMax = 0.45;	// パステル彩度
	const double valMin = 0.95, valMax = 1.00;	// パステル明度

	// 直交方向（横揺れ用）
	const Vec2 ortho = Vec2{-nDir.y, nDir.x};

	for (size_t i = 0; i < count; ++i)
	{
		// 擬似乱数（インデックスのみから決まる → 純粋）
		const double r0 = frac(std::sin((i + 0.5) * 12.9898) * 43758.5453);
		const double r1 = frac(std::sin((i + 2.5) * 78.2330) * 12345.6789);
		const double r2 = frac(std::sin((i + 4.5) * 34.1234) * 98765.4321);
		const double r3 = frac(std::sin((i + 6.5) * 19.9753) * 54321.1234);

		// 進行度に対する開始オフセット（ループ感を出す）
		const double phase = frac(r0 + t01);

		// エリア内の初期位置（決定的に散らす）
		Vec2 basePos{area.x + area.w * frac(r1 + 0.37 * i),
					 area.y + area.h * frac(r2 + 0.61 * i)};

		// 進行に応じた移動量（エリア対角を基準に大きさ調整）
		const double loopDist = baseSpeed;	// 1 周期での移動距離（px）
		Vec2 advect = nDir * (loopDist * phase);

		// 横揺れ（やわらかい流れ感）
		double wobble = std::sin((r3 * 7.0 + phase) * swirlFreq);
		Vec2 offset = ortho * (wobble * swirlScale);

		// 位置合成 → エリア内にラップ（はみ出しをループ）
		Vec2 p = basePos + advect + offset;

		auto wrap = [](double v, double a, double b)
		{
			const double w = (b - a);
			v = std::fmod((v - a), w);
			if (v < 0) v += w;
			return v + a;
		};

		p.x = wrap(p.x, area.x, area.x + area.w);
		p.y = wrap(p.y, area.y, area.y + area.h);

		// パステル色（HSV）：色相は全域、彩度低め、明度高め
		const double hue = 360.0 * r1;
		const double sat = Math::Lerp(satMin, satMax, r2);
		const double val = Math::Lerp(valMin, valMax, r3);

		// 粒子サイズとアルファ（中心ほど濃く、端は淡く）
		const double size = Math::Lerp(4.0, 8.0, r0);
		const double a =
			alphaBase *
			(0.75 + 0.25 * std::cos(phase * Math::Pi * 2));	 // ゆるく呼吸する

		const ColorF col = HSV{hue, sat, val, a};

		// 軌跡（やわらかい流れ方向のヒント）
		const Vec2 tail = p - nDir * trailLength;
		(void)tail;
		// Line{tail, p}.draw(2.0, col);
		Circle{p, size}.draw(col);
	}
}

////////////////////////
// ここからGameScene
////////////////////////
GameScene::GameScene(const InitData& init) : IScene(init)
{
	getData().sound.stopBGM();
	startProblem();

	m_noKigoBtn = ui::Button(U"季語なし", U"Game", Vec2(960, 140));
}

void GameScene::startProblem()
{
	m_showExplanation = false;
	m_result = false;

	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		return;
	}

	const auto& ui = getData().configManager.ui();
	TextLayouter layouter{U"Game", ui.maxLineWidth, ui.lineHeightScale,
						  ui.lineWidthScale,
						  static_cast<double>(ui.clientSizeX)};
	m_chars = layouter.layout(
		getData().gameState.problems[getData().gameState.currentIndex].text);

	// 俳句表示の開始位置（左上）にオフセットを与える
	const Vec2 base{ui.clientSizeX / 2, 60};
	for (auto& c : m_chars)
	{
		c.pos += base;
		c.box.moveBy(base);
	}
}

void GameScene::update()
{
	// ミスクリック時に方向を示す
	if (m_flowTime > 0.0f)
	{
		m_flowTime -= Scene::DeltaTime() / 3.0f;
	}
	m_noKigoBtn.update();

	const bool disableInput = getData().showHowToPlay;
	if (disableInput)
	{
		return;
	}

	handleClick();
	// GameScene 内部で問題を進め、末尾まで到達したら結果へ
	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		changeScene(State::Result);
		return;
	}
}

void GameScene::draw() const
{
	Reseed(getData().gameState.currentIndex);

	getData().renderer.drawBackground();

	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		FontAsset(U"Game")(U"終了！").drawAt(Scene::Center(), Palette::Black);
		return;
	}

	// チュートリアルに関しては特別扱い
	const bool isTutorial = (getData().gameState.currentIndex == 0);

	if (m_showExplanation || isTutorial)
	{
		// 季語をハイライトする
		drawKigoRect();
	}
	else
	{
		// 俳句の背景に色を付ける
		drawHiakuRect();
	}

	// ミスクリック時に方向を示す
	if (m_flowTime > 0.0f)
	{
		DrawFlowHintPastel(RectF(getData().configManager.ui().clientSizeX,
								 getData().configManager.ui().clientSizeY),
						   (m_flowStartPos - getKigoRectCenter()), m_flowTime);
	}

	// 俳句本文
	getData().renderer.drawHaiku(m_chars);

	// 季語なしボタン（簡易）
	m_noKigoBtn.draw();

	// 先生リアクション
	if (!getData().gameState.answered)
	{
		getData().renderer.drawTeacherNormal();
	}
	else
	{
		if (m_result)
		{
			getData().renderer.drawTeacherHappy();
		}
		else
		{
			getData().renderer.drawTeacherAngry();
		}
	}

	// 解説
	if (m_showExplanation)
	{
		getData().renderer.drawExplanation(
			getData().gameState.problems[getData().gameState.currentIndex].kigo,
			getData()
				.gameState.problems[getData().gameState.currentIndex]
				.getSeason(),
			getData()
				.gameState.problems[getData().gameState.currentIndex]
				.explanation);
	}
	else
	{
		// チュートリアルテキスト
		if (isTutorial)
		{
			getData().renderer.drawTutorial(
				U"俳句の中の季語を見つけていく（クリックする）ゲームです。\n言"
				U"葉の芯を楽しんでください！！！");
		}
	}
}

void GameScene::drawKigoRect() const
{
	const auto& prob =
		getData().gameState.problems[getData().gameState.currentIndex];
	if (prob.hasKigo)
	{
		const auto& ui = getData().configManager.ui();
		for (int32 i = prob.kigoStart; i < prob.kigoEnd; ++i)
		{
			drawWordRect(i, ui);
		}
	}
}

void GameScene::drawHiakuRect() const
{
	const auto& ui = getData().configManager.ui();
	for (int32 i = 0; i < m_chars.size(); ++i)
	{
		drawWordRect(i, ui);
	}
}

void GameScene::drawWordRect(s3d::int32 i, const UIConfig& ui) const
{
	const bool isSpace = (m_chars[i].ch == U' ' || m_chars[i].ch == U'　');
	if (isSpace)
	{
		return;
	}

	const RectF inflated =
		Inflate(m_chars[i].box, ui.hitboxPaddingPx, ui.hitboxPaddingScale);
	// inflated.draw(Palette::White);

	// 放射状グラデーション（中心が濃く、外側が薄い）
	Circle rectCenter{inflated.center(), (inflated.w + inflated.h) / 2};
	DrawRadialFadeCircle(rectCenter, ColorF(GeneratePastelColor(), 0.5));
}

Vec2 GameScene::getKigoRectCenter() const
{
	Vec2 result{};
	int rectCount{};

	const auto& prob =
		getData().gameState.problems[getData().gameState.currentIndex];
	if (prob.hasKigo)
	{
		const auto& ui = getData().configManager.ui();
		for (int32 i = prob.kigoStart; i < prob.kigoEnd; ++i)
		{
			const bool isSpace =
				(m_chars[i].ch == U' ' || m_chars[i].ch == U'　');
			if (isSpace)
			{
				continue;
			}

			const RectF inflated = Inflate(m_chars[i].box, ui.hitboxPaddingPx,
										   ui.hitboxPaddingScale);
			result += inflated.center();
			++rectCount;
		}
	}

	return result / rectCount;
}

RectF GameScene::Inflate(const RectF& r, double padPx, double padScale)
{
	const double pad = padPx + (r.w * padScale);
	return RectF{r.x - pad, r.y - pad, r.w + pad * 2.0, r.h + pad * 2.0};
}

bool GameScene::isHitKigo() const
{
	const auto& prob =
		getData().gameState.problems[getData().gameState.currentIndex];
	if (!prob.hasKigo)
	{
		return false;
	}

	const auto& ui = getData().configManager.ui();

	for (int32 i = prob.kigoStart; i < prob.kigoEnd; ++i)
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
	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		return;
	}

	auto& prob = getData().gameState.problems[getData().gameState.currentIndex];

	if (!MouseL.down())
	{
		return;
	}

	// 季語ヒット
	if (isHitKigo())
	{
		getData().gameState.answered = true;
		m_result = prob.hasKigo;  // hasKigo=true のときだけここに来る設計
		if (m_result)
		{
			ExecCorrect();
		}
		else
		{
			ExecWrong();
		}
		return;
	}

	// 季語なしボタンの簡易チェック
	if (m_noKigoBtn.roundRect().mouseOver())
	{
		getData().gameState.answered = true;
		m_result = (!prob.hasKigo);
		if (m_result)
		{
			ExecCorrect();
		}
		else
		{
			ExecWrong();
		}
		return;
	}

	// 解説表示中にクリックで次の問題へ
	if (getData().gameState.answered && m_showExplanation)
	{
		getData().gameState.state_next();
		startProblem();
	}
	else
	{
		// 何もないところをクリックすると間違い扱いにする
		ExecWrong();
	}
}

void GameScene::ExecWrong()
{
	getData().sound.playWrong();
	m_flowTime = 1.0f;
	m_flowStartPos = Cursor::Pos();
}

void GameScene::ExecCorrect()
{
	getData().sound.playCorrect();
	// 連打したときに音は鳴らすけど、正解数カウントは上げないようにする
	if (m_showExplanation)
	{
		getData().gameState.score +=
			10 * (getData()
					  .gameState.problems[getData().gameState.currentIndex]
					  .grade +
				  1);
		getData().gameState.correctCount += 1;
		m_showExplanation = true;
	}
}
